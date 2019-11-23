
/*
Eat -> ObtainFood, EatFood
ObtainFood -> assert food in inventory
ObtainFood -> ChooseFoodLocation, GoToLocation, BuyFood
ChooseFoodLocation -> ObtainKnowledgeOfFoodPlaces, ChooseBetweenFoodPlaces
ObtainKnowledgeOfFoodPlaces -> assert knowledge of at least one food place
ObtainKnowledgeOfFoodPlaces -> look around, if see some food places, you're done
ObtainKnowledgeOfFoodPlaces -> assert map in inventory, read map
ObtainKnowledgeOfFoodPlaces -> see someone official in proximity, ask them
GoToLocation -> PathFind, FollowPath
PathFind -> assert knowledge of path
PathFind -> assert map in inventory, inspect map
PathFind -> assert official person close, ask them

Root -> if hunger > 50%, Eat
Root -> Wander
Root -> Leave
Root -> PickExhibit
(if they have >50% hunger, but Eat isn't chosen, then have text like "even though she was hungry, she really wanted to see the hippos.")

Now, if you're in a group, this probably gets a lot more complicated.
Like, the parent asks the child if they want to pick an exhibit, and if they have knowledge
of any, then they pick that.
*/


pub mod inner {
    use crate::world::World;
    use crate::person::Person;
    trait Expand {
        fn expand(&self, person: Person, world: World) -> Vec<PlanOrAction>;
    }

    pub struct APlan {
        id: &'static str,
        name: &'static str,
        description: &'static str,

    }

    pub enum Goal {
        // Root,
        Leave,
        VisitExhibit,
        Eat,
    }

    impl Expand for Goal {
        fn expand(&self, person: Person, world: World) -> Vec<PlanOrAction> {
            use PlanOrAction as PA;
            match self {
                Leave => vec![PA::Plan(Plan::FindClosestExit)]
            }
        }
    }

    pub enum Plan {
        FindClosestExit, // hmm then I need to feed this forward...

        ObtainFood,
        EatFood,
        ChooseFoodLocation,
        BuyFood,
        NavigateTo(usize),
        ObtainKnowledgeOfFoodPlaces,
    }

    pub enum Action {
        Plan, // for expanding goals into sub-goals and actions
        FollowPath(Vec<usize>),
        Purchase {seller: usize, item: usize},
    }

    pub enum PlanOrAction {
        Plan(Plan),
        Action(Action)
    }
}

pub enum PlanOrAction {
    Plan(Plan),
    Action(Action)
}


pub struct Plan {
    inner: inner::Plan,
    completed: bool,
    children: Option<Vec<PlanOrAction>>,
}

pub struct Action {
    completed: bool,
    start_time: usize,
    completion_time: usize, // anticipated
    inner: inner::Action,
}

pub struct Goal {
    inner: inner::Goal,
    completed: bool,
    // if None, this goal hasn't been expanded. You'll need a `Plan` action to expand probably
    children: Option<Vec<PlanOrAction>>,
}
