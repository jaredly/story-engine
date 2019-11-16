
// Ok, we have
/*
- paths
- intersections
- and the joints between them I guess? Like how far between doors
- I guess we can say that a number of edges are in the same "location"

[ monkeys ]
| |
| |
| |
[ food place          _____
  couple of vendors ] _____ [ penguins ]
| |
| |

|  |  |  |  |  |
|  |  |  |  |  |
|  |  |  |  |  |
|  |  |  |  |  |
      |  |
      |  |
      |  |
|  |  |  |  |  | _____________ |  |  |  |  |
|  |  |  |  |  | _____________ |  |  |  |  |
|  |  |  |  |  |               |  |  |  |  |
|  |  |  |  |  |

SPOT = monkeys
SPOT = food place

hmmm maybe a physical approach is actually the easiest?

soooo it's still edges though, right?
But edges have names

ok, so one question was doors. Doors are on one end of an edge. not at a point.

*/

use std::collections::HashMap;

#[derive(Debug, Default)]
pub struct Lock {
    id: usize,
    is_locked: bool,
    strength: f64,
}

#[derive(Debug, Default)]
pub struct Door {
    is_open: bool,
    lock: Option<Lock>
}

#[derive(Debug, Default)]
pub struct Edge {
    id: usize,
    source: usize,
    dest: usize,
    source_door: Option<Door>,
    dest_door: Option<Door>,
    length: f64,
    location: usize,
    // other properties?
}

#[derive(Debug)]
pub struct Point {
    id: usize,
    building: Option<usize>,
    pos: nalgebra::Point2<f64>,
    location: Option<usize>, // might be spanning multiple locations
    // location: usize,
    // location_section: Option<String>,
}

impl Default for Point {
    fn default() -> Self {
        Point {
            id: 0,
            building: None,
            pos: nalgebra::Point2::new(nalgebra::zero(), nalgebra::zero()),
            location: None,
        }
    }
}

impl Point {
    pub fn with_loc(loc: usize) -> Self {
        let mut point = Self::default();
        point.location = Some(loc);
        point
    }
}

#[derive(Debug)]
pub struct Location {
    id: usize,
    name: String,
    // npcs?
    // items?
}

#[derive(Debug)]
pub struct FoodForSale {
    food: usize,
    price: usize, // in cents
    priceRange: (usize, usize),
    quantity: usize,
    refillTimer: (usize, usize),
}

#[derive(Debug)]
pub enum BuildingKind {
    FoodStand {
        foods: Vec<FoodForSale>,
        vendor: usize,
    },
    Bathroom {
        stalls: Vec<bool>, // occupied
        drinking_fountains: Vec<Vec<usize>>, // line of people
    },
    Exhibit {
        animals: Vec<usize>, // individual animal IDs I guess
        habitat: String, // a description of the habitat probably
        name: String,
    }
}

pub fn exhibit(name: String, habitat: String) -> BuildingKind {
    BuildingKind::Exhibit {
        animals: vec![],
        name,
        habitat
    }
}

#[derive(Debug)]
pub struct Building {
    id: usize,
    // TODO this means that a building could be on a point without
    // a location. Is that OK? I guess it could be at the "crossroads"
    // of a couple of places. Like a toll booth.
    // location: usize,
    point: usize,
    kind: BuildingKind,
}

impl Building {
    pub fn new(kind: BuildingKind) -> Self {
        Building {
            id: 0,
            // location,
            point: 0,
            kind,
        }
    }
}

#[derive(Default, Debug)]
pub struct Map {
    last_id: usize,
    points: HashMap<usize, Point>,
    edges: HashMap<usize, Edge>,
    buildings: HashMap<usize, Building>,
    locations: HashMap<usize, Location>,
    // point -> edges
    outgoing: HashMap<usize, Vec<usize>>,
    // point <- edges
    // incoming: HashMap<usize, Vec<usize>>,
}

impl Map {
    fn gen_id(&mut self) -> usize {
        self.last_id += 1;
        self.last_id
    }

    pub fn add_location(&mut self, name: String) -> usize {
        let id = self.gen_id();
        self.locations.insert(id, Location {id, name});
        id
    }

    pub fn add_animal(&mut self, exhibit: usize, animal: usize) {
        match &mut self.buildings.get_mut(&exhibit).unwrap().kind {
            BuildingKind::Exhibit { animals, .. } => animals.push(animal),
            _ => panic!("Not an exhibit {}", exhibit)
        }
    }

    pub fn add_point(&mut self, mut point: Point) -> usize {
        let id = self.gen_id();
        point.id = id;
        self.points.insert(id, point);
        id
    }

    pub fn add_building(&mut self, point_id: usize, mut building: Building) -> usize {
        let id = self.gen_id();
        building.id = id;
        let point = self.points.get_mut(&point_id).unwrap();
        if point.building.is_some() {
            panic!("Point {} already has a building", point_id);
        }
        point.building = Some(id);
        self.buildings.insert(id, building);
        id
    }

    pub fn point(&self, id: usize) -> Option<&Point> {
        self.points.get(&id)
    }

    pub fn add_edge(&mut self, source: usize, dest: usize) -> usize {
        let p1 = self.point(source).unwrap();
        let p2 = self.point(dest).unwrap();
        let length = nalgebra::distance(&p1.pos, &p2.pos);
        let location = match (p1.location, p2.location) {
            (None, None) => panic!("Adding an edge to two locationless points. Use add_full_edge"),
            (Some(loc), _) => loc,
            (_, Some(loc)) => loc,
        };
        self.add_full_edge(Edge {
            id: 0,
            source,
            dest,
            length,
            location,
            source_door: None,
            dest_door: None,
        })
    }

    pub fn add_full_edge(&mut self, mut edge: Edge) -> usize {
        let id = self.gen_id();
        edge.id = id;
        add_hash_vec(&mut self.outgoing, edge.source, id);
        add_hash_vec(&mut self.outgoing, edge.dest, id);
        self.edges.insert(id, edge);
        id
    }
}

fn add_hash_vec<K: std::cmp::Eq + std::hash::Hash, V>(hash: &mut HashMap<K, Vec<V>>, k: K, v: V) {
    match hash.get_mut(&k) {
        None => {hash.insert(k, vec![v]);},
        Some(vec) => vec.push(v)
    }
}

/*

[ monkeys ]
| |
| |
| |
[ food place          _____
  couple of vendors ] _____ [ penguins ]
| |
| |

*/

fn basic() -> Map {
    let mut map = Map::default();

    let monkeys_loc = map.add_location("Monkeys".to_owned());
    let monkeys_point = map.add_point(Point::with_loc(monkeys_loc));
    map.add_building(monkeys_point, Building::new(
        exhibit("Monkey Exhibit".to_owned(), "Trees".to_owned())
    ));

    // add a bunch of monkeys I guess
    // would be cool to also keep track of flora

    map
}

