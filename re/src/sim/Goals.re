open Types;

let step = ((world, person), goal) =>
  if (goal.timer <= 1) {
    let Goal(inner) = goal.contents;
    let (ndata, result, updates) = inner.updater(inner.state, person, world);
    let contents = Goal({...inner, state: ndata});
    let goal = {...goal, contents};
    switch (result) {
    | Failed(message) => (goal, (updates, Some(Some(message))))
    | Succeeded(message, result) => (
        {...goal, result: Some(result)},
        (updates, Some(None)),
      )
    | InProcess(timer) => ({...goal, timer}, (updates, None))
    };
  } else {
    ({...goal, timer: goal.timer - 1}, ([], None));
  };

let step = (world, person, goal) => {
  mapGoal(goal, (world, person), {run: step})
}

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

let watchTheAnimals = (bid, person, world) => {
  let (aids, name, terrain) =
    switch (world.map.buildings->Belt.Map.Int.getExn(bid).kind) {
    | Exhibit(aids, name, terrain) => (aids, name, terrain)
    | _ => assert(false)
    };
  let kind = "watchTheAnimals";
  {
    kind,
    state: (0, 0, world.rng->Prando.int(20, 50)),
    updater: 
    ((time, enjoyment, maxTime), person, world) => {
      let interestingAnimals = aids
      ->Belt.Set.Int.toList
      ->Belt.List.keepMap(world.animals->Belt.Map.Int.get)
      // TODO base this on the person's attentiveness / concentration
      // Also if they've been concentrating for a few minutes, they have
      // a random chance of spotting a mostly-hidden animal.
      ->Belt.List.keep(animal => animal.visibility > 0.2);
      let updates = if (interestingAnimals == []) {
        [
          {update: Person(person.id, Observe(NoAnimals)), trail: [kind]}
        ]
      } else {
        let map = Types.emptyAnimalBehaviorMap();
        let animalsByBehavior = interestingAnimals->Belt.List.reduce(
          map,
          (map, animal) => map->Belt.Map.set(animal.behavior, switch (map->Belt.Map.get(animal.behavior)) {
            | None => 1
            | Some(n) => n + 1
          })
        );
        let a = interestingAnimals->List.hd;

        let actions = animalsByBehavior->Belt.Map.toList->Belt.List.map(((k, v)) => {
          (v, k)
        });
        [
          {update: Person(person.id, Observe(AnimalActions(a.kind, actions))), trail: [kind]}
        ]
      };
      // let updates = interestingAnimals->Belt.List.map(a => {
      //   {update: Person(person.id, Observe(AnimalActions(a.kind, 1, a.behavior))), trail: [kind]}
      // });
      let enjoyment = enjoyment + interestingAnimals->Belt.List.length;
      // TODO check their patience, etc.
      if (time > maxTime) {
        // Js.log3(name, time, enjoyment);
        (
          (time, enjoyment, maxTime),
          Succeeded(
            person.demographics.name ++ " was satisfied with the " ++ name,
            enjoyment > 10 ? Satisfied : Unsatisfied("no interesting animals")
          ),
          updates,
        );
      } else {
        let num = world.rng->Prando.int(1, 4);
        ((time + num, enjoyment, maxTime), InProcess(num), updates);
      }
    }
  };
};

let goToPoint = (world, person, pid): option(Types.singleGoal('a, 'b)) => {
  let update = update => {update, trail: ["goToPoint"]};
  let path =
    Types.Map.findPath(world.map, Types.closestPoint(person, world.map), pid)
    ->Js.Null.toOption;
  switch (path) {
  | None => None
  | Some(path) =>
    let path = path->Array.to_list;
    Some({
          kind: "goToPoint",
          state: path,
          updater: (path, person, world) => {
            let edge =
              world.map.edges->Belt.Map.Int.getExn(person.position.edge);
            switch (path) {
            | [] => ([], Succeeded("Reached the goal!", ()), [])
            | [pid] =>
              let (position, reached) = advance(pid, edge, person.position);
              (
                [pid],
                reached ? Succeeded("Reached the goal!", ()) : InProcess(1),
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
        // })
    });
  };
};

let mapResult = (inner, map) =>
  {...inner,
    updater: (data, person, world) => {
      let (data, result, changes) = inner.updater(data, person, world);
      (data, map(result), changes);
    },
  };

let mapSuccess = (goal, map) =>
  mapResult(goal, result =>
    switch (result) {
    | Succeeded(m, res) => map(m, res)
    | r => r
    }
  );

let addTrail = (trail, goalUpdate) => {...goalUpdate, trail: goalUpdate.trail @ trail};

let chainGoals = (kind, agoal, bgoalMaker, transitionMessage) =>
  {
    kind,
    state: `First(agoal),
    updater: (state, person, world) =>
      switch state {
        | `First(agoal) =>
        let (adata, result, changes) = agoal.updater(agoal.state, person, world);
        let changes = changes->Belt.List.map(addTrail([kind]));
        let (state, result, more) =
          switch (result) {
          | InProcess(c) => (`First({...agoal, state: adata}), InProcess(c), [])
          | Succeeded(m, intermediate) =>
            let bgoal = bgoalMaker(intermediate, person, world);
            (
              `Second(bgoal),
              InProcess(0),
              [{trail: [kind], update: Message(m)}, {trail: [kind], update: Message(transitionMessage)}],
            )
          | Failed(e) => (`First({...agoal, state: adata}), Failed(e), [])
          };
          (state, result, changes @ more);
        | `Second(bgoal) =>
        let (bdata, result, changes) = bgoal.updater(bgoal.state, person, world);
        let changes = changes->Belt.List.map(addTrail([kind]));
        (`Second({...bgoal, state: bdata}), result, changes);
      },
  }

let changeMessages = (goal, success, failure) =>
  mapResult(goal, result =>
    switch (result) {
    | Succeeded(_, intermediate) => Succeeded(success, intermediate)
    | Failed(_) => Failed(failure)
    | p => p
    }
  );

let leave =
  {
    kind: "leave",
    state: (),
    updater: ((), person, _world) =>
      (
        (),
        Succeeded(person.demographics.name ++ " went home", ()),
        [{trail: ["leave"], update: Updates.removePerson(person.id)}],
      ),
  };

let goToExhibit = (world, person, building: Types.Map.building) => {
  switch (building.kind) {
  | Types.Map.Exhibit(animals, name, terrain) =>
    let goal = goToPoint(world, person, building.point);
    switch (goal) {
    | None => None
    | Some(inner) =>
      // let Goal(inner) = goal;
      Some(GoToExhibit({
        id: world.genId(),
        timeStarted: world.clock,
        timer: 0,
        result: None,
        attrs: building.id,
        // kind: GoToExhibit(building.id, None),
        name: "go to the " ++ name,
        contents:
          Goal(chainGoals(
            "goToExhibit",
            changeMessages(
              inner,
              person.demographics.name ++ " reached the " ++ name,
              person.demographics.name
              ++ " gave up trying to find the "
              ++ name,
            ),
            () => watchTheAnimals(building.id),
            "Now on to watching the animals",
          )),
      }))
    };
  | _ => assert(false)
  };
};

let chooseExhibit = (world: Types.world, person: Types.person) => {
  let (satisfied, unsatisfied, failed) = person.pastGoals->Belt.List.reduce((Belt.Set.Int.empty, Belt.Map.Int.empty, Belt.Set.Int.empty), ((satisfied, unsatisfied, failed), {goal}) => {
    switch (goal) {
      | GoToExhibit({attrs: id, result: Some(Unsatisfied(reason))}) => (satisfied, unsatisfied->Belt.Map.Int.set(id, reason), failed)
      | GoToExhibit({attrs: id, result: Some(Satisfied)}) => (satisfied->Belt.Set.Int.add(id), unsatisfied, failed)
      | GoToExhibit({attrs: id, result: None}) => (satisfied, unsatisfied, failed->Belt.Set.Int.add(id))
      | _ => (satisfied, unsatisfied, failed)
    }
  });
  let lastExhibit = switch (person.pastGoals) {
    | [{goal: GoToExhibit({attrs: id})}, ..._] => Some(id)
    | _ => None
  };
  Js.log2("last", lastExhibit);

  let exhibitsLeft =
    world.map.buildings
    ->Belt.Map.Int.valuesToArray
    ->Belt.Array.keepMap(building =>
        switch (building.kind) {
        | Exhibit(_) =>
          if (lastExhibit == Some(building.id)) {
            None;
          } else if (satisfied->Belt.Set.Int.has(building.id)) {
            None;
          } else {
            switch (unsatisfied->Belt.Map.Int.get(building.id)) {
            | Some(reason) => Some((building, `Unsatisfied(reason)))
            | None =>
              if (failed->Belt.Set.Int.has(building.id)) {
                Some((building, `TryAgain));
              } else {
                Some((building, `NeverBeen));
              }
            };
          }
        | _ => None
        }
      );

  if (exhibitsLeft->Belt.Array.length == 0) {
    None
  } else {
    Some(world.rng->Prando.choose(exhibitsLeft))
  }
};

let leaveGoal = (world, person) => {
    let exit = world.rng->Prando.choose(world.map.exits->Array.of_list);
    switch (goToPoint(world, person, exit)) {
    | None => None
    | Some(goal) =>
  Some(Leave({
    id: world.genId(),
    timeStarted: world.clock,
    timer: 0,
    // kind: Leave,
    attrs: exit,
    result: None,
    name: "leave the zoo",
    contents:
      Goal(
        chainGoals(
          "leaveTheZoo",
          changeMessages(
            goal,
            person.demographics.name ++ " left the zoo",
            person.demographics.name ++ " gave up trying to leave the zoo",
          ),
          ((), _, _) => leave,
          "They got in their car",
        ),
      ),
  }));
    };
};

let randomGoal = (world: Types.world, person: Types.person) =>
  if (person.condition.stamina <= 0.0) {
      // let Goal(inner) = goal.contents;
      leaveGoal(world, person)
  } else {
    // let exhibits = 
    //   world.map.buildings
    //   ->Belt.Map.Int.valuesToArray
    //   ->Belt.Array.keep(building =>
    //       switch (building.kind) {
    //       | Exhibit(_) => true
    //       | _ => false
    //       }
    //     );
    // let exhibit =
    //   world.rng
    //   ->Prando.choose(exhibits);
    switch (chooseExhibit(world, person)) {
      | None =>
        Js.log("Leaving");
        leaveGoal(world, person)
      | Some((id, reason)) => {
        Js.log2("Chose", id);
        goToExhibit(world, person, id);
      }
    }
  };