open Types;

/**
 * TickLength is 1 minute. I guess? Doesn't make for as nice of an animation.
 * Maybe I'll have a "multiplier" that gets applied to... everything?
 * Or a function that is "minutes"
 */
let closingTime = minutesToTicks(17. -. 9. *. 60.);

let idGenerator = () => {
  let last = ref(0);
  () => {
    last := last^ + 1;
    last^;
  };
};

open Belt.Map.Int;

let makeMapping = edges => {
  edges->reduce(empty, (mapping, eid, edge: Types.Map.edge) =>
    mapping
    ->set(edge.source, [eid, ...mapping->getWithDefault(edge.source, [])])
    ->set(edge.dest, [eid, ...mapping->getWithDefault(edge.dest, [])])
  );
};

let pointToPoint = edges => {
  Belt.Map.Int.(
    edges->reduce(empty, (p2p, _, edge: Types.Map.edge) =>
      p2p->set(
        edge.source,
        p2p->getWithDefault(edge.source, empty)->set(edge.dest, edge.id),
      )
      ->set(
        edge.dest,
        p2p->getWithDefault(edge.dest, empty)->set(edge.source, edge.id),
      )
    )
  );
};

let processPersonUpdate = (world, person, trail, update: Types.personUpdate) => {
  // Js.log2("Person update", update)
  let person =
    switch (update) {
    | AddGoal(goal) => {...person, goals: [goal, ...person.goals]}
    | SetPosition(position) => {...person, position}
    | RemoveGoal(_)
    | Observe(_) => person
    | _ =>
      Js.log2("Ignoring person update", update);
      person;
    };
  {
    ...person,
    experiences: [
      {
        startTime: world.clock,
        endTime: world.clock,
        goalTrail: trail,
        update,
        condition: person.condition,
      },
      ...person.experiences,
    ],
  };
};

let processUpdate = (world, update: Types.goalUpdate) => switch update.update {
  | Person(id, Remove) => 
    let person = world.people->getExn(id);
    let person = processPersonUpdate(world, person, update.trail, Remove);
    world.peopleWhoLeft = [person, ...world.peopleWhoLeft];
    world.people = world.people->remove(id)
  | Person(id, personUpdate) => 
    switch (world.people->get(id)) {
      | None => ()
      | Some(person) =>
        world.people = world.people->set(id, processPersonUpdate(
          world,
          person,
          update.trail,
          personUpdate
        ))
    }
  | Message(message) => {
    // Js.log(message);
    ()
  }
  | _ => failwith("nope")
}

let addPerson = world => {
  let id = world.genId();
  let exit = world.rng->Prando.choose(world.map.exits->Array.of_list);
  let edges = world.map.pointToEdges->getExn(exit);
  let edge = world.rng->Prando.choose(edges->Array.of_list);
  let person = Types.person(id, world.rng, {
    edge,
    progress: world.map.edges->getExn(edge).source == exit ? 0.0 : 1.0,
  }, world.clock);
  world.people =
    world.people
    ->set(id, person);
  switch (Goals.randomGoal(world, person)) {
    | None => ()
    | Some(goal) => 
    // Js.log(person.demographics.name ++ " decided to " ++ goal.name);
    world->processUpdate({trail: [], update: Updates.addGoal(id, goal)})
  };
};

let step = world => {
  world.clock = world.clock + 1;
  let numBuildings = world.map.buildings->Belt.Map.Int.size;
  let maxPeople = numBuildings * world.maxPeoplePerExhibit;
  if (world.people->Belt.Map.Int.size < maxPeople
      && world.rng->Prando.float < 0.04) {
    world->addPerson;
  };
  world.animals =
    world.animals
    ->map(animal =>
        if (animal.behaviorTimer <= 0) {
          {
            ...animal,
            behaviorTimer: world.rng->Prando.int(5, 50),
            behavior: world.rng->Prando.choose(Types.behaviors),
            visibility: world.rng->Prando.float,
          };
        } else {
          {...animal, behaviorTimer: animal.behaviorTimer - 1};
        }
      );
  let changes =
    world.people
    ->keysToArray
    ->Belt.Array.reduce([], (changes, pid) => {
        switch (world.people->get(pid)) {
        | None => changes
        | Some(person) =>
          let (person, updates) = Person.step(world, person);
          world.people = world.people->set(pid, person);
          changes @ updates;
        }
      });
  // Js.log2("Changes", List.length(changes))
  changes->Belt.List.forEach(world->processUpdate);
  ();
};