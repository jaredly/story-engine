let useState = initial =>
  React.useReducer((_, newState) => newState, initial);

let makeWorld = (size, minDist, seed) => {
  let rng = Prando.make(seed);
  // let world = RandomMap.gen(size, 180.0, 380.0, rng);
  let world = RandomMap.gen(size, minDist, size, rng);
  world;
};

[@react.component]
let make = () => {
  let (size, setSize) = useState(500.0);
  let (minDist, setMinDist) = useState(75.0);
  let (seed, setSeed) = useState(100);

  let canvasRef = React.useRef(Js.Nullable.null);

  let initialWorld = React.useMemo(() => makeWorld(size, minDist, seed));
  let world = React.useRef(initialWorld);

  let draw = () => {
    switch (canvasRef->React.Ref.current->Js.Nullable.toOption) {
    | None => ()
    | Some(canvas) =>
      let ctx = Canvas.getContext(Obj.magic(canvas));
      ctx->Canvas.Ctx.clearRect(0.0, 0.0, size, size);
      Draw.world(ctx, world->React.Ref.current);
    };
  };

  // React.useMemo3(() => {
  //   world->React.Ref.setCurrent(makeWorld(size, minDist, seed));
  //   draw();
  //   ()
  // }, (size, minDist, seed))

  React.useEffect3(
    () => {
      world->React.Ref.setCurrent(makeWorld(size, minDist, seed));
      draw();
      None;
    },
    (size, minDist, seed),
  );

  React.useEffect(() => {
    let id =
      Js.Global.setInterval(
        () => {
          world->React.Ref.current->World.step;
          draw();
        },
        50,
      );
    Some(() => Js.Global.clearInterval(id));
  });

  <div>
    <div>
      {React.string("Seed")}
      <input
        type_="range"
        min=0
        max="100"
        value=string_of_int(seed)
        onChange={evt => setSeed(int_of_string(evt->ReactEvent.Form.target##value))}
      />
      {React.string(string_of_int(seed))}
    </div>
    <div>
      {React.string("Size")}
      <input
        type_="range"
        min=200
        max="1000"
        value=Js.Float.toString(size)
        onChange={evt => setSize(Js.Float.fromString(evt->ReactEvent.Form.target##value))}
      />
      {React.string(Js.Float.toString(size))}
    </div>
    <div>
      {React.string("MinDist")}
      <input
        type_="range"
        min=30
        max="300"
        value=Js.Float.toString(minDist)
        onChange={evt => setMinDist(Js.Float.fromString(evt->ReactEvent.Form.target##value))}
      />
      {React.string(Js.Float.toString(minDist))}
    </div>
    <canvas
      width={Js.Float.toString(size)}
      height={Js.Float.toString(size)}
      ref={canvasRef->ReactDOMRe.Ref.domRef}
    />
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
  </div>;
};