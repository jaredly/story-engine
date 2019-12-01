[@bs.module] [@bs.val]
external animals:
  array({
    .
    "name": string,
    "href": string,
  }) =
  "../../assets/animals.json";

let shuffle = (points, rng) => {
  let ln = Array.length(points);
  let rec loop = index =>
    if (index < ln - 1) {
      let idx = rng->Prando.int(index, ln - 1);
      let removed =
        points->Js.Array2.spliceInPlace(~pos=idx, ~remove=1, ~add=[||]);
      points->Js.Array2.unshift(removed[0])->ignore;
      loop(index + 1);
    };
  loop(0);
};

let gen = (size, minDist, maxDist, rng) => {
  let animals = animals->Js.Array2.copy;
  let p =
    Poisson.make(
      ~shape=[|size, size|], ~minDist, ~maxDist, ~maxTries=30, ~rng=() =>
      rng->Prando.float
    );
  let values = Poisson.fill(p);

  let d = Poisson.delaunay(values);
  let triangles = d##triangles;

  let low = 0.0;
  let high = size;

  let voronoi = d->Poisson.voronoi((low, low, high, high));

  let isClipped = ((x, y)) => x == low || x == high || y == low || y == high;

  let isInternal = index => {
    let poly = voronoi->Poisson.cellPolygon(index);
    let touchesClip = poly->Belt.Array.some(isClipped);
    !touchesClip;
  };

  let pointIndices =
    values->Belt.Array.mapWithIndex((index, _point) => index);

  let internalPoints = pointIndices->Belt.Array.keep(isInternal);
  // let edgePoints = pointIndices->Belt.Array.keep(index => !isInternal(index));

  let edgeToInternal = Hashtbl.create(100);
  let addEdgeToInternal = (a, b) => {
    let ai = isInternal(a);
    let bi = isInternal(b);
    if (ai != bi) {
      let edge = ai ? b : a;
      Hashtbl.replace(edgeToInternal, edge, ());
    };
  };

  for (i in 0 to Array.length(triangles) / 3 - 1) {
    addEdgeToInternal(triangles[i * 3], triangles[i * 3 + 1]);
    addEdgeToInternal(triangles[i * 3 + 2], triangles[i * 3 + 1]);
    addEdgeToInternal(triangles[i * 3], triangles[i * 3 + 2]);
  };

  let edgePoints =
    Hashtbl.fold((k, _, keys) => [k, ...keys], edgeToInternal, [])
    ->Array.of_list;

  edgePoints->shuffle(rng);

  let num_exits = max(1, edgePoints->Array.length / 10);
  let exits = edgePoints->Js.Array2.slice(~start=0, ~end_=num_exits);
  let isExit = idx => exits->Js.Array2.includes(idx);

  let isGood = index => {
    isExit(index) || isInternal(index);
  };

  let bothGood = (a, b) =>
    if (isExit(a)) {
      isInternal(b);
    } else {
      isInternal(a) && (isExit(b) || isInternal(b));
    };

  let edges = Hashtbl.create(100);
  let addEdge = (a, b) =>
    if (bothGood(a, b)) {
      Hashtbl.replace(edges, (min(a, b), max(a, b)), ());
    };

  for (i in 0 to Array.length(triangles) / 3 - 1) {
    addEdge(triangles[i * 3], triangles[i * 3 + 1]);
    addEdge(triangles[i * 3 + 2], triangles[i * 3 + 1]);
    addEdge(triangles[i * 3], triangles[i * 3 + 2]);
  };

  let genId = World.idGenerator();

  internalPoints->shuffle(rng);
  let buildingPoints =
    internalPoints->Js.Array2.slice(
      ~start=0,
      ~end_=Array.length(internalPoints) * 2 / 3,
    );
  let (animalMap, buildings) =
    buildingPoints->Belt.Array.reduce(
      (Belt.Map.Int.empty, Belt.Map.Int.empty),
      ((animalMap, map), idx) => {
        let id = genId();
        let aidx = rng->Prando.int(0, Array.length(animals) - 1);
        let animal = animals->Js.Array2.spliceInPlace(
                       ~pos=aidx,
                       ~remove=1,
                       ~add=[||],
                     )[0];
        let count = rng->Prando.int(1, 5);
        let (animalIds, animalMap) =
          Prando.rangeList(count)
          ->Belt.List.reduce(
              ([], animalMap),
              ((ids, map), _) => {
                let aid = genId();
                ([aid, ...ids], map->Belt.Map.Int.set(
                  aid,
                  {
                    Types.exhibit: id,
                    id: aid,
                    kind: animal##name,
                    behavior: rng->Prando.choose(Types.behaviors),
                    behaviorTimer: rng->Prando.int(10, 50),
                    visibility: rng->Prando.float,
                  },
                ));
              },
            );
        // Js.log(animalIds);
        let map = map->Belt.Map.Int.set(
          id,
          {
            Types.Map.point: idx,
            id,
            kind:
              Exhibit({
                animals: Belt.Set.Int.fromArray(Array.of_list(animalIds)),
                name: animal##name ++ " exhibit",
                terrain: rng->Prando.choose([|
                  "forest",
                  "grassland",
                  "prarie",
                  "stream"
                |]),
                id,
                sleepWeight: rng->Prando.int(5, 10),
                playWeight: rng->Prando.int(1, 5),
                eatWeight: rng->Prando.int(1, 5),
              }),
          },
        );
        (animalMap, map)
      },
    );

  let points =
    values
    ->Belt.Array.mapWithIndex((index, point) => (index, point))
    ->Belt.Array.keepMap(((index, point)) =>
        isGood(index)
          ? Some({
              Types.Map.building: None,
              id: index,
              pos: point,
              location: None,
            })
          : None
      );
  let points =
    Belt.Map.Int.fromArray(points->Belt.Array.map(item => (item.id, item)));

  let points =
    buildings->Belt.Map.Int.reduce(points, (points, _, building) =>
      Belt.Map.Int.(
        points->set(
          building.point,
          {...points->getExn(building.point), building: Some(building.id)},
        )
      )
    );

  let l1 = genId();

  let edges =
    Hashtbl.fold((k, (), keys) => [k, ...keys], edges, [])
    ->Belt.List.map(((p1, p2)) =>
        Types.Map.edge(genId(), p1, p2, l1, points)
      )
    ->Belt.List.map(item => (item.id, item))
    ->Belt.List.toArray
    ->Belt.Map.Int.fromArray;

  let pointToEdges = World.makeMapping(edges);
  open Belt.Map.Int;
  let map: Types.Map.map = {
    points,
    edges,
    pointToEdges,
    pointToPoint: World.pointToPoint(edges),
    buildings,
    locations: empty->set(l1, {Types.Map.id: l1, name: "the entrance"}),
    exits: Array.to_list(exits),
  };

  let world: Types.world = {
    rng,
    genId,
    clock: 0,
    maxPeoplePerExhibit: 20,
    peopleWhoLeft: [],
    people: empty,
    animals: animalMap,
    items: empty,
    map,
  };

  world;
};