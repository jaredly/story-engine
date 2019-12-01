
open Types;

type paragraph = list(string);
type narrative = {
  title: string,
  body: list(paragraph),
  stars: int,
  helpful: int,
  wordCount: int,
};

let wordCount = text => text->Js.String2.splitByRe([%bs.re "/\W+/g"])->Belt.Array.length;

let bodyWordCount = (body) => body->Belt.List.map(
  p => p->Belt.List.map(wordCount)->Belt.List.reduce(0, (+))
)->Belt.List.reduce(0, (+));

let rec repeat = (num, text) => num <= 0 ? "" : (text ++ repeat(num - 1, text));
let stars = num => repeat(num, {j|★|j}) ++ repeat(5 - num, {j|☆|j});
let starRating = float => Js.Math.round(float *. 4.)->int_of_float + 1;

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
    | (AddGoal(Leave(_)), SetPosition(_)) => Some(a)
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
    if (ticksToMinutes(endTime - startTime) > 10.) {

    Some(rng->Prando.choose([|
      "It took me a while to get there.",
      "I had been walking for a while.",
      "It was a " ++ string_of_int(int_of_float(ticksToMinutes(endTime - startTime))) ++ " minute walk."
    |]))
    } else {
      None
    }
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
    let exhibit = Types.Map.getExhibit(world.map, attrs);
    Some(rng->Prando.choose([|
      "I decided to go to the " ++ (exhibit.name) ++ ".",
      "I then went to the " ++ (exhibit.name) ++ ".",
      "I ended up at " ++ (exhibit.name) ++ ".",
    |]))
  | AddGoal(Leave(_)) =>
    condition.stamina < 0.2
    ? Some("I was out of energy at that point, and decided to leave.")
    : (startTime > closingTime - minutesToTicks(30.)
    ? Some("Since the zoo was closing soon, I headed toward the exit.")
    : Some("Since I had seen all the exhibits, I decided to leave."))
  | RemoveGoal(GoToExhibit({timeStarted, attrs, result: Some(enjoyment)})) =>
    let exhibit = Types.Map.getExhibit(world.map, attrs);
    Some((if (enjoyment > 0.9) {
      ("The " ++ exhibit.name ++ " was so cool!")
    } else if (enjoyment > 0.5) {
      ("The " ++ exhibit.name ++ " was pretty cool!")
    } else if (enjoyment > 0.2) {
      ("The " ++ exhibit.name ++ " was ok I guess.")
    } else {
      rng->Prando.choose([|
      "I didn't like the "++ exhibit.name ++ " at all.",
      "I wasn't impressed.",
      "It was pretty boring.",
      "I was expecting a little more excitement.",
      |])
    }))
    // }) ++ " " ++ stars(starRating(enjoyment)))
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

let narrate = (world: world, person: person): narrative => {
  let rng = Prando.make(person.arrivalTime);
  let title = "A zoo story";
  let experiences = person.experiences->Belt.List.reverse;
  let satisfactions = person.pastGoals->Belt.List.keepMap(({goal}) => switch goal {
    | GoToExhibit({result: Some(satisfaction)}) => Some(satisfaction)
    | _ => None
  });
  let avgSat = satisfactions->Belt.List.reduce(0., (+.)) /. float_of_int(Belt.List.length(satisfactions));
  let stars = rng->Prando.test(0.05)
  ? (avgSat > 0.5 ? 1 : 5)
  : int_of_float(Js.Math.round(avgSat *. 4.)) + 1;
  let body = [
    [
      "I entered the zoo at " ++ clockTime(person.arrivalTime) ++ "."
    ] @
    narrateExperiences(rng, world, person, experiences)
    // (experiences->collapseExperiences->Belt.List.keepMap(narrateExperience(world)))
  ];
  {title, body, stars, helpful: rng->Prando.int(0, 10), wordCount: bodyWordCount(body)}
}
