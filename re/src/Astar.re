
[@bs.module]
external js: (~start: int, ~finish: int, ~dist: (int, int) => float, ~neighbors: int => array(int)) => Js.null(array(int)) = "./AstarJs";

// // function reconstruct_path(cameFrom, current)
// //     total_path := {current}
// //     while current in cameFrom.Keys:
// //         current := cameFrom[current]
// //         total_path.prepend(current)
// //     return total_path

// let reconstructPath = (cameFrom, current) => {
//     let rec loop = (current, path) => {
//         switch (cameFrom->Belt.Map.Int.get(current)) {
//             | None => []
//             | Some(prev) => loop(current, [prev, ...path])
//         }
//     }
//     // might need to reverse
//     loop(current, [])
// }

// let dist = ((x1, y1), (x2, y2)) => {
//     let dx = x2 -. x1;
//     let dy = y2 -. y1;
//     sqrt(dx *. dx +. dy *. dy)
// };

// let aStar = (start, goal, h, neighbors) => {
//     // let goalPos = points->Belt.Map.Int.getExn(goal).Types.pos;
//     // let h = (pid) => {
//     //     let pos = points->Belt.Map.Int.getExn(pid).pos;
//     // }
//     open Belt.HashMap.Int;
//     let openSet = Belt.HashSet.Int.make(~hintSize=100);
//     openSet->Belt.HashSet.Int.add(start);
//     let cameFrom = Belt.HashSet.Int.make(~hintSize=100);
//     let gScore = Belt.HashMap.Int.make(~hintSize=100);
//     gScore->set(start, 0);

//     let fScore = make(~hintSize=100);
//     fScore->set(start, h(start))

//     let rec loop = openSet => {

//     }
// }

// // A* finds a path from start to goal.
// // h is the heuristic function. h(n) estimates the cost to reach goal from node n.
// function A_Star(start, goal, h)
//     // The set of discovered nodes that may need to be (re-)expanded.
//     // Initially, only the start node is known.
//     openSet := {start}

//     // For node n, cameFrom[n] is the node immediately preceding it on the cheapest path from start to n currently known.
//     cameFrom := an empty map

//     // For node n, gScore[n] is the cost of the cheapest path from start to n currently known.
//     gScore := map with default value of Infinity
//     gScore[start] := 0

//     // For node n, fScore[n] := gScore[n] + h(n).
//     fScore := map with default value of Infinity
//     fScore[start] := h(start)

//     while openSet is not empty
//         current := the node in openSet having the lowest fScore[] value
//         if current = goal
//             return reconstruct_path(cameFrom, current)

//         openSet.Remove(current)
//         for each neighbor of current
//             // d(current,neighbor) is the weight of the edge from current to neighbor
//             // tentative_gScore is the distance from start to the neighbor through current
//             tentative_gScore := gScore[current] + d(current, neighbor)
//             if tentative_gScore < gScore[neighbor]
//                 // This path to neighbor is better than any previous one. Record it!
//                 cameFrom[neighbor] := current
//                 gScore[neighbor] := tentative_gScore
//                 fScore[neighbor] := gScore[neighbor] + h(neighbor)
//                 if neighbor not in openSet
//                     openSet.add(neighbor)

//     // Open set is empty but goal was never reached
//     return failure
