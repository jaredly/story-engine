use strum_macros::{Display, EnumIter};

#[derive(EnumIter, Display)]
pub enum EmotionKind {
    Happiness,
    Fear,
    Sadness,
    Anger,
    Lonliness,
    Surprise,
    Confusion,
}

pub struct Emotion {
    kind: EmotionKind,
    intensity: f64,
    triggers: Vec<usize>,
    duration: f64,
}

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

pub struct Characteristics {
    curiousity: f64,
    patience: f64,
    excitability: f64,
}

pub enum Role {
    Vendor(usize), // building id
    Patron,
}

pub struct Person {
    id: usize,
    emotions: Vec<Emotion>,
    characteristics: Characteristics,
    knowledge: Vec<crate::knowledge::Knowledge>,
    // TODO something about memory
}

impl crate::RandDefault for Person {
    fn default(rand: &mut crate::Rng) -> Self {
        Person {
            id: 0,
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
