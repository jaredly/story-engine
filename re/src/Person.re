
let stepCondition = (condition: Types.condition, rng)  => {
  ...condition,
  thirst: min(1.0, condition.thirst +. (rng->Prando.range(0.01, 0.05))),
  hunger: min(1.0, condition.hunger +. (rng->Prando.range(0.01, 0.05))),
  stamina: max(0.0, condition.stamina -. rng->Prando.range(0.0001, 0.0005)),
  // stamina: max(0.0, condition.stamina -. 0.1),
}

let step = (world, person: Types.person) => {
  switch (person.goals) {
    | [] => (person, [])
    | [goal, ...rest] => {
      let (goal, updates, finished) = Goals.step(world, person, goal);
      let goals = finished ? rest : [goal, ...rest];
      let updates = goals == [] ? switch (Goals.randomGoal(world, person)) {
        | None => updates
        | Some(goal) => updates @ [(Some(person.name ++ " decided to " ++ goal.name), Types.Updates.addGoal(person.id, goal))]
      } : updates;
      ({...person, goals, condition: stepCondition(person.condition, world.rng)}, updates)
    }
  }
}