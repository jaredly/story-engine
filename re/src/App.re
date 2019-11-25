[%%debugger.chrome];

let maunal = () => {

let size = 1000.0 /. 2.;
let minDist = 150.0 /. 2.;

let canvas = Canvas.createOnBody(size |> int_of_float, size |> int_of_float);
let ctx = canvas->Canvas.getContext;

ctx->Canvas.Ctx.scale(2.0, 2.0);

let slider = Web.createElement(Web.document, "input");
slider->Web.setAttribute("type", "range");
slider->Web.setAttribute("min", "0");
slider->Web.setAttribute("value", "0");
slider->Web.setAttribute("max", "100");
Web.documentEl->Web.body->Web.appendChild(slider);

let sliderText = Web.document->Web.createElement("span");
Web.documentEl->Web.body->Web.appendChild(sliderText);

let button = Web.createElement(Web.document, "button");
button->Web.textContent("Play");
Web.documentEl->Web.body->Web.appendChild(button);

// ctx->Canvas.Ctx.translate(30.0, 30.0);
// ctx->Canvas.Ctx.scale(30.0, 30.0)

let makeWorld = (seed) => {
    // let world = Basic.gen();
    let rng = Prando.make(seed);
    // let world = RandomMap.gen(size, 180.0, 380.0, rng);
    let world = RandomMap.gen(size, minDist, size, rng);
    world
}

let draw = (world: Types.world) => {
    ctx->Canvas.Ctx.clearRect(0.0, 0.0, size, size);
    ctx->Draw.map(world.map)
}

let ival = ref(None);

let pause = () => {
    switch (ival^) {
        | None => ()
        | Some(id) => Js.Global.clearInterval(id)
    }
    ival := None
};

let world = ref(makeWorld(100))
draw(world^);
world^ ->World.addPerson;

let play = () => {
    ival := Some(Js.Global.setInterval(() => {
        world^ ->World.step
        // world->World.step
        // world->World.step
        // world->World.step
        // world->World.step
        ctx->Canvas.Ctx.clearRect(0.0, 0.0, size, size);
        ctx->Draw.world(world^)
    }, 50))
};

let toggle = () => {
    let title = switch (ival^) {
        | None => play(); "Pause"
        | Some(_) => pause(); "Play"
    };
    button->Web.textContent(title)
}

slider->Web.addEventListener("input", evt => {
    let seed = slider->Web.value;
    sliderText->Web.textContent(string_of_int(seed));
    world := makeWorld(seed);
    draw(world^)
    world^ ->World.addPerson;
}, false);

button->Web.addEventListener("click", evt => toggle(), false);
};

ReactDOMRe.renderToElementWithId(<Play />, "root")