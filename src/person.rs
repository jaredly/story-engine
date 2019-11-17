use strum_macros::{Display, EnumIter};
use crate::{events, knowledge};


// DND has
/*
strength
dexterity
constitution
intelligence
wisdom
charisma

I dunno if those are quite the ones I care about.
For the zoo, which is what we're doing right now by the way,
I want things like

- curiousity
- patience
- excitability / enthusiasm
- temper

there's myers-briggs I think, which, like, has maybe been debunked?
Mind (Introverted / Extroverted)
Energy (observant / intuitive)
Nature (thinking / feeling)
Tactics (judging / prospecting)
Identity (Assertive / Turbulent)

Umm here's a list of a couple hundred personality traits
http://ideonomy.mit.edu/essays/traits.html


*/

#[derive(EnumIter, Display, Clone, Copy, Debug)]
pub enum EmotionKind {
    Happiness,
    Fear,
    Sadness,
    Anger,
    Lonliness,
    Surprise,
    Confusion,
}

impl EmotionKind {
    fn random(rand: &mut crate::Rng) -> Self {
        use strum::IntoEnumIterator;
        *rand.choose(&Self::iter().collect::<Vec<Self>>())
    }
}

#[derive(Debug, Clone)]
pub struct Emotion {
    pub kind: EmotionKind,
    pub intensity: f64,
    pub triggers: Vec<usize>, // an event id probably
    pub duration: usize,
}

impl Emotion {
    pub fn random(rand: &mut crate::Rng) -> Self {
        Emotion { 
            kind: EmotionKind::random(rand),
            intensity: 0.5,
            triggers: vec![],
            duration: 100,
        }
    }
}

#[derive(Debug)]
pub struct Characteristics {
    curiousity: f64,
    patience: f64,
    excitability: f64,
}

#[derive(Debug)]
pub enum Role {
    Vendor(usize), // building id
    Patron,
}

#[derive(Clone, Copy, Debug)]
pub enum Goal {
    Eat,
    ViewExhibit(usize),
    Leave,
}

impl Goal {
    pub fn all_goals(exhibits: &[usize]) -> Vec<Goal> {
        let mut goals = vec![
            Goal::Eat,
            Goal::Leave,
        ];
        for id in exhibits {
            goals.push(Goal::ViewExhibit(*id));
        }
        goals
    }
}

#[derive(Debug)]
pub struct Person {
    pub id: usize,
    pub name: String,
    pub age: u8,
    pub emotions: Vec<Emotion>,
    pub stamina: f64,
    pub characteristics: Characteristics,
    pub knowledge: Vec<crate::knowledge::Knowledge>,
    pub goals: Vec<Goal>,
    pub position: (usize, f64), // edge id, position from source to sink
    // TODO something about memory
}

impl Person {
    // fn decide_on_goal(&mut self, goal: Goal) -> events::Event {
    //     self.goals.push(goal);
    // }
    pub fn experience_event(&mut self, id: usize, time: usize) {
        self.knowledge.push(knowledge::Knowledge {
            id: 0, // tODO
            learned_time: time,
            item: knowledge::Knowable::Experience(id),
            source: knowledge::KnowledgeSource::Experience(id),
            reliability: 1.0, // TODO modulate based on characteristics
            memorability: 1.0, // umm
            confidence: 1.0, // umm
            suprisingness: 0.0, // umm
        })
    }
    // fn add_random_goal(&mut self, rand: &mut crate::Rng) {
    //     let goal = *rand.choose(&vec![
    //         Goal::Eat,
    //         Goal::ViewExhibit(0),
    //         Goal::Leave
    //     ])
    // }
}

use rand::distributions::{Distribution, Uniform};

impl crate::RandDefault for Person {
    fn default(rand: &mut crate::Rng) -> Self {
        Person {
            id: 0,
            name: (*rand.choose(&vec![
                "Sarah",
                "Jared",
                "Micheal",
                "June",
            ])).into(),
            age: Uniform::new(4, 40).sample(&mut rand.rng),
            position: (0, 0.0),
            goals: vec![ ],
            stamina: rand.next() * 0.2 + 0.8,
            // Ok, so we generate a couple of random emotions for the person,
            // and come up wiht reasons for them....
            // Orr we generate a couple of events for the person, and then
            // those have the natural emotional effect?
            // Orr we just start everyone off as blank slate? Yeah that's easier.
            emotions: vec![],
            knowledge: vec![],
            characteristics: Characteristics {
                curiousity: rand.next(),
                patience: rand.next(),
                excitability: rand.next()
            }
        }
    }
}
