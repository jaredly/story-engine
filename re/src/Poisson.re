type poisson;

[@bs.module]
[@bs.new]
external make:
  (
    ~shape: array(float),
    ~minDist: float,
    ~maxDist: float,
    ~maxTries: int,
    ~rng: unit => float
  ) =>
  poisson =
  "poisson-disk-sampling";

[@bs.send] external fill: poisson => array((float, float)) = "fill";

// type delaunay = {. "triangles": array(int)};
// [@bs.module "delaunator"]
// [@bs.val]
// [@bs.scope "default"]
// external delaunay: (array((float, float))) => delaunay = "from"

type delaunay = {. "triangles": array(int)};
[@bs.module "d3-delaunay"]
[@bs.val]
[@bs.scope "Delaunay"]
external delaunay: (array((float, float))) => delaunay = "from"

type voronoi;

[@bs.send]
external voronoi: (delaunay, (float, float, float, float)) => voronoi = "voronoi";
[@bs.send]
external cellPolygons: voronoi => Js.Array.array_like(array((float, float))) = "cellPolygons";
let cellPolygons = (v):array(array((float,float))) => cellPolygons(v)->Js.Array.from;
[@bs.send]
external cellPolygon: (voronoi, int) => array((float, float)) = "cellPolygon"