[%%debugger.chrome];

// let world = Basic.gen();
let size = 1000.0;
let rng = Prando.make(100);
// let world = RandomMap.gen(size, 180.0, 380.0, rng);
let world = RandomMap.gen(size, 110.0, size, rng);

let canvas = Canvas.createOnBody(size |> int_of_float, size |> int_of_float);
let ctx = canvas->Canvas.getContext;

let slider = Web.createElement(Web.document, "input");
slider->Web.setAttribute("type", "range");
slider->Web.setAttribute("min", "0");
slider->Web.setAttribute("value", "0");
slider->Web.setAttribute("max", "100");
slider->Web.addEventListener("input", evt => {
    Js.log2("slide", slider->Web.value)
}, false);
Web.documentEl->Web.body->Web.appendChild(slider);

let button = Web.createElement(Web.document, "button");
button->Web.textContent("Play");
Web.documentEl->Web.body->Web.appendChild(button);

ctx->Canvas.Ctx.translate(30.0, 30.0);
// ctx->Canvas.Ctx.scale(30.0, 30.0)

ctx->Canvas.Ctx.clearRect(0.0, 0.0, size, size);
ctx->Draw.map(world.map)

world->World.addPerson;

let ival = ref(None);

let pause = () => {
    switch (ival^) {
        | None => ()
        | Some(id) => Js.Global.clearInterval(id)
    }
    ival := None
};

let play = () => {
    ival := Some(Js.Global.setInterval(() => {
        world->World.step
        // world->World.step
        // world->World.step
        // world->World.step
        // world->World.step
        ctx->Canvas.Ctx.clearRect(0.0, 0.0, size, size);
        ctx->Draw.world(world)
    }, 50))
};

let toggle = () => {
    let title = switch (ival^) {
        | None => play(); "Pause"
        | Some(_) => pause(); "Play"
    };
    button->Web.textContent(title)
}


button->Web.addEventListener("click", evt => toggle(), false);