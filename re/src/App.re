[%%debugger.chrome];

// let world = Basic.gen();
let size = 1450.0;
let rng = Prando.make(100);
let world = RandomMap.gen(size, 180.0, 380.0, rng);

let canvas = Canvas.createOnBody(size |> int_of_float, size |> int_of_float);
let ctx = canvas->Canvas.getContext;

// ctx->Canvas.Ctx.translate(100.0, 100.0)
// ctx->Canvas.Ctx.scale(30.0, 30.0)

ctx->Canvas.Ctx.clearRect(0.0, 0.0, size, size);
ctx->Draw.map(world.map)

world->World.addPerson;

Js.Global.setInterval(() => {
    world->World.step
    world->World.step
    world->World.step
    world->World.step
    world->World.step
    ctx->Canvas.Ctx.clearRect(0.0, 0.0, size, size);
    ctx->Draw.world(world)
}, 10)