use crate::events::Event;
use crate::items::{self, Food};
use crate::map::{self, Map};
use crate::person::{Emotion, Person};
use std::collections::HashMap;

#[derive(Debug)]
pub enum AnimalKind {
    SpiderMonkey,
    Gorilla,
}

// TODO activity, maybe activity history too?
#[derive(Debug)]
pub struct Animal {
    id: usize,
    exhibit: usize,
    kind: AnimalKind,
}

impl Animal {
    pub fn new(exhibit: usize, kind: AnimalKind) -> Self {
        Animal {
            id: 0,
            exhibit,
            kind,
        }
    }
}

#[derive(Default, Debug)]
pub struct World {
    pub last_id: usize,
    pub time: usize, // ticks, in increments of like 15 minutes probably, or something.
    pub map: Map,
    pub foods: HashMap<usize, Food>,
    pub animals: HashMap<usize, Animal>,
    pub people: HashMap<usize, Person>,
    pub events: HashMap<usize, Event>,
}

impl World {
    fn gen_id(&mut self) -> usize {
        self.last_id += 1;
        self.last_id
    }

    fn with_map(map: Map) -> Self {
        World {
            last_id: 0,
            time: 0,
            map,
            events: HashMap::default(),
            foods: HashMap::default(),
            animals: HashMap::default(),
            people: HashMap::default(),
        }
    }

    fn add_person_goal(
        &mut self,
        pid: usize,
        goal: crate::person::Goal,
        emotion: crate::person::Emotion,
        decision_start: usize,
    ) {
        let person = self.people.get_mut(&pid).unwrap();
        person.goals.push(goal);
        person.emotions.push(emotion.clone());

        self.add_event(crate::events::Event {
            id: 0,
            participants: vec![(pid, emotion)],
            what: crate::events::Happening::Decide { person: pid, goal },
            start: decision_start,
            end: self.time,
        });
        // self.people.get_mut(&pid).unwrap().experience_event(event, self.time)
    }

    fn add_event(&mut self, mut event: Event) -> usize {
        let id = self.gen_id();
        event.id = id;
        for (pid, emotion) in &mut event.participants {
            emotion.triggers.push(id);
            self.people
                .get_mut(&pid)
                .unwrap()
                .experience_event(id, self.time);
        }
        self.events.insert(id, event);

        id
        // let goal = *rand.choose(&possible_goals);
        // person.goals.push(goal);
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
(i) monkeys // 0,0
|
|
(h)         // 0,1
---
|
| simian trail
|
* (a)       // 0,2
  \
   \
    [donuts] (b) // 1,3
    |\
    | \
    |  * (c 2,4) -------- [(f 3,4) ----- (g 4,4) penguins]
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

fn pos(x: usize, y: usize) -> nalgebra::Point2<f64> {
    nalgebra::Point2::new(x as f64, y as f64)
}

pub fn zoo(rand: &mut crate::Rng) -> World {
    let mut world = World::default();

    let monkeys_loc = world.map.add_location("Simian Circle".to_owned());
    let simian_trail_loc = world.map.add_location("Simian Trail".to_owned());
    let food_loc = world.map.add_location("Munchie Cirlce".to_owned());
    let frigid_loc = world.map.add_location("Frigid Walk".to_owned());
    let penguins_loc = world.map.add_location("Penguins".to_owned());
    let welcome_loc = world.map.add_location("Welcome Blvd".to_owned());
    let entrance_loc = world.map.add_location("Entrance".to_owned());

    let sushi = world.add_food(items::food((
        "Piece of Sushi".to_owned(),
        "Pieces of Suchi".to_owned(),
    )));
    let ice_cream = world.add_food(items::food((
        "Ice cream cone".to_owned(),
        "Ice cream cones".to_owned(),
    )));
    let ice_cream_bar = world.add_food(items::food((
        "Ice cream bar".to_owned(),
        "Ice cream bars".to_owned(),
    )));

    let a = world.map.add_point(None, pos(0, 2));

    let b = world
        .map
        .add_building_point(
            food_loc,
            pos(1, 3),
            map::Building::new(map::food_building(
                "Ice cream shack".to_owned(),
                vec![
                    map::FoodForSale {
                        food: ice_cream,
                        price: 200,
                        price_range: (150, 350),
                        quantity: (50, 50),
                        refill_timer: (100, 10000), // very long time
                    },
                    map::FoodForSale {
                        food: ice_cream_bar,
                        price: 180,
                        price_range: (150, 350),
                        quantity: (50, 50),
                        refill_timer: (100, 10000), // very long time
                    },
                ],
            )),
        )
        .0;

    let c = world.map.add_point(None, pos(2, 4));
    let d = world
        .map
        .add_building_point(
            food_loc,
            pos(1, 5),
            map::Building::new(map::food_building(
                "Sushi".to_owned(),
                vec![map::FoodForSale {
                    food: sushi,
                    price: 300,
                    price_range: (250, 450),
                    quantity: (50, 50),
                    refill_timer: (100, 10000), // very long time
                }],
            )),
        )
        .0;
    let e = world.map.add_point(None, pos(0, 6));
    let f = world.map.add_point(None, pos(3, 4));
    let g = world
        .map
        .add_building_point(
            penguins_loc,
            pos(4, 4),
            map::Building::new(map::exhibit("Penguins".to_owned(), "Ice sheets".to_owned())),
        )
        .0;
    let h = world.map.add_point(None, pos(0, 1));
    let (i, monkeys_building) = world.map.add_building_point(
        monkeys_loc,
        pos(0, 0),
        map::Building::new(map::exhibit(
            "Spider Monkey Exhibit".to_owned(),
            "Trees".to_owned(),
        )),
    );
    let (l, gorillas_building) = world.map.add_building_point(
        monkeys_loc,
        pos(0, 0),
        map::Building::new(map::exhibit(
            "Gorilla Exhibit".to_owned(),
            "Trees".to_owned(),
        )),
    );
    let j = world.map.add_point(None, pos(0, 7));
    let k = world.map.add_point(Some(entrance_loc), pos(0, 8));
    let edge = map::Edge {
        id: 0,
        source: j,
        dest: k,
        length: 0.0,
        location: entrance_loc,
        source_door: None,
        dest_door: Some(map::Door {
            is_open: true,
            lock: None,
        }),
    };
    world.map.add_full_edge(edge);

    map::fully_conntected(&mut world.map, vec![a, b, c, d, e], food_loc);
    map::fully_conntected(&mut world.map, vec![i, h, l], monkeys_loc);
    // world.map.add_edge(i, h);

    world.map.add_edge_loc(h, a, simian_trail_loc);
    world.map.add_edge_loc(c, f, frigid_loc);
    world.map.add_edge(f, g);
    world.map.add_edge_loc(e, j, welcome_loc);

    world.add_animal(Animal::new(monkeys_building, AnimalKind::SpiderMonkey));
    world.add_animal(Animal::new(monkeys_building, AnimalKind::SpiderMonkey));
    world.add_animal(Animal::new(monkeys_building, AnimalKind::SpiderMonkey));

    world.add_animal(Animal::new(gorillas_building, AnimalKind::Gorilla));
    world.add_animal(Animal::new(gorillas_building, AnimalKind::Gorilla));
    world.add_animal(Animal::new(gorillas_building, AnimalKind::Gorilla));

    let possible_goals = crate::person::GoalKind::all_goals(&world.map.exhibit_ids());
    let possible_edges = world.map.edge_ids();

    for _ in 0..10 {
        let id = world.gen_id();
        use crate::RandDefault;
        let mut person = crate::person::Person::default(rand);
        person.id = id;
        person.position = (*rand.choose(&possible_edges), rand.next());
        world.people.insert(id, person);
        for _ in 0..2 {
            let goal = *rand.choose(&possible_goals);
            // person.goals.push(goal);
            world.add_person_goal(id, goal.as_goal(), Emotion::random(rand), world.time);
        }
    }

    world
}
