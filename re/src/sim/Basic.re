let gen = () => {
  open Belt.Map.Int;
  let genId = World.idGenerator();
  let map = {
    // let map: Types.Map.map = {
    //     points:empty,
    //     locations:empty,
    //     buildings:empty,
    //     edges:empty,
    // };

    open Types.Map;

    let p1 = genId();
    let p2 = genId();
    let p3 = genId();
    let p4 = genId();

    let e1 = genId();
    let e2 = genId();
    let e3 = genId();
    let e4 = genId();
    // let e5 = genId();

    let b1 = genId();
    let b2 = genId();

    let l1 = genId();
    let l2 = genId();

    let points =
      empty
      ->set(
          p1,
          {id: p1, building: None, pos: (0.0, 0.0), location: Some(l1)},
        )
      ->set(p2, {id: p2, building: None, pos: (10.0, 10.0), location: None})
      ->set(
          p3,
          {
            id: p3,
            building: Some(b1),
            pos: (10.0, 30.0),
            location: Some(l2),
          },
        )
      ->set(
          p4,
          {
            id: p4,
            building: Some(b2),
            pos: (30.0, 10.0),
            location: Some(l2),
          },
        );

    let edges =
      empty
      ->set(e1, edge(e1, p1, p2, l1, points))
      ->set(e2, edge(e2, p2, p3, l2, points))
      ->set(e3, edge(e3, p2, p4, l2, points))
      ->set(e4, edge(e4, p3, p4, l2, points));

    let pointToEdges = World.makeMapping(edges);

    let map = {
      points,
      edges,
      pointToEdges,
      pointToPoint: World.pointToPoint(edges),
      buildings:
        empty
        ->set(
            b1,
            {
              id: b1,
              point: p3,
              kind:
                Exhibit(
                  Belt.Set.Int.empty,
                  "Giraffe exhibit",
                  "grass and tall trees",
                ),
            },
          )
        ->set(
            b2,
            {
              id: b2,
              point: p4,
              kind:
                Exhibit(
                  Belt.Set.Int.empty,
                  "Hamerhead shark exhibit",
                  "a large tank full of water",
                ),
            },
          ),
      locations:
        empty
        ->set(l1, {id: l1, name: "the entrance"})
        ->set(l2, {id: l2, name: "the plaza"}),
      exits: [p1],
    };

    map;
  };

  let world: Types.world = {
    rng: Prando.make(0),
    genId,
    peopleWhoLeft: [],
    people: empty,
    animals: empty,
    items: empty,
    map,
  };

  world;
};