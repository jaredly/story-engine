
open Types;

let idGenerator = () => {
    let last = ref(0);
    () => {
        last := last^ + 1;
        last^
    }
};
let genId = idGenerator();

let world: world = {
    people: Belt.Map.Int.emty
}
