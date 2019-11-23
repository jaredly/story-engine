
type emotionKind =
    | Happiness
    | Fear
    | Sadness
    | Anger
    | Lonliness
    | Surprise
    | Confusion;

type emotion = {
    kind: emotionKind,
    intensity: float,
    triggers: list(int),
    duration: int,
};

type characteristics = {
    curiousity: float,
    patience: float,
    excitability: float,
};

// module Knowledge = {

// }

type person = {
    id: int,
    name: string,
    age: int,
    emotions: list(emotion),
    stamina: float,
    characteristics,
    // knowledge: list(knowledge),
    // goals: list(goal),
    position: (int, float),
}
