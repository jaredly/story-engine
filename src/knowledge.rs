

#[derive(Debug)]
pub enum Knowable {
    Directions { source: usize, dest: usize, path: Vec<usize> },
    // maybe have PersonFact, where you know something about the
    // person's personality or somethign?
    AnimalFact { animal: String, fact: String },
    // knowledge of an experience
    // this doesn't allow for partial knowledge, though.
    // Not sure how to model that.
    Experience(usize),
    // ItemInformation?
    // ItemSource? where it came from, when/where/how it was created.
    // but then also we have knowledge that's generailzable -- like how
    // if an ice cream vendor hands me a code, I know how that ice cream
    // came to be. Or I assume, from my other knowledge.
}

#[derive(Debug)]
pub enum KnowledgeSource {
    // if none, that person dodn't tell me where they got it from
    Person(usize, Option<Box<KnowledgeSource>>),
    Object(usize),
    Experience(usize),
}

#[derive(Debug)]
pub struct Knowledge {
    pub id: usize,
    pub item: Knowable,
    pub source: KnowledgeSource,
    pub reliability: f64, // of the source, maybe?
    pub memorability: f64,
    pub confidence: f64, // maybe redundant
    pub suprisingness: f64, // or something. novelty?
    pub learned_time: usize,
    // this will impact memorability. or maybe that's handled already?
}
