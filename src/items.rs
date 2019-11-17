
#[derive(Debug)]
pub struct Food {
    pub id: usize,
    pub name: (String, String),
}

pub fn food(name: (String, String)) -> Food {
    Food {
        id: 0, name
    }
}
