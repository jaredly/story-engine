
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
        source: usize,
        dest: usize,
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
    }
}

pub enum ObservationTarget {
    Person(usize),
    Animal(usize),
    Habitat(usize),
    // Could also have like "the ocean" here if needed
}

pub enum Event {
    id: usize,
    what: Happening,
    start: f64,
    end: f64,
}
