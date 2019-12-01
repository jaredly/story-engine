module Map = {
  type point = {
    id: int,
    building: option(int),
    pos: (float, float),
    location: option(int),
  };
  type location = {
    id: int,
    name: string,
  };

  type lock = {
    is_locked: bool,
    strength: float,
  };
  type door = {
    is_open: bool,
    lock: option(lock),
  };

  type edge = {
    id: int,
    source: int,
    dest: int,
    source_door: option(door),
    dest_door: option(door),
    length: float,
    location: int,
  };

  let dist = ((x1, y1), (x2, y2)) => {
    let dx = x2 -. x1;
    let dy = y2 -. y1;
    sqrt(dx *. dx +. dy *. dy);
  };

  let edge = (id, source, dest, location, points) => {
    id,
    source,
    dest,
    source_door: None,
    dest_door: None,
    length: {
      let s = points->Belt.Map.Int.getExn(source).pos;
      let d = points->Belt.Map.Int.getExn(dest).pos;
      dist(s, d);
    },
    location,
  };

  type foodForSale = {
    food: int,
    price: int,
    price_range: (int, int),
    quantity: (int, int),
    refill_timer: (int, int),
  };

  type buildingKind =
    | Exhibit(Belt.Set.Int.t, string, string)
    | FoodStand(Belt.Map.Int.t(foodForSale), string, string);

  type building = {
    id: int,
    point: int,
    kind: buildingKind,
  };

  type map = {
    exits: list(int),
    points: Belt.Map.Int.t(point),
    locations: Belt.Map.Int.t(location),
    buildings: Belt.Map.Int.t(building),
    edges: Belt.Map.Int.t(edge),
    pointToEdges: Belt.Map.Int.t(list(int)),
    pointToPoint: Belt.Map.Int.t(Belt.Map.Int.t(int)),
  };

  let getExhibit = (map, id) => switch (map.buildings->Belt.Map.Int.getExn(id).kind) {
    | Exhibit(a, b, c) => (a, b, c)
    | _ => failwith("not an exhibit")
  };

  let findPath = (map, p1, p2) => {
    Astar.js(
      ~start=p1,
      ~finish=p2,
      ~dist=
        (p1, p2) => {
          let s = map.points->Belt.Map.Int.getExn(p1).pos;
          let d = map.points->Belt.Map.Int.getExn(p2).pos;
          dist(s, d);
        },
      ~neighbors=
        p1 => {
          map.pointToPoint->Belt.Map.Int.getExn(p1)->Belt.Map.Int.keysToArray
        },
    );
  };
};

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
};

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

type gender =
  | Male
  | Female;

type demographics = {
  age: int,
  // is_birthday
  name: string,
  clothesColor: string,
  gender,
};

type exhibitResult =
  | Satisfied
  | Unsatisfied(string)

type animalBehavior =
  | Sleeping
  | Moving
  | Sitting
  | Eating
  | Playing

let behaviorIsInteresting = fun
  | Eating | Playing => true
  | _ => false;

let showBehavior = fun
  | Sleeping => "sleeping"
  | Sitting => "sitting"
  | Moving => "moving"
  | Eating => "eating"
  | Playing => "playing";

let behaviors = [|
  Sleeping,
  Sleeping,
  Sleeping,
  Sleeping,
  Sleeping,
  // Sleeping,
  // Sleeping,
  // Sleeping,
  // Sitting,
  // Sitting,
  // Sitting,
  // Sitting,
  Sitting,
  Sitting,
  Sitting,
  Sitting,
  Moving,
  Eating,
  Eating,
  Playing
|];

type animal = {
  id: int,
  exhibit: int,
  kind: string,
  behavior: animalBehavior,
  behaviorTimer: int,
  visibility: float,
};

type observation =
  | NoAnimals
  | AnimalActions(string, list((int, animalBehavior)));

type goalResult('result) =
  | Failed(string)
  | InProcess(int) // timer for when to recheck
  | Succeeded(string, 'result);

type goalUpdater('result) =
  | Goal(singleGoal('state, 'result)) : goalUpdater('result)

and singleGoal('state, 'output) = {
    kind: string,
    state: 'state,
    updater: ('state, person, world) => ('state, goalResult('output), list(goalUpdate))
}

and goalUpdate = {
    update: worldUpdate,
    // a list of goal IDs probably
    trail: list(string),
}

and goal('attrs, 'result) = {
  id: int,
  name: string,
  attrs: 'attrs,
  result: option('result),
  timer: int,
  contents: goalUpdater('result),
  timeStarted: int,
}

and anyGoal =
  | GoToExhibit(goal(int, exhibitResult))
  | Leave(goal(int, unit))

and pastGoal = {
    goal: anyGoal,
    failed: option(string),
    timeStopped: int,
}

and genericGoal = GenericGoal(goal('a, 'b)) : genericGoal

and personUpdate =
  | AddEmotion(emotion)
  | Characteristics(characteristics)
  | Observe(observation)
  | Condition(condition)
  | AddGoal(anyGoal)
  // FinishGoal(anyGoal)
  | SetPosition(position)
  // Notice
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

and personExperience = {
  time: int,
  goalTrail: list(string),
  update: personUpdate,
}

and person = {
  id: int,
  demographics,
  // name: string,
  // age: int,
  emotions: list(emotion),
  characteristics,
  condition,
  // knowledge: list(knowledge),
  goals: list(anyGoal),
  pastGoals: list(pastGoal),
  position,
  offset: float,
  // updates? events? experiences?
  // time, trail, personUpdate
  experiences: list(personExperience),
}

and itemOwner =
  | Person(int)
  | Building(int)

and item = {
  id: int,
  name: string,
  owner: itemOwner,
}

and world = {
  rng: Prando.t,
  genId: unit => int,
  maxPeoplePerExhibit: int,
  /** Minutes since opening time, at 9am */
  mutable clock: int,
  mutable peopleWhoLeft: list(person),
  mutable people: Belt.Map.Int.t(person),
  mutable animals: Belt.Map.Int.t(animal),
  mutable items: Belt.Map.Int.t(item),
  mutable map,
};

// let names = [|"Aba", "Joe", "Misha", "Ginny", "Jessica", "Taylor", "Jessie"|];

module Updates = {
  let setPosition = (id, position): worldUpdate => {
    Person(id, SetPosition(position));
  };
  let addGoal = (id, goal): worldUpdate => {
    Person(id, AddGoal(goal));
  };
  let removePerson = id => Person(id, Remove);
};

let closestPoint = (person, map) => {
  let edge = map.edges->Belt.Map.Int.getExn(person.position.edge);
  person.position.progress < 0.5 ? edge.source : edge.dest;
};

[@bs.module] [@bs.val]
external names: {
  .
  "m": array(string),
  "f": array(string),
} =
  "../../assets/names.json";

let chooseName = (rng, gender) => {
  let names = gender == Female ? names##f : names##m;
  rng->Prando.choose(names);
};

let person = (id, rng, position) => {
  let gender = rng->Prando.choose([|Male, Female|]);
  {
    id,
    // name: rng->Prando.choose(names) ++ " #" ++ string_of_int(id),
    demographics: {
      age: rng->Prando.int(5, 25),
      name: rng->chooseName(gender),
      clothesColor:
        "hsl(" ++ string_of_int(rng->Prando.int(0, 360)) ++ ",100%,50%)",
      gender,
    },
    emotions: [],
    characteristics: characteristics(rng),
    condition: condition(rng),
    // knowledge: list(knowledge),
    goals: [],
    pastGoals: [],
    position,
    offset: rng->Prando.range(-1.0, 1.0),
    experiences: [],
  };
};

let getGoal = goal => switch goal {
  | GoToExhibit(inner) => GenericGoal(inner)
  | Leave(inner) => GenericGoal(inner)
};

type goalMapper('input, 'extra) = {run: 'a 'b . ('input, goal('a, 'b)) => (goal('a, 'b), 'extra) };

let mapGoal = (goal, input, f) => switch goal {
  | GoToExhibit(inner) => 
  let (inner, extra) = f.run(input, inner);
  (GoToExhibit(inner), extra)
  | Leave(inner) => 
  let (inner, extra) = f.run(input, inner);
  (Leave(inner), extra)
}

let showGoal = goal => switch goal {
  | GoToExhibit({attrs}) => "Go to exhibit " ++ string_of_int(attrs)
  | Leave({attrs}) => "Leave the zoo by exit " ++ string_of_int(attrs)
};

let showObservation =
  fun
  | NoAnimals => "no animals"
  | AnimalActions(name, behaviors) =>
    behaviors->Belt.List.map(((count, behavior)) => {
      string_of_int(count) ++ " " ++ name ++ "s " ++ showBehavior(behavior);
    }) |> String.concat(" and ");

let showPersonUpdate = fun
  | AddEmotion(emotion) => "Add emotion"
  | Characteristics(characteristics) => "Characteristics"
  | Condition(condition) => "Condition"
  | AddGoal(anyGoal) => "Add goal " ++ showGoal(anyGoal)
  | SetPosition(position) => "Set position"
  | Observe(observation) => "Observe " ++ showObservation(observation)
  // Notice
  | Remove => "Remove";

module CmpAnimalBehavior = Belt.Id.MakeComparable({
  type t = animalBehavior;
  let cmp = compare;
});
let emptyAnimalBehaviorMap = () => Belt.Map.make(~id=(module CmpAnimalBehavior));