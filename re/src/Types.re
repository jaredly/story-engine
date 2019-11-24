
module Map = {

    type point = {
        id: int,
        building: option(int),
        pos: (float, float),
        location: option(int),
    }
    type location = {
        id: int,
        name: string,
    }

    type lock = {
        is_locked: bool,
        strength: float,
    }
    type door = {
        is_open: bool,
        lock: option(lock)
    }

    type edge = {
        id: int,
        source: int,
        dest: int,
        source_door: option(door),
        dest_door: option(door),
        length: float,
        location: int,
    }

    let dist = ((x1, y1), (x2, y2)) => {
        let dx = x2 -. x1;
        let dy = y2 -. y1;
        sqrt(dx *. dx +. dy *. dy)
    };

    let edge = (id, source, dest, location, points) => {id,source,dest,source_door: None, dest_door: None, length: {
        let s = points->Belt.Map.Int.getExn(source).pos;
        let d = points->Belt.Map.Int.getExn(dest).pos;
        dist(s, d)
    }, location}

    type foodForSale = {
        food: int,
        price: int,
        price_range: (int, int),
        quantity: (int, int),
        refill_timer: (int, int)
    }

    type buildingKind =
        | Exhibit(Belt.Set.Int.t, string, string)
        | FoodStand(Belt.Map.Int.t(foodForSale), string, string)

    type building = {
        id: int,
        point: int,
        kind: buildingKind
    }

    type map = {
        exits: list(int),
        points: Belt.Map.Int.t(point),
        locations: Belt.Map.Int.t(location),
        buildings: Belt.Map.Int.t(building),
        edges: Belt.Map.Int.t(edge),
        pointToEdges: Belt.Map.Int.t(list(int)),
        pointToPoint: Belt.Map.Int.t(Belt.Map.Int.t(int)),
    };

    let findPath = (map, p1, p2) => {
        Astar.js(
            ~start=p1,
            ~finish=p2,
            ~dist=(p1, p2) => {
                let s = map.points->Belt.Map.Int.getExn(p1).pos;
                let d = map.points->Belt.Map.Int.getExn(p2).pos;
                dist(s, d)
            },
            ~neighbors=p1 => {
                map.pointToPoint->Belt.Map.Int.getExn(p1)
                ->Belt.Map.Int.keysToArray
            }
        )
    }
}

open Map;



type emotionKind =
    | Happiness
    | Fear
    | Sadness
    | Anger
    | Lonliness
    | Surprise
    | Confusion;

type emotion = {
    kind: emotionKind,
    intensity: float,
    triggers: list(int),
    duration: int,
};

type characteristics = {
    curiousity: float,
    patience: float,
    excitability: float,
};

let characteristics = rng => {
    curiousity: rng->Prando.float,
    patience: rng->Prando.float,
    excitability: rng->Prando.float,
};

// type attribute = {
//     name: string,
//     value: float
// }
// in future, we could generaloize attributes
type condition = {
    thirst: float,
    hunger: float,
    sleepiness: float,
    stamina: float,
}

let condition = rng => {
    thirst: rng->Prando.range(0.0, 0.5),
    hunger: rng->Prando.range(0.0, 0.5),
    sleepiness: rng->Prando.range(0.0, 0.5),
    stamina: rng->Prando.range(0.5, 1.0),
};

// when making a choice, return the list of poassible choices ass well, their probabilities, and wwhy they were likely
// so, when a high likelihgood thing wasnt chosen, i can say something like
// evewn though she was tiredd, emma decided tyo stay a little longer so she could look at the monkeys

// module Knowledge = {
// }

type goalResult =
    | Failed(string)
    | InProcess(int) // timer for when to recheck
    | Succeeded(string)

type goalUpdater =
    | Goal('data, ('data, person, world) => ('data, goalResult, list((option(string), worldUpdate)))) : goalUpdater

and goal = {
    id: int,
    name: string,
    timer: int,
    updater: goalUpdater,
}

and personUpdate =
    | AddEmotion(emotion)
    | Characteristics(characteristics)
    | Condition(condition)
    | AddGoal(goal)
    | SetPosition(position)
    | Remove

and itemUpdate =
    | Transfer(itemOwner)
    | Remove

and worldUpdate =
    | Message(string)
    | Person(int, personUpdate)
    | Item(int, itemUpdate)
    | AddPerson(person)
    | AddItem(item)

and position = {
    edge: int,
    progress: float,
}

and person = {
    id: int,
    name: string,
    age: int,
    emotions: list(emotion),
    characteristics,
    condition,
    // knowledge: list(knowledge),
    goals: list(goal),
    position,
    offset: float,
}

and itemOwner =
    | Person(int)
    | Building(int)

and item = {
    id: int,
    name: string,
    owner: itemOwner,
}

and animalKind =
    | SpiderMonkey
    | Gorilla
    | Antelope
    | Giraffe
    | HammerHeadShark

and animal = {
    id: int,
    exhibit: int,
    kind: animalKind
}

and world = {
    rng: Prando.t,
    genId: unit => int,
    mutable people: Belt.Map.Int.t(person),
    mutable animals: Belt.Map.Int.t(animal),
    mutable items: Belt.Map.Int.t(item),
    mutable map
};

let names = [|"Aba", "Joe", "Misha", "Ginny", "Jessica", "Taylor", "Jessie"|];

module Updates = {
    let setPosition = (id, position): worldUpdate => {
        Person(id, SetPosition(position))
    }
    let addGoal = (id, goal): worldUpdate => {
        Person(id, AddGoal(goal))
    }
}

let closestPoint = (person, map) => {
    let edge = map.edges->Belt.Map.Int.getExn(person.position.edge);
    person.position.progress < 0.5 ? edge.source : edge.dest
}

let person = (id, rng, position) => {
    id: id,
    name: rng->Prando.choose(names) ++ " #" ++ string_of_int(id),
    age: rng->Prando.int(5, 25),
    emotions: [],
    characteristics: characteristics(rng),
    condition: condition(rng),
    // knowledge: list(knowledge),
    goals: [],
    position,
    offset: rng->Prando.range(-1.0, 1.0),
}
