
module Point = {
    type point = {
        id: int,
        building: option(int),
        pos: (float, float),
        location: option(int),
    }
}

module Location = {
    type location = {
        id: int,
        name: string,
    }
}

module Edge = {

    type lock ={
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

open Point;
open Location;
open Edge;


type map = {
    points: Belt.Map.Int.t(point),
    locations: Belt.Map.Int.t(location),
    edges: Belt.Map.Int.t(edge),
}