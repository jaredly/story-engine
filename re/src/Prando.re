
type t;

[@bs.module "prando"]
[@bs.new]
external make: int => t = "default";

[@bs.send]
external float: t => float = "next";

[@bs.send]
external range: (t, float, float) => float = "next";

[@bs.send]
external int: (t, int, int) => int = "nextInt";

[@bs.send]
external choose: (t, array('a)) => 'a = "nextArrayItem";

[@bs.send]
external reset: t => unit = "reset";