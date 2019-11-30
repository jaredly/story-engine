
let str = React.string;

module Expander = {
  [@react.component]
  let make = (~header, ~renderBody) => {
    let (isOpen, setOpen) = Hooks.useState(false);
    if (isOpen) {
      <div>
        <div onClick={_ => setOpen(false)} className=Css.(style([cursor(`pointer)]))>
          {header}
        </div>
        {renderBody()}
      </div>

    } else {
      <div onClick={_ => setOpen(true)} className=Css.(style([cursor(`pointer)]))>
        {header}
      </div>
    }
  }
}

module Person = {
  [@react.component]
  let make = (~person: Types.person) => {
    <Expander
      header={<div className=Css.(style([padding(px(8))]))>
      {str(person.demographics.name)}
      </div>}
      renderBody={() => {
        <div>
          {person.experiences
           ->Belt.List.toArray
           ->Belt.Array.map(((time, trail, personUpdate)) => {
              <div>
               {str(trail |> String.concat(" <- "))}
               {str(Types.showPersonUpdate(personUpdate))}
              </div>
             })
           ->React.array}
        </div>
      }}
    />;
  };
};

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
  <div>
    <button onClick={_evt => tick()}> {React.string("Check")} </button>
    <div>
      {world->React.Ref.current.Types.map.Types.Map.buildings
       ->Belt.Map.Int.valuesToArray
       ->Belt.Array.map(building =>
           <div key={string_of_int(building.id)}>
             {switch (building.kind) {
              | Exhibit(animals, name, terrain) => React.string(name)
              | _ => React.string("unknwone building")
              }}
           </div>
         )
       ->React.array}
    </div>
    <div>
      {world->React.Ref.current.Types.people
       ->Belt.Map.Int.valuesToArray
       ->Belt.Array.map(person =>
           <Person person key={string_of_int(person.id)}/>
              // {React.string(person.demographics.name)} </div>
             // {React.string(string_of_int(person.id))}
         )
       ->React.array}
    </div>
  </div>;
};