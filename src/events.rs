
#[derive(Debug)]
pub enum Happening {
    // maybe superfluous
    // Tell {
    //     teller: usize,
    //     tellee: usize,
    //     information: crate::knowledge::Knowable,
    // },
    Conversation {
        people: Vec<usize>,
        topic: String,
        // all of these will have source Person, with the one who said it.
        information: Vec<crate::knowledge::Knowledge>,
    },
    Travel {
        // source: usize,
        // dest: usize,
        path: Vec<usize>,
        conveyance: Option<String>,
        // do we need that?
        manner: Option<String>,
        people: Vec<usize>,
        // speed?
    },
    Eat {
        person: usize,
        what: Vec<usize>,
    },
    Bathroom {
        bathroom: usize,
        stall: usize,
    },
    Drink {
        bathroom: usize,
        fountain: usize,
    },
    // waiting in line, etc.
    Purchase {
        seller: usize,
        buyer: usize,
        cost: usize, // in cents
        items: Vec<usize>,
    },
    Give {
        giver: usize,
        receiver: usize,
        items: Vec<usize>,
    },
    Perform {
        animal: usize,
        trick: String,
    },
    Trip {
        person: usize,
        caught_self: bool,
    },
    Observe {
        person: usize,
        targets: Vec<ObservationTarget>,
    },
    Decide {
        person: usize,
        goal: crate::person::Goal,
    }
}

#[derive(Debug)]
pub enum ObservationTarget {
    Person(usize),
    Animal(usize),
    Habitat(usize),
    // Could also have like "the ocean" here if needed
}

// for memory, you go back to events the person has experienced,
// and see the associated emotions, and you can use that to influence
// their decisions
#[derive(Debug)]
pub struct Event {
    pub id: usize,
    pub participants: Vec<(usize, crate::person::Emotion)>,
    pub what: Happening,
    pub start: usize,
    pub end: usize,
}

// TODO write a prolog solver
