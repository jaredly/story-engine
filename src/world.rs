
use std::collections::HashMap;
use crate::map::{self, Map};
use crate::person::Person;
use crate::items::{self, Food};

pub enum AnimalKind {
    Monkey,
}

// TODO activity, maybe activity history too?
pub struct Animal {
    id: usize,
    exhibit: usize,
    kind: AnimalKind,
}

impl Animal {
    pub fn new(exhibit: usize, kind: AnimalKind) -> Self {
        Animal {id: 0, exhibit, kind}
    }
}

#[derive(Default)]
pub struct World {
    last_id: usize,
    map: Map,
    foods: HashMap<usize, Food>,
    animals: HashMap<usize, Animal>,
    people: HashMap<usize, Person>,
}

impl World {
    fn gen_id(&mut self) -> usize {
        self.last_id += 1;
        self.last_id
    }

    fn with_map(map: Map) -> Self {
        World {
            last_id: 0,
            map,
            foods: HashMap::default(),
            animals: HashMap::default(),
            people: HashMap::default(),
        }
    }

    fn add_animal(&mut self, mut animal: Animal) -> usize {
        let id = self.gen_id();
        animal.id = id;
        self.map.add_animal(animal.exhibit, id);
        self.animals.insert(id, animal);
        id
    }

    fn add_food(&mut self, mut food: Food) -> usize {
        let id = self.gen_id();
        food.id = id;
        self.foods.insert(id, food);
        id
    }
}

/*
[ monkeys ]
| |
| | simian trail
| |
[ food place          _____
  couple of vendors ] _____ [ penguins ]
| |                   frigid walk
| |
[ entrance ]

Food place

___
(i) monkeys
|
|
(h)
---
|
| simian trail
|
* (a)
  \
   \ 
    [donuts] (b)j
    |\
    | \
    |  * (c) -------- [(f) ----- (g) penguins]
    | /      frigid walk
    |/
    [sushi] (d)
   /
  /
* (e)
|
| welcome blvd
|
___
(j)
|
|
(k) entrance gate
---

*/

fn basic() -> World {
    let mut world = World::default();

    let monkeys_loc = world.map.add_location("Monkeys".to_owned());
    let simian_trail_loc = world.map.add_location("Simian Trail".to_owned());
    let food_loc = world.map.add_location("Munchie Cirlce".to_owned());
    let frigid_loc = world.map.add_location("Frigid Walk".to_owned());
    let penguins_loc = world.map.add_location("Penguins".to_owned());
    let welcome_loc = world.map.add_location("Welcome Blvd".to_owned());
    let entrance_loc = world.map.add_location("Entrance".to_owned());

    let sushi = world.add_food(items::food(("Piece of Sushi".to_owned(), "Pieces of Suchi".to_owned())));
    let ice_cream = world.add_food(items::food(("Ice cream cone".to_owned(), "Ice cream cones".to_owned())));
    let ice_cream_bar = world.add_food(items::food(("Ice cream bar".to_owned(), "Ice cream bars".to_owned())));

    let a = world.map.add_point(None);

    let b = world.map.add_building_point(food_loc, map::Building::new(
        map::food_building("Ice cream shack".to_owned(), vec![
            map::FoodForSale {
                food: ice_cream,
                price: 200,
                priceRange: (150, 350),
                quantity: (50, 50),
                refillTimer: (100, 10000), // very long time
            },
            map::FoodForSale {
                food: ice_cream_bar,
                price: 180,
                priceRange: (150, 350),
                quantity: (50, 50),
                refillTimer: (100, 10000), // very long time
            },
        ])
    )).0;

    let c = world.map.add_point(None);
    let d = world.map.add_building_point(food_loc, map::Building::new(
        map::food_building("Sushi".to_owned(), vec![
            map::FoodForSale {
                food: sushi,
                price: 300,
                priceRange: (250, 450),
                quantity: (50, 50),
                refillTimer: (100, 10000), // very long time
            }
        ])
    )).0;
    let e = world.map.add_point(None);
    let f = world.map.add_point(None);
    let g = world.map.add_building_point(penguins_loc, map::Building::new(
        map::exhibit("Penguins".to_owned(), "Ice sheets".to_owned())
    )).0;
    let h = world.map.add_point(None);
    let (i, monkeys_building) = world.map.add_building_point(monkeys_loc, map::Building::new(
        map::exhibit("Monkey Exhibit".to_owned(), "Trees".to_owned())
    ));
    let j = world.map.add_point(None);
    let k = world.map.add_point(Some(entrance_loc));
    let edge = map::Edge {
        id: 0,
        source: j,
        dest: k,
        length: 0.0,
        location: entrance_loc,
        source_door: None,
        dest_door: Some(map::Door {is_open: true, lock: None,})
    };
    world.map.add_full_edge(edge);

    map::fully_conntected(&mut world.map, vec![a, b, c, d, e]);

    world.map.add_edge(i, h);
    world.map.add_edge(h, a);
    world.map.add_edge(c, f);
    world.map.add_edge(f, g);
    world.map.add_edge(e, j);
    // world.map.add_edge(j, k);

    // let monkeys_building = world.map.add_building_point(i, map::Building::new(
    //     map::exhibit("Monkey Exhibit".to_owned(), "Trees".to_owned())
    // ));


    // let frigid_penguins = world.map.add_point();

    // let frigid_walk = world.map.add_edge(
    //     c,
    //     frigid_penguins
    // );




    world.add_animal(Animal::new(
        monkeys_building,
        AnimalKind::Monkey,
    ));
    world.add_animal(Animal::new(
        monkeys_building,
        AnimalKind::Monkey,
    ));
    world.add_animal(Animal::new(
        monkeys_building,
        AnimalKind::Monkey,
    ));

    world
}
