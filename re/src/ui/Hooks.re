let useState = initial =>
  React.useReducer((_, newState) => newState, initial);

