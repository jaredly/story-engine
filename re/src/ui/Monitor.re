
let str = React.string;

module Expander = {
  [@react.component]
  let make = (~openInitially=false, ~header, ~renderBody) => {
    let (isOpen, setOpen) = Hooks.useState(openInitially);
    if (isOpen) {
      <div>
        <div onClick={_ => setOpen(false)} className=Css.(style([cursor(`pointer), padding(px(8))]))>
          {header}
        </div>
        {renderBody()}
      </div>
    } else {
      <div onClick={_ => setOpen(true)} className=Css.(style([cursor(`pointer), padding(px(8))]))>
        {header}
      </div>
    }
  }
}

module Narrative = {
  [@react.component]
  let make = (~narrative: Story.narrative) => {
    <div>
      <h3>
        {str(narrative.title ++ " ")}
        {str(Story.stars(narrative.stars) ++ " stars")}
      </h3>
      <section>
        {narrative.body->Belt.List.toArray
        ->Belt.Array.mapWithIndex((i, p) => (
          <p key={string_of_int(i)}>
            {str(String.concat(" ", p))}
          </p>
        ))
        ->React.array}
      </section>
    </div>
  }
}

module Person = {
  [@react.component]
  let make = (~showing=false, ~world, ~person: Types.person) => {
    <Expander
      openInitially=showing
      header={str(person.demographics.name)}
      renderBody={() => {
        <div>
          <Narrative narrative={Story.narrate(person, React.Ref.current(world))} />
          // {person.experiences
          //  ->Belt.List.toArray
          //  ->Belt.Array.mapWithIndex((i, {time, goalTrail, update}) => {
          //     <div key={string_of_int(i)}>
          //     //  {str(trail |> String.concat(" <- "))}
          //      {str(Types.showPersonUpdate(update))}
          //     </div>
          //    })
          //  ->React.array}
        </div>
      }}
    />;
  };
};

module Building = {
  [@react.component]
  let make = (~world: React.Ref.t(Types.world), ~building: Types.Map.building) => {
    <Expander
      header={str(switch building.kind {
        | Exhibit(_, name, terrain) => name ++ ": " ++ terrain
        | FoodStand(_, name, _) => "Food stand " ++ name
      })}
      renderBody={() => {
        <div className=Css.(style([padding(px(16))]))>
          {
            let world = React.Ref.current(world);
            switch building.kind {
            | FoodStand(_) => React.null
            | Exhibit(animals, name, terrain) =>
              React.array(
                animals->Belt.Set.Int.toArray
                ->Belt.Array.keepMap(world.animals->Belt.Map.Int.get)
                ->Belt.Array.map(animal => (
                  <div key={string_of_int(animal.id)}>
                    {str(animal.kind ++ " is ")}
                    {str(Types.showBehavior(animal.behavior))}
                  </div>
                ))
              )
          }}
          {
            let world = React.Ref.current(world);
            let experiences = world.peopleWhoLeft->Belt.List.reduce([], (col, person) => {
              col @ person.pastGoals->Belt.List.keepMap(pastGoal => switch (pastGoal.goal) {
                | GoToExhibit({attrs, result: Some(result)}) when attrs == building.id => Some(result)
                | _ => None
              })
            });
            let experiences = world.people->Belt.Map.Int.reduce(experiences, (col, _, person) => {
              col @ person.pastGoals->Belt.List.keepMap(pastGoal => switch (pastGoal.goal) {
                | GoToExhibit({attrs, result: Some(result)}) when attrs == building.id => Some(result)
                | _ => None
              })
            });
            let avg = experiences->Belt.List.reduce(0., (+.)) /. float_of_int(List.length(experiences));
            // Js.log2()
            str("Experiences: " ++ string_of_int(List.length(experiences)) ++ " - avg stars " ++ Js.Float.toFixedWithPrecision(avg, ~digits=2))
            // let good = experiences->Belt.List.keep(ex => ex > 0.2);
            // str("Experiences: " ++ string_of_int(List.length(good)) ++ "/" ++ string_of_int(List.length(experiences)))
          }
        </div>
      }}
    />
  }
};

[@react.component]
let make = (~world: React.Ref.t(Types.world)) => {
  let (tick, setTick) = Hooks.useState(0);
  let tick = () => setTick(tick + 1);
  React.useEffect(() => {
    let tid = Js.Global.setInterval(() => {tick()}, 500);
    Some(() => Js.Global.clearInterval(tid));
  });
  <div>
    {React.string(Story.clockTime(world->React.Ref.current.clock))}
    {React.string(Story.clockTime(Types.closingTime))}
    <div>
      {React.Ref.current(world).map.buildings
       ->Belt.Map.Int.valuesToArray
       ->Belt.Array.map(building =>
           <Building world building key={string_of_int(building.id)} />
         )
       ->React.array}
    </div>
    <div>
      {React.Ref.current(world).people
       ->Belt.Map.Int.valuesToArray
       ->Belt.Array.map(person =>
           <Person world person key={string_of_int(person.id)} />
         )
       ->React.array}
      {React.Ref.current(world).peopleWhoLeft
       ->Belt.List.toArray
       ->Belt.Array.map(person =>
           <Person showing=true world person key={string_of_int(person.id)} />
         )
       ->React.array}
    </div>
  </div>;
};