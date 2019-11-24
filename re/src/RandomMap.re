
[@bs.module]
[@bs.val]
external animals: array({.
  "name": string,
  "href": string
}) = "../animals.json";

let shuffle = (points, rng) => {
  let ln = Array.length(points);
  let rec loop = (index) => {
    if (index < ln - 1) {
      let idx = rng->Prando.int(index, ln - 1);
      let removed = points->Js.Array2.spliceInPlace(~pos=idx, ~remove=1, ~add=[||]);
      points->Js.Array2.unshift(removed[0])->ignore;
      loop(index + 1)
    }
  }
  loop(0)
}

let gen = (size, minDist, maxDist, rng) => {
  let p =
    Poisson.make(
      ~shape=[|size, size|],
      ~minDist,
      ~maxDist,
      ~maxTries=30,
      ~rng=() => rng->Prando.float,
    );
  let values = Poisson.fill(p);

  let d = Poisson.delaunay(values);
  let triangles = d##triangles;

  let low = 0.0;
  let high = size;

  let voronoi = d->Poisson.voronoi((low, low, high, high));

  let isClipped = ((x, y)) =>
    x == low || x == high || y == low || y == high;

  let isInternal = index => {
    let poly = voronoi->Poisson.cellPolygon(index);
    let touchesClip = poly->Belt.Array.some(isClipped);
    !touchesClip
  }

  let pointIndices = values->Belt.Array.mapWithIndex((index, _point) => index);

  let edgePoints = pointIndices
  ->Belt.Array.keep(index => {
    !isInternal(index)
  });
  edgePoints->shuffle(rng);

  let num_exits = max(1, edgePoints->Array.length / 10);
  let exits = edgePoints->Js.Array2.slice(~start=0, ~end_=num_exits);
  let isExit = idx => exits->Js.Array2.includes(idx);

  let isGood = index => {
    isExit(index) || isInternal(index)
  }

  let edges = Hashtbl.create(100);
  let addEdge = (a, b) => {
    if (isGood(a) && isGood(b)) {
      Hashtbl.replace(edges, (min(a, b), max(a, b)), ());
    }
  };

  for (i in 0 to Array.length(triangles) / 3 - 1) {
    addEdge(triangles[i * 3], triangles[i * 3 + 1]);
    addEdge(triangles[i * 3 + 2], triangles[i * 3 + 1]);
    addEdge(triangles[i * 3], triangles[i * 3 + 2]);
  }

  let genId = World.idGenerator();

  let internalPoints = pointIndices->Belt.Array.keep(isInternal);
  internalPoints->shuffle(rng);
  let buildingPoints = internalPoints->Js.Array2.slice(~start=0, ~end_=Array.length(internalPoints) * 2 / 3);
  let buildings = buildingPoints
  ->Belt.Array.reduce(Belt.Map.Int.empty, (map, idx) => {
    let id = genId();
    let aidx = rng->Prando.int(0, Array.length(animals));
    let animal = animals->Js.Array2.spliceInPlace(~pos=aidx, ~remove=1, ~add=[||])[0];
    map->Belt.Map.Int.set(id, {
      Types.Map.point: idx,
      id,
      kind:
        Exhibit(
          Belt.Set.Int.empty,
          animal##name ++ " exhibit",
          "grass and tall trees",
        ),
    })
  })


  let points = values
  ->Belt.Array.mapWithIndex((index, point) => (index, point))
  ->Belt.Array.keepMap(((index, point)) => isGood(index) ? Some({
    Types.Map.building: None,
    id: index,
    pos: point,
    location: None
  }) : None);
  let points = Belt.Map.Int.fromArray(points->Belt.Array.map(item => (item.id, item)));

  // Js.log3("Stuff", buildings->Belt.Map.Int.valuesToArray,
  // points->Belt.Map.Int.valuesToArray);

  let points = buildings->Belt.Map.Int.reduce(points, (points, _, building) => {
    open Belt.Map.Int;
    // Js.log3("Building point", building.id, building.point);
    points->set(building.point, {
      ...points->getExn(building.point),
      building: Some(building.id)
    })
  });

  let l1 = genId();

  let edges = Hashtbl.fold((k, (), keys) => [k, ...keys], edges, [])
  ->Belt.List.map(((p1, p2)) => Types.Map.edge(genId(), p1, p2, l1, points))
  ->Belt.List.map(item => (item.id, item))
  ->Belt.List.toArray
  ->Belt.Map.Int.fromArray;

  let pointToEdges = World.makeMapping(edges);

  // let pointIds = 

  open Belt.Map.Int;
  let map: Types.Map.map = {
    points,
    edges,
    pointToEdges,
    pointToPoint: World.pointToPoint(edges),
    buildings,
    locations:
      empty
      ->set(l1, {Types.Map.id: l1, name: "the entrance"}),
    // exits: [points->Belt.Map.Int.keysToArray[0]],
    exits: Array.to_list(exits),
  };

  // map;
  let world: Types.world = {
    rng,
    genId,
    people: empty,
    animals: empty,
    items: empty,
    map,
  };

  world

  // let pointToPoint = Hashtbl.fold((key, (), edges))

  // ctx->Canvas.Ctx.setFillStyle("white");
  // values->Belt.Array.forEachWithIndex((index, point) => {
  //   let poly = voronoi->Poisson.cellPolygon(index);
  //   let touchesClip = poly->Belt.Array.some(isClipped);
  //   if (!touchesClip) {
  //     drawPoly(poly);
  //     ctx->Canvas.Ctx.beginPath;
  //     // let (x, y) = point;
  //     // ctx->Canvas.Ctx.circle(x, y, 5.0);
  //     // ctx->Canvas.Ctx.fill;
  //   };
  // });
};