

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
        let (x, y) = point;
        ctx->Canvas.Ctx.circle(x, y, 5.0);
        ctx->Canvas.Ctx.fill;
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
Js.log(values)
