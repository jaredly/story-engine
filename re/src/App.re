[%%debugger.chrome];

let world = Basic.gen();

let size = 1450.0;
let canvas = Canvas.createOnBody(size |> int_of_float, size |> int_of_float);
let ctx = canvas->Canvas.getContext;

ctx->Canvas.Ctx.translate(100.0, 100.0)
ctx->Canvas.Ctx.scale(30.0, 30.0)

ctx->Canvas.Ctx.clearRect(0.0, 0.0, 50.0, 50.0);
ctx->Draw.map(world.map)

world->World.addPerson;

Js.Global.setInterval(() => {
    world->World.step
    ctx->Canvas.Ctx.clearRect(0.0, 0.0, 50.0, 50.0);
    ctx->Draw.world(world)
}, 50)