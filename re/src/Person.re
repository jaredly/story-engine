
let step = (world, person: Types.person) => {
  switch (person.goals) {
    | [] => (person, [])
    | [goal, ...rest] => {
  // Js.log2("step", goal)
      let (goal, updates, finished) = Goals.step(world, person, goal);
      let goals = finished ? rest : [goal, ...rest];
      let updates = goals == [] ? switch (Goals.randomGoal(world, person)) {
        | None => updates
        | Some(goal) => updates @ [(Some(person.name ++ " decided to " ++ goal.name), Types.Updates.addGoal(person.id, goal))]
      } : updates;
      ({...person, goals}, updates)
    }
  }
}