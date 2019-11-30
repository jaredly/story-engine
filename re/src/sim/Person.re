let stepCondition = (condition: Types.condition, rng) => {
  ...condition,
  thirst: min(1.0, condition.thirst +. rng->Prando.range(0.01, 0.05)),
  hunger: min(1.0, condition.hunger +. rng->Prando.range(0.01, 0.05)),
  stamina: max(0.0, condition.stamina -. rng->Prando.range(0.0001, 0.0005)),
};

let step = (world, person: Types.person) => {
  switch (person.goals) {
  | [] => (person, [])
  | [goal, ...rest] =>
    let (goal, updates, finished) = Goals.step(world, person, goal);
    let (goals, pastGoals) =
      switch (finished) {
      | None => ([goal, ...rest], [])
      | Some(failed) => (
          rest,
          [{Types.goal, failed, timeStopped: world.clock}],
        )
      };
    let updates =
      goals == []
        ? switch (Goals.randomGoal(world, person)) {
          | None => updates
          | Some(goal) =>
            updates
            @ [
              {
                trail: [],
                update:
                  Message(
                    person.demographics.name ++ " decided to " ++ goal.name,
                  ),
              },
              {trail: [], update: Types.Updates.addGoal(person.id, goal)},
            ]
          }
        : updates;
    (
      {
        ...person,
        goals,
        condition: stepCondition(person.condition, world.rng),
        pastGoals: pastGoals @ person.pastGoals,
      },
      updates,
    );
  };
};