[@react.component]
let make = (~world: React.Ref.t(Types.world)) => {
  Js.log(world->React.Ref.current.people);
  Js.log(world->React.Ref.current.map);
  let (tick, setTick) = Hooks.useState(0);
  let tick = () => setTick(tick + 1);
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
           <div key={string_of_int(person.id)}>
              {React.string(person.demographics.name)} </div>
             // {React.string(string_of_int(person.id))}
         )
       ->React.array}
    </div>
  </div>;
};