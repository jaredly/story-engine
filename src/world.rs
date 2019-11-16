
use std::collections::HashMap;
use crate::map::{self, Map};
use crate::person::Person;

pub enum AnimalKind {
    Monkey,
}

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
            animals: HashMap::default(),
            people: HashMap::default(),
        }
    }

    fn add_animal(&mut self, mut animal: Animal) {
        let id = self.gen_id();
        animal.id = id;
        self.map.add_animal(animal.exhibit, id);
        self.animals.insert(id, animal);
    }
}

fn basic() -> World {
    let mut world = World::default();

    let monkeys_loc = world.map.add_location("Monkeys".to_owned());
    let monkeys_point = world.map.add_point(map::Point::with_loc(monkeys_loc));
    let monkeys_building = world.map.add_building(monkeys_point, map::Building::new(
        map::exhibit("Monkey Exhibit".to_owned(), "Trees".to_owned())
    ));
    world.add_animal(Animal::new(
        monkeys_building,
        AnimalKind::Monkey,
    ));

    world
}
