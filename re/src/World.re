
open Types;

let idGenerator = () => {
    let last = ref(0);
    () => {
        last := last^ + 1;
        last^
    }
};
let genId = idGenerator();
let empty = Belt.Map.Int.empty;

let map = {
    let map = {
        points:empty,
        locations:empty,
        edges:empty,
    };
    map
}


let world: world = {
    people: empty,
    animals: empty,
    items: empty,
    map
}
