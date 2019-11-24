
const reconstruct = (cameFrom, current) => {
    const path = [current];
    while (cameFrom[current] != null) {
        current = cameFrom[current]
        path.unshift(current)
    }
    return path
}

const aStar = (start, goal, h, neighbors) => {
    const open = [start];
    const cameFrom = {};
    const gScore = {};
    gScore[start] = 0;
    let g = v => gScore[v] == null ? Infinity : gScore[v]
    const fScore = {};
    fScore[start] = h(start, goal);
    let f = v => fScore[v] == null ? Infinity : fScore[v];
    while (open.length) {
        open.sort((a, b) => f(a) - f(b))
        let current = open.shift();
        if (current == goal) {
            return reconstruct(cameFrom, current)
        }
        const n = neighbors(current)
        // console.log(n, current)
        n.forEach(neighbor => {
            let tentative = g(current) + h(current, neighbor)
            if (tentative < g(neighbor)) {
                cameFrom[neighbor] = current
                gScore[neighbor] = tentative
                fScore[neighbor] = tentative + h(neighbor, goal)
                if (!open.includes(neighbor)) {
                    open.push(neighbor)
                }
            }
        })
    }
    // console.log(cameFrom)

    return null
}

const dist = (p1, p2) => {
    const dx = p2[0] - p1[0]
    const dy = p2[1] - p1[1]
    return Math.sqrt(dx * dx + dy * dy)
}

module.exports = aStar

// const test = () => {
//     const points = [[0, 0], [5, 5], [2, 7], [4, 8], [3, 5]]
//     const edges = [[0, 1], [1, 2], [1, 4], [0, 3], [3, 4]]

//     const h = (p1, p2) => (console.log(p1,p2), dist(points[p1], points[p2]))
//     const neighbors = p1 => {
//         return edges.filter(e => e[0] == p1 || e[1] == p1 )
//         .map(e => e[0] == p1 ? e[1] : e[0])
//         // .map(p2 => ({id: p2, cost: dist(points[p1], points[p2])}))
//     }

//     console.log(aStar(0, 4, h, neighbors))
// }
// test()