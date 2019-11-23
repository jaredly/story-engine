
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
}

open Map;

type map = {
    points: Belt.Map.Int.t(point),
    locations: Belt.Map.Int.t(location),
    edges: Belt.Map.Int.t(edge),
}


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
    | Goal('data, ('data) => ('data, goalResult, list((string, worldUpdate)))) : goalUpdater

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
    | Person(int, personUpdate)
    | Item(int, itemUpdate)
    | AddPerson(person)
    | AddItem(item)

and foodForSale = {
    food: int,
    price: int,
    price_range: (int, int),
    quantity: (int, int),
    refill_timer: (int, int)
}

and buildingKind =
    | Exhibit(Belt.Set.Int.t, string, string)
    | FoodStand(Belt.Map.Int.t(foodForSale), string, string)

and building = {
    id: int,
    point: int,
    kind: buildingKind
}

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
    mutable people: Belt.Map.Int.t(person),
    mutable animals: Belt.Map.Int.t(animal),
    mutable items: Belt.Map.Int.t(item),
    mutable map
}