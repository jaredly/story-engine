
open Types;

type paragraph = list(string);
type narrative = {title: string, body: list(paragraph)};

let toHTML = narrative => {|<head><meta charset=utf8><title>|} ++ narrative.title ++ "</title></head><body>"
++ String.concat("\n", narrative.body->Belt.List.map(p => "<p>" ++ String.concat(" ", p) ++ "</p>"));

[@bs.send]
external padLeft: (string, int, string) => string = "padStart";

let clockTime = ticks => {
  let time = ticksToMinutes(ticks)->int_of_float;
  let hours = time / 60 + 9;
  let minutes = time mod 60;
  let pm = hours >= 12;
  let hours = hours > 12 ? hours - 12 : hours;
  string_of_int(hours) ++ ":" ++ string_of_int(minutes)->padLeft(2, "0") ++ (pm ? "pm" : "am")
}

// type nestedExperience = {
//   startTime: int,
//   kind: string,
//   // trail: list(string),
//   inner: list(nestedExperience)
// };

let countAnimals = actions => {
  actions->Belt.List.reduce(0, (num, (count, _)) => num + count)
}

let combineAnimalActions = (actions1, actions2) => {
  let map = Types.emptyAnimalBehaviorMap();
  let map =
    actions1->Belt.List.reduce(map, (map, (count, behavior)) =>
      map->Belt.Map.set(
        behavior,
        max(map->Belt.Map.getWithDefault(behavior, 0), count)
      )
    );
  let map =
    actions2->Belt.List.reduce(map, (map, (count, behavior)) =>
      map->Belt.Map.set(
        behavior,
        max(map->Belt.Map.getWithDefault(behavior, 0), count)
      )
    );
  map->Belt.Map.toList->Belt.List.map(((k, v)) => (v, k));
};

let collapseTwoExperiences = (a, b) => {
  switch (a.update, b.update) {
    | (SetPosition(_), SetPosition(_)) => Some({...b, startTime: a.startTime})
    | (Observe(oa), Observe(ob)) when oa == ob => Some({...b, startTime: a.startTime})
    | (Observe(AnimalActions(name, actions)), Observe(AnimalActions(name2, actions2))) when name == name2 =>
      Some({...a, endTime: b.endTime, update: Observe(AnimalActions(name, combineAnimalActions(actions, actions2)))})
    | _ => None
  }
};

let collapseExperiences = experiences => {
  let rec loop = (collected, list) => switch list {
    | [one, two, ...rest] => switch (collapseTwoExperiences(one, two)) {
      | None => loop([one, ...collected], [two, ...rest])
      | Some(one) => loop(collected, [one, ...rest])
    }
    | [one] => [one, ...collected]
    | [] => collected
  };
  loop([], experiences)->List.rev
};

[@ocaml.warning "-11"]
let narrateExperience = (rng, world, person, {update, startTime, endTime, condition}, before, after) => {
  let main = switch update {
  | SetPosition(_) => 
    Some(rng->Prando.choose([|
      "I walked for a while.",
      "I walked for " ++ string_of_int(int_of_float(ticksToMinutes(endTime - startTime))) ++ " minutes."
    |]))
  | Observe(NoAnimals) => switch (before, after) {
    | ([{update: Observe(AnimalActions(_, actions))}, ..._], _) => Some("Then it hid.")
    | (_, [{update: Observe(AnimalActions(_, _))}]) => Some("At first, I didn't see any animals.")
    | _ => Some("I saw no animals.")
  }
  | Observe(AnimalActions(name, actions) as obs) => Some(switch before {
    | [{update: Observe(NoAnimals)}, ..._] => "But then, I saw "
    | _ => "I saw "
  } ++ showObservation(obs) ++ ".")
  | Observe(obs) => Some("I observed " ++ showObservation(obs) ++ ".")
  | Remove => Some("I then left the zoo at " ++ clockTime(startTime) ++ "."
  )
  | AddGoal(GoToExhibit({attrs})) => 
    let (animals, name, terrain) = Types.Map.getExhibit(world.map, attrs);
    Some("I decided to go to the " ++ (name) ++ ".")
  | AddGoal(Leave(_)) =>
    condition.stamina < 0.2
    ? Some("I was out of energy at that point, and decided to leave.")
    : (startTime > closingTime - minutesToTicks(30.)
    ? Some("Since the zoo was closing soon, I headed toward the exit")
    : Some("Since I had seen all the exhibits, I decided to leave."))
  | RemoveGoal(GoToExhibit({timeStarted, attrs, result: Some(enjoyment)})) =>
    let (animals, name, terrain) = Types.Map.getExhibit(world.map, attrs);
    Some((if (enjoyment > 0.9) {
      ("The " ++ name ++ " was so cool!")
    } else if (enjoyment > 0.5) {
      ("The " ++ name ++ " was pretty cool!")
    } else if (enjoyment > 0.2) {
      ("The " ++ name ++ " was ok I guess.")
    } else {
      ("I didn't like the "++ name ++ " at all.")
    }))
    // }) ++ " I had stayed for " ++ Js.Float.toString(ticksToMinutes(endTime - timeStarted)) ++ " minutes")
  | _ => None
  };
  switch main {
    | None => None
    | Some(main) =>
      Some(if (rng->Prando.test(0.1)) {
        "At " ++ clockTime(startTime) ++ ", " ++ main
      } else {
        main
      })
  }
};

let narrateExperiences = (rng, world, person, experiences) => {
  let rec loop = (coll, before, after) => switch after {
    | [] => coll
    | [one, ...after] => loop(
      switch (narrateExperience(rng, world, person, one, before, after)) {
        | None => coll
        | Some(msg) => [msg, ...coll]
      },
      [one, ...before],
      after
    )
  }
  loop([], [], experiences->collapseExperiences)->List.rev
}

// so I want a way to calculate "interestingness" of an experience, and then,
// with some random chance based on the interestingness, we report on that.
// Also, different "reporters" will have different threshholds for how interesting something
// has to be in order to report on it?

let narrate = (person: person, world: world): narrative => {
  let rng = Prando.make(person.arrivalTime);
  let title = "A zoo story";
  let experiences = person.experiences->Belt.List.reverse;
  let body = [
    [
      "I entered the zoo at " ++ clockTime(person.arrivalTime) ++ "."
    ] @
    narrateExperiences(rng, world, person, experiences)
    // (experiences->collapseExperiences->Belt.List.keepMap(narrateExperience(world)))
  ];
  {title, body}
}
