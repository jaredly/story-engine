let size = 1450.0;

let p = Poisson.make(~shape=[|size, size|], ~minDist=200.0, ~maxDist=1150.0, ~maxTries=30, ~rng=Js.Math.random);
let values = Poisson.fill(p);

let d = Poisson.delaunay(values);
let triangles = d##triangles;

let canvas = Canvas.createOnBody(size |> int_of_float, size |> int_of_float);
let ctx = canvas->Canvas.getContext;

ctx->Canvas.Ctx.setFillStyle("black");
ctx->Canvas.Ctx.fillRect(0.0, 0.0, size, size);

ctx->Canvas.Ctx.setStrokeStyle("red");
ctx->Canvas.Ctx.setLineWidth(4.0);
for (i in 0 to Array.length(triangles)/3 - 1) {
    ctx->Canvas.Ctx.beginPath;
    ctx->Canvas.Ctx.line(
        values[triangles[i*3]],
        values[triangles[i*3 + 1]]
    );
    ctx->Canvas.Ctx.line(
        values[triangles[i*3 + 2]],
        values[triangles[i*3 + 1]]
    );
    ctx->Canvas.Ctx.line(
        values[triangles[i*3 + 2]],
        values[triangles[i*3]]
    );
    ctx->Canvas.Ctx.stroke;
}

ctx->Canvas.Ctx.setStrokeStyle("white");

let drawPoly = poly => {
    ctx->Canvas.Ctx.beginPath;
    ctx->Canvas.Ctx.moveToPos(poly[0]);
    poly->Belt.Array.forEach(((x, y)) => {
        ctx->Canvas.Ctx.lineTo(x, y)
    });
    ctx->Canvas.Ctx.stroke;
};

let voronoi = d->Poisson.voronoi((10.0,10.0,size-.10.,size-.10.));

let isClipped = ((x, y)) =>
        x == 10.0 || x == size -. 10.0 ||
        y == 10.0 || y == size -. 10.0;

ctx->Canvas.Ctx.setFillStyle("white");
values->Belt.Array.forEachWithIndex((index, point) => {
    let poly = voronoi->Poisson.cellPolygon(index);
    let touchesClip = poly->Belt.Array.some(isClipped);
    if (!touchesClip) {
        drawPoly(poly);
        ctx->Canvas.Ctx.beginPath;
        // let (x, y) = point;
        // ctx->Canvas.Ctx.circle(x, y, 5.0);
        // ctx->Canvas.Ctx.fill;
    }
});

// let polys = voronoi->Poisson.cellPolygons;
// polys->Belt.Array.forEach(poly => {
//     ctx->Canvas.Ctx.beginPath;
//     ctx->Canvas.Ctx.moveToPos(poly[0]);
//     poly->Belt.Array.forEach(((x, y)) => {
//         ctx->Canvas.Ctx.lineTo(x, y)
//     });
//     ctx->Canvas.Ctx.stroke;
// })

// ctx->Canvas.Ctx.setFillStyle("white");
// values->Belt.Array.forEach(((x, y)) => {
//     ctx->Canvas.Ctx.beginPath;
//     ctx->Canvas.Ctx.circle(x, y, 5.0);
//     ctx->Canvas.Ctx.fill;
// });


print_endline("hi")
// Js.log(values)

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