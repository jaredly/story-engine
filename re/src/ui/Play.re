module Config = {
  type t = {
    seed: int,
    size: float,
    minDist: float,
    maxPeoplePerExhibit: int,
    // paused: option(int),
  };

  let default = {
    seed: 100,
    size: 500.0,
    minDist: 75.0,
    maxPeoplePerExhibit: 20,
    // paused: None,
  };

  let parse = hash => {
    switch (hash->Js.String2.split(":")->Array.to_list) {
    | [seed, size, minDist, maxPeople, ...rest] =>
      switch (
        int_of_string(seed),
        float_of_string(size),
        float_of_string(minDist),
        int_of_string(maxPeople),
      ) {
      | exception _ => None
      | (seed, size, minDist, maxPeoplePerExhibit) =>
        Some(({seed, size, minDist, maxPeoplePerExhibit,
        }, 
          switch rest {
            | [one] => Some(int_of_string(one))
            | _ => None
          }
        ))
      }
    | _ => None
    };
  };

  let stringify = ({seed, size, minDist, maxPeoplePerExhibit}, paused) =>
    ([
      string_of_int(seed),
      Js.Float.toString(size),
      Js.Float.toString(minDist),
      string_of_int(maxPeoplePerExhibit),
    ] @ switch paused { | None => [] | Some(x) => [string_of_int(x)]})
    |> String.concat(":");
};

let makeWorld = ({Config.size, minDist, seed, maxPeoplePerExhibit}, paused) => {
  let rng = Prando.make(seed);
  // let world = RandomMap.gen(size, 180.0, 380.0, rng);
  let world = {
    ...RandomMap.gen(size, minDist, size, rng),
    maxPeoplePerExhibit,
  };
  world->World.addPerson;
  switch (paused) {
    | None => ()
    | Some(x) => for (_ in 0 to x) {
      world->World.step
    }
  }
  world;
};

[@react.component]
let make = () => {
  let (initialConfig, initialPaused) =
    React.useMemo(() => {
      switch (
        Config.parse(Web.Location.hash()->Js.String2.sliceToEnd(~from=1))
      ) {
      | None => (Config.default, None)
      | Some(x) => x
      }
    });
  let (paused, setPaused) = Hooks.useState(initialPaused)
  let (config, setConfig) = Hooks.useState(initialConfig);
  // let (size, setSize) = Hooks.useState(500.0);
  // let (minDist, setMinDist) = Hooks.useState(75.0);
  // let (seed, setSeed) = Hooks.useState(100);

  let setPaused = paused => {
      Web.Location.setHash(Config.stringify(config, paused));
      setPaused(paused);
  };

  let (speed, setSpeed) = Hooks.useState(20);

  let canvasRef = React.useRef(Js.Nullable.null);

  let initialWorld = React.useMemo(() => makeWorld(config, paused));
  let world = React.useRef(initialWorld);

  let draw = () => {
    switch (canvasRef->React.Ref.current->Js.Nullable.toOption) {
    | None => ()
    | Some(canvas) =>
      let ctx = Canvas.getContext(Obj.magic(canvas));
      ctx->Canvas.Ctx.clearRect(0.0, 0.0, config.size, config.size);
      Draw.world(ctx, world->React.Ref.current);
    };
  };

  let skip = count => {
    for (_ in 0 to count) {
      world->React.Ref.current->World.step;
    };
    if (paused != None) {
      setPaused(Some(world->React.Ref.current.clock))
    };
    draw();
  };

  React.useEffect1(
    () => {
      world->React.Ref.setCurrent(makeWorld(config, paused));
      draw();
      Web.Location.setHash(Config.stringify(config, paused));
      None;
    },
    [|config|],
  );

  React.useEffect2(
    () =>
      if (paused == None) {
        let id =
          Js.Global.setInterval(
            () => {
              world->React.Ref.current->World.step;
              draw();
            },
            1000 / speed,
          );
        Some(() => Js.Global.clearInterval(id));
      } else {
        None;
      },
    (speed, paused),
  );

  <div>
    <div>
      {React.string("Seed")}
      <input
        type_="range"
        min=0
        max="100"
        value={string_of_int(config.seed)}
        onChange={evt =>
          setConfig({
            ...config,
            seed: int_of_string(evt->ReactEvent.Form.target##value),
          })
        }
      />
      {React.string(string_of_int(config.seed))}
      <button onClick={_ => {
        setPaused(paused == None ? Some(world->React.Ref.current.clock) : None)
      }}>
        {React.string(paused == None ? "Pause" : "Play")}
      </button>
    </div>
    <div>
      {React.string("Size")}
      <input
        type_="range"
        min=200
        max="1000"
        value={Js.Float.toString(config.size)}
        onChange={evt =>
          setConfig({
            ...config,
            size: Js.Float.fromString(evt->ReactEvent.Form.target##value),
          })
        }
      />
      {React.string(Js.Float.toString(config.size))}
    </div>
    <div>
      {React.string("Max people per exhibit")}
      <input
        type_="range"
        min=1
        max="40"
        value={string_of_int(config.maxPeoplePerExhibit)}
        onChange={evt =>
          setConfig({
            ...config,
            maxPeoplePerExhibit:
              int_of_string(evt->ReactEvent.Form.target##value),
          })
        }
      />
      {React.string(string_of_int(config.maxPeoplePerExhibit))}
    </div>
    <div>
      {React.string("MinDist")}
      <input
        type_="range"
        min=30
        max="300"
        value={Js.Float.toString(config.minDist)}
        onChange={evt =>
          setConfig({
            ...config,
            minDist: Js.Float.fromString(evt->ReactEvent.Form.target##value),
          })
        }
      />
      {React.string(Js.Float.toString(config.minDist))}
    </div>
    <div>
      {React.string("Skip steps")}
      <button onClick={_ => skip(100)}> {React.string("100")} </button>
      <button onClick={_ => skip(500)}> {React.string("500")} </button>
      <button onClick={_ => skip(1000)}> {React.string("1000")} </button>
    </div>
    <div>
      {React.string("Speed")}
      <input
        type_="range"
        min=1
        max="50"
        value={string_of_int(speed)}
        onChange={evt =>
          setSpeed(int_of_string(evt->ReactEvent.Form.target##value))
        }
      />
      {React.string(string_of_int(speed))}
    </div>
    <div
      className=Css.(
        style([
          display(`flex),
          flexDirection(`row),
          alignItems(`flexStart),
        ])
      )>
      <canvas
        width={Js.Float.toString(config.size)}
        height={Js.Float.toString(config.size)}
        ref={canvasRef->ReactDOMRe.Ref.domRef}
      />
      <Monitor world />
    </div>
  </div>;
};