
#![allow(dead_code)]
use std::collections::HashMap;

struct Edge {
    source: usize,
    dest: usize,
    length: f64,
    location: usize,
    // other properties?
}

struct Lock {
    is_locked: bool,
    strength: f64,
}

struct Door {
    is_open: bool,
    lock: Option<Lock>
}

struct Point {
    id: usize,
    door: Option<Door>,
    location: usize,
    location_section: Option<String>,
}

struct Location {
    name: String,
}

struct Map {
    points: HashMap<usize, Point>,
    edges: HashMap<usize, Edge>,
    locations: HashMap<usize, Location>,
    // point -> edges
    outgoing: HashMap<usize, Vec<usize>>,
    // point <- edges
    incoming: HashMap<usize, Vec<usize>>,
}

fn generate_map() {
}

struct Activity {
    kind: usize,
}

struct Memory {
    people: Vec<usize>,
    start_time: usize,
    end_time: usize,
    action: usize, // was it a collective action? could be a conversation. Could be multiple people having separate conversations.
}

struct Person {
    memories: Vec<Memory>,
}

fn generate_people() {
}

fn step() {}

fn main() {
    // use rand::SeedableRng;
    use rand_core::SeedableRng;
    let gen = rand_chacha::ChaChaRng::seed_from_u64(0);
    println!("Hello, world!");
}
