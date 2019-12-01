
type city = {. "city": string, "state": string, "population": string};

[@bs.module]
external cities: array(city) = "../../assets/cities.json";

let cities = Js.Array2.copy(cities);

let getCity = rng => {
  let idx = rng->Prando.int(0, Array.length(cities) - 1);
  let city = cities->Js.Array2.spliceInPlace(~pos=idx, ~remove=1, ~add=[||])->Array.to_list->List.hd;
  city
}

type zoo = {
  world: Types.world,
  narratives: list((Types.person, Story.narrative)),
  wordCount: int,
  city: city,
}

let target = 50000;

let makeZoos = () => {
  let rec run = (rng, total, zoos) => {
    let city = getCity(rng);
    print_endline("Doing " ++ city##city);
    let size = rng->Prando.range(500., 1000.); // todo get from pop
    let world = RandomMap.gen(size, 85., size, rng);
    world->World.addPerson;
    while (!world.people->Belt.Map.Int.isEmpty) {
      world->World.step;
    };
    print_endline("Done with " ++ city##city);
    let people = world.peopleWhoLeft->Array.of_list;
    RandomMap.shuffle(people, rng);
    let numPeople = Array.length(people);
    let numReviewsToGet = rng->Prando.int(numPeople / 10, numPeople / 2);
    let narratives =
      people->Belt.Array.slice(~offset=0, ~len=numReviewsToGet)
      ->Belt.Array.map(person => (person, Story.narrate(world, person)))
      ->Belt.List.fromArray;
    Js.log2(narratives->Belt.List.length, "narratives");
    let zoo = {
      world,
      narratives,
      wordCount:
        narratives->Belt.List.reduce(0, (a, (_, b)) => a + b.wordCount),
      city,
    };
    let zoos = [zoo, ...zoos];
    let total = total + zoo.wordCount;
    total > target ? zoos : run(rng, total, zoos);
    // zoos
  };
  run(Prando.make(0), 0, []);
};

module Zoo = {
  [@react.component]
  let make = (~zoo) => {
    <div>
      <div className=Css.(style([
        fontSize(px(48)),
        marginBottom(px(32)),
        textAlign(`center),
        fontWeight(`bold),
      ]))>
        {React.string(
           "The " ++ zoo.city##city ++ ", " ++ zoo.city##state ++ " Zoo",
         )}
      </div>
      <div className=Css.(style([marginBottom(px(16)),
      fontSize(`percent(80.)),
      padding(px(16))
      ]))>
        {React.string("Containing the following exhibits: ")}
        {zoo.world.map.buildings->Belt.Map.Int.valuesToArray->Belt.Array.keepMap(building => switch building.kind {
          | Exhibit({name}) => Some(name)
          | _ => None
        })->Belt.List.fromArray |> String.concat(", ") |> React.string}
      </div>
      // {React.string(string_of_int(zoo.wordCount) ++ " words")}
      // {React.string(string_of_int(zoo.narratives->List.length) ++ " narratives")}
      {zoo.narratives
       ->Belt.List.toArray
       ->Belt.Array.mapWithIndex((i, (person, narrative)) =>
        <div className=Css.(style([
          marginBottom(px(16))
        ]))>
          <Monitor.Narrative key={string_of_int(i)} narrative person />
          </div>
         )
       ->React.array}
    </div>;
  };
};

[@react.component]
let make = (~zoos) => {
  <div>
    // {zoos->Belt.List.length->string_of_int->React.string}
    {zoos->Belt.List.toArray->Belt.Array.mapWithIndex((i, zoo) => <Zoo key={string_of_int(i)} zoo />)->React.array}
  </div>;
};