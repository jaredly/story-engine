
#![allow(dead_code)]
use std::collections::HashMap;
use rand::distributions::{Distribution, Uniform};

mod person;
mod knowledge;
mod events;
mod map;
mod world;
mod items;

fn generate_map() {
}

struct ActivityKind {
    solo: bool,
    time_range: (usize, usize),
}

struct Activity {
    kind: usize,
}

// struct Memory {
//     people: Vec<usize>,
//     start_time: usize,
//     end_time: usize,
//     action: usize, // was it a collective action? could be a conversation. Could be multiple people having separate conversations.
// }

// struct Person {
//     id: usize,
//     memories: Vec<Memory>,
// }

fn generate_people() {
}

fn step() {}

pub struct GenericRng<T: rand::Rng> {
    pub rng: T,
    uni: Uniform<f64>,
}

impl GenericRng<rand_pcg::Pcg64Mcg> {
    fn new(seed: u64) -> Self {
        use rand_core::SeedableRng;
        Rng {
            rng: rand_pcg::Pcg64Mcg::seed_from_u64(seed),
            uni: Uniform::from(0.0..1.0)
        }
    }

    pub fn next(&mut self) -> f64 {
        self.uni.sample(&mut self.rng)
    }

    pub fn choose<'a, T>(&mut self, items: &'a [T]) -> &'a T {
        let index = Uniform::from(0..items.len()).sample(&mut self.rng);
        &items[index]
    }
}

pub type Rng = GenericRng<rand_pcg::Pcg64Mcg>;

pub trait RandDefault {
    fn default(rng: &mut Rng) -> Self;
}

fn main() {
    let mut rand = Rng::new(0);
    println!("{} - {} - {}", rand.next(), rand.next(), rand.next());
    // use rand::SeedableRng;
    // let between = Uniform::from(0.0..1.0);
    // // rand_pcg::Pcg64Mcg
    // let gen = rand_pcg::Pcg64Mcg// Lcg128Xsl64
    // //  rand_chacha::ChaChaRng
    //  ::seed_from_u64(0);
    println!("Hello, world!");
}

