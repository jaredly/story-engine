let line_size = 30.0;

let map = (ctx: Canvas.ctx, map: Types.Map.map) => {
    open Canvas.Ctx;
    ctx->setStrokeStyle("#ccc");
    ctx->setLineWidth(line_size);
    map.edges->Belt.Map.Int.forEach((_key, v) => {
        let p1 = map.points->Belt.Map.Int.getExn(v.source).pos;
        let p2 = map.points->Belt.Map.Int.getExn(v.dest).pos;
        ctx->beginPath;
        ctx->line(p1, p2);
        ctx->stroke;
    });

    ctx->setFillStyle("#ccc");
    map.points->Belt.Map.Int.forEach((_key, v) => {
        // let p1 = map.points->Belt.Map.Int.getExn(v.source).pos;
        // let p2 = map.points->Belt.Map.Int.getExn(v.dest).pos;
        let (x, y) = v.pos;
        ctx->beginPath;
        ctx->circle(x, y, line_size /. 2.0);
        ctx->fill;
    });

    ctx->setFillStyle("#7fa");
    map.exits->Belt.List.forEach((b) => {
        let (x, y) = map.points->Belt.Map.Int.getExn(b).pos;
        ctx->beginPath;
        ctx->circle(x, y, line_size /. 2.0 *. 1.5);
        ctx->fill;
    });

    ctx->setFillStyle("red");
    map.buildings->Belt.Map.Int.forEach((_, b) => {
        let (x, y) = map.points->Belt.Map.Int.getExn(b.point).pos;
        ctx->beginPath;
        // ctx->circle(x +. 2.0, y +. 2.0, 2.0);
        ctx->circle(x, y, line_size /. 2.0 *. 1.5);
        ctx->fill;
    })
}

let lerp = (a, b, d) => (b -. a) *. d +. a;

let lerp2d = ((x1, y1), (x2, y2), amount) => {
    (lerp(x1, x2, amount), lerp(y1, y2, amount))
}

let angleTo = ((x1, y1), (x2, y2)) => atan2(y2 -. y1, x2 -. x1);

let pi = Js.Math._PI;

let world = (ctx, world: Types.world) => {
    ctx->map(world.map);
    open Belt.Map.Int;
    open Canvas.Ctx;
    ctx->setFillStyle("#777");
    world.people->Belt.Map.Int.forEach((_, person) => {
        let edge = world.map.edges->getExn(person.position.edge);
        let p1 = world.map.points->getExn(edge.source);
        let p2 = world.map.points->getExn(edge.dest);
        let angle = angleTo(p1.pos, p2.pos);
        let offset = line_size /. 2.0;
        let ox = cos(angle +. pi /. 2.) *. person.offset *. offset;
        let oy = sin(angle +. pi /. 2.) *. person.offset *. offset;
        let (x, y) = lerp2d(p1.pos, p2.pos, person.position.progress)
        ctx->beginPath;
        ctx->circle(x +. ox, y +. oy, line_size /. 4.0);
        ctx->fill;
    })
}
