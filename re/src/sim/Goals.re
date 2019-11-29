open Types;

let step = (world, person, goal) =>
  if (goal.timer <= 1) {
    // Js.log("updating")
    switch (goal.updater) {
    | Goal(data, fn) =>
      let (ndata, result, updates) = fn(data, person, world);
      let updater = Goal(ndata, fn);
      let goal = {...goal, updater};
      // Js.log3("Updating", ndata, updates);
      switch (result) {
      | Failed(message) => (goal, updates, Some(Some(message)))
      | Succeeded(message) => (goal, updates, Some(None))
      | InProcess(timer) => ({...goal, timer}, updates, None)
      };
    };
  } else {
    ({...goal, timer: goal.timer - 1}, [], None);
  };

let speed = 4.0;

let advance = (pid, edge: Types.Map.edge, position) => {
  let speed = speed /. edge.length;
  let (progress, goal) =
    if (edge.source == pid) {
      (
        max(0.0, position.progress -. speed),
        0.0,
      );
    } else {
      (
        min(1.0, position.progress +. speed),
        1.0,
      );
    };
  ({...position, progress}, progress == goal);
};

let point = (position, edge: Types.Map.edge) =>
  position.progress < 0.5 ? edge.source : edge.dest;

let watchTheAnimals = (world, person, bid) => {
  let (name, terrain) =
    switch (world.map.buildings->Belt.Map.Int.getExn(bid).kind) {
    | Exhibit(_, name, terrain) => (name, terrain)
    | _ => assert(false)
    };
  Types.Goal(
    0,
    (time, person, world) =>
      // TODO check their patience, etc.
      if (time > 20) {
        (
          time,
          Succeeded(
            person.demographics.name ++ " was satisfied with the " ++ name,
          ),
          [],
        );
      } else {
        let num = world.rng->Prando.int(2, 10);
        (time + num, InProcess(num), []);
      },
  );
};

let goToPoint = (world, person, pid) => {
  let update = update => {update, trail: ["goToPoint"]};
  let path =
    Types.Map.findPath(world.map, Types.closestPoint(person, world.map), pid)
    ->Js.Null.toOption;
  switch (path) {
  | None => None
  | Some(path) =>
    let path = path->Array.to_list;
    Some({
      id: world.genId(),
      name: "go to a certain place",
      timer: 1,
      timeStarted: world.clock,
      updater:
        Types.Goal(
          path,
          (path, person, world) => {
            let edge =
              world.map.edges->Belt.Map.Int.getExn(person.position.edge);
            switch (path) {
            | [] => ([], Succeeded("Reached the goal!"), [])
            | [pid] =>
              let (position, reached) = advance(pid, edge, person.position);
              (
                [pid],
                reached ? Succeeded("Reached the goal!") : InProcess(1),
                [update(Updates.setPosition(person.id, position))],
              );
            | [pid, next, ...rest] =>
              let (position, reached) = advance(pid, edge, person.position);
              if (reached) {
                let eid =
                  world.map.pointToPoint
                  ->Belt.Map.Int.getExn(point(position, edge))
                  ->Belt.Map.Int.getExn(next);
                let edge = world.map.edges->Belt.Map.Int.getExn(eid);
                let progress = edge.source == next ? 1.0 : 0.0;
                let position = {edge: eid, progress};
                (
                  [next, ...rest],
                  InProcess(1),
                  [
                    update(Message(person.demographics.name ++ " turned")),
                    update(
                      Updates.setPosition(person.id, position),
                    ),
                  ],
                );
              } else {
                (
                  path,
                  InProcess(1),
                  [update(Updates.setPosition(person.id, position))],
                );
              };
            };
          },
        ),
    });
  };
};

let mapResult = (Goal(data, fn), map) =>
  Goal(
    data,
    (data, person, world) => {
      let (data, result, changes) = fn(data, person, world);
      (data, map(result), changes);
    },
  );

let mapSuccess = (goal, map) =>
  mapResult(goal, result =>
    switch (result) {
    | Succeeded(m) => map(m)
    | r => r
    }
  );

let addTrail = (trail, goalUpdate) => {...goalUpdate, trail: goalUpdate.trail @ trail};

let chainGoals = (Goal(adata, afn), Goal(bdata, bfn), trail, transitionMessage) =>
  Goal(
    (false, adata, bdata),
    ((second, adata, bdata), person, world) =>
      if (!second) {
        let (adata, result, changes) = afn(adata, person, world);
        let changes = changes->Belt.List.map(addTrail(trail));
        let (second, result, more) =
          switch (result) {
          | InProcess(c) => (false, InProcess(c), [])
          | Succeeded(m) => (
              true,
              InProcess(0),
              [{trail, update: Message(m)}, {trail, update: Message(transitionMessage)}],
            )
          | Failed(e) => (false, Failed(e), [])
          };
        ((second, adata, bdata), result, changes @ more);
      } else {
        let (bdata, result, changes) = bfn(bdata, person, world);
        let changes = changes->Belt.List.map(addTrail(trail));
        ((true, adata, bdata), result, changes);
      },
  );

let changeMessages = (goal, success, failure) =>
  mapResult(goal, result =>
    switch (result) {
    | Succeeded(_) => Succeeded(success)
    | Failed(_) => Failed(failure)
    | p => p
    }
  );

let leave =
  Goal(
    (),
    ((), person, _world) =>
      (
        (),
        Succeeded(person.demographics.name ++ " went home"),
        [{trail: ["leave"], update: Updates.removePerson(person.id)}],
      ),
  );

let goToExhibit = (world, person, building: Types.Map.building) => {
  switch (building.kind) {
  | Types.Map.Exhibit(animals, name, terrain) =>
    let goal = goToPoint(world, person, building.point);
    switch (goal) {
    | None => None
    | Some(goal) =>
      Some({
        ...goal,
        name: "go to the " ++ name,
        updater:
          chainGoals(
            changeMessages(
              goal.updater,
              person.demographics.name ++ " reached the " ++ name,
              person.demographics.name
              ++ " gave up trying to find the "
              ++ name,
            ),
            watchTheAnimals(world, person, building.id),
            ["goToExhibit"],
            "Now on to watching the animals",
          ),
      })
    };
  | _ => assert(false)
  };
};

let randomGoal = (world: Types.world, person: Types.person) =>
  if (person.condition.stamina <= 0.0) {
    let exit = world.rng->Prando.choose(world.map.exits->Array.of_list);
    switch (goToPoint(world, person, exit)) {
    | None => None
    | Some(goal) =>
      Some({
        ...goal,
        name: "leave the zoo",
        updater:
          chainGoals(
            changeMessages(
              goal.updater,
              person.demographics.name ++ " left the zoo",
              person.demographics.name ++ " gave up trying to leave the zoo",
            ),
            leave,
            ["goToExhibit"],
            "They got in their car",
          ),
      })
    };
  } else {
    let exhibit =
      world.rng
      ->Prando.choose(
          world.map.buildings
          ->Belt.Map.Int.valuesToArray
          ->Belt.Array.keep(building =>
              switch (building.kind) {
              | Exhibit(_) => true
              | _ => false
              }
            ),
        );
    goToExhibit(world, person, exhibit);
  };