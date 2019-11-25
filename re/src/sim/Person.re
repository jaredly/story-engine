
let stepCondition = (condition: Types.condition, rng)  => {
  ...condition,
  thirst: min(1.0, condition.thirst +. (rng->Prando.range(0.01, 0.05))),
  hunger: min(1.0, condition.hunger +. (rng->Prando.range(0.01, 0.05))),
  stamina: max(0.0, condition.stamina -. rng->Prando.range(0.00001, 0.00005)),
}

let step = (world, person: Types.person) => {
  switch (person.goals) {
    | [] => (person, [])
    | [goal, ...rest] => {
      let (goal, updates, finished) = Goals.step(world, person, goal);
      let goals = finished ? rest : [goal, ...rest];
      let updates = goals == [] ? switch (Goals.randomGoal(world, person)) {
        | None => updates
        | Some(goal) => updates @ [(Some(person.demographics.name ++ " decided to " ++ goal.name), Types.Updates.addGoal(person.id, goal))]
      } : updates;
      ({...person, goals, condition: stepCondition(person.condition, world.rng)}, updates)
    }
  }
}