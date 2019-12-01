
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
        {str(narrative.title)}
      </h3>
      <section>
        {narrative.body->Belt.List.toArray
        ->Belt.Array.map(p => (
          <p>
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
  let make = (~world, ~person: Types.person) => {
    <Expander
      openInitially=true
      header={str(person.demographics.name)}
      renderBody={() => {
        <div>
          <Narrative narrative={Story.narrate(person, world)} />
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
  let make = (~world: Types.world, ~building: Types.Map.building) => {
    <Expander
      header={str(switch building.kind {
        | Exhibit(_, name, terrain) => name ++ ": " ++ terrain
        | FoodStand(_, name, _) => "Food stand " ++ name
      })}
      renderBody={() => {
        <div className=Css.(style([padding(px(16))]))>
          {switch building.kind {
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
            let experiences = world.people->Belt.Map.Int.reduce([], (col, _, person) => {
              person.pastGoals->Belt.List.keepMap(pastGoal => switch (pastGoal.goal) {
                | GoToExhibit({result: Some(result)}) => Some(result)
                | _ => None
              })
            });
            let good = experiences->Belt.List.keep(ex => switch ex { | Satisfied => true | _ => false });
            str("Experiences: " ++ string_of_int(List.length(good)) ++ "/" ++ string_of_int(List.length(experiences)))
          }
        </div>
      }}
    />
  }
}

[@react.component]
let make = (~world: React.Ref.t(Types.world)) => {
  // Js.log(world->React.Ref.current.people);
  // Js.log(world->React.Ref.current.map);
  let (tick, setTick) = Hooks.useState(0);
  let tick = () => setTick(tick + 1);
  React.useEffect(() => {
    let tid = Js.Global.setInterval(() => {
      tick()
    }, 500);
    Some(() => Js.Global.clearInterval(tid))
  });
  let world = world->React.Ref.current;
  <div>
    // <button onClick={_evt => tick()}> {React.string("Check")} </button>
    <div>
      {world.map.buildings
       ->Belt.Map.Int.valuesToArray
       ->Belt.Array.map(building =>
           <Building world building key={string_of_int(building.id)}/>
            //  {switch (building.kind) {
            //   | Exhibit(animals, name, terrain) => React.string(name)
            //   | _ => React.string("unknwone building")
            //   }}
          //  </div>
         )
       ->React.array}
    </div>
    <div>
      {world.people
       ->Belt.Map.Int.valuesToArray
       ->Belt.Array.map(person =>
           <Person world person key={string_of_int(person.id)}/>
              // {React.string(person.demographics.name)} </div>
             // {React.string(string_of_int(person.id))}
         )
       ->React.array}
    </div>
  </div>;
};