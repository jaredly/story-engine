[@bs.module "querystring"]
external stringify: Js.t('a) => string = "stringify";

[@bs.module "url"]
external urlParse:
  (string, bool) =>
  {
    .
    "query":
      Js.nullable({
        .
        "error": Js.nullable(string),
        "code": Js.nullable(string),
      }),
  } =
  "parse";

module LocalForage = {
  [@bs.module "localforage"]
  external getItem: string => Js.Promise.t(Js.nullable('a)) = "getItem";

  [@bs.module "localforage"]
  external setItem: (string, 'a) => Js.Promise.t(unit) = "setItem";

  [@bs.module "localforage"]
  external keys: unit => Js.Promise.t(array(string)) = "keys";
};

type blob;
type canvas;

[@bs.send] external toBlob: (canvas, blob => unit) => unit = "toBlob";
external asCanvas: Dom.element => canvas = "%identity";

[@bs.val] [@bs.scope "document"]
external documentGetElementById: string => Dom.element = "getElementById";

type headers;
[@bs.send] external get: (headers, string) => string = "get";

type response = {
  .
  "headers": headers,
  "status": int,
};

[@bs.val]
external fetch: (string, 'config) => Js.Promise.t(response) = "fetch";
[@bs.send] external json: response => Js.Promise.t('a) = "json";

type document;
[@bs.val] external documentEl: document = "document";
[@bs.get] external body: document => Dom.element = "body";
[@bs.send]
external appendChild: (Dom.element, Dom.element) => unit = "appendChild";
[@bs.send]
external removeChild: (Dom.element, Dom.element) => unit = "removeChild";

module Window = {
  type window;
  [@bs.val] external window: window = "window";
[@bs.send]
external addEventListener: (window, string, 'event => unit, bool) => unit =
  "addEventListener";
[@bs.send]
external removeEventListener: (window, string, 'event => unit, bool) => unit =
  "removeEventListener";
}

[@bs.set]
external textContent: (Dom.element, string) => unit = "textContent";
[@bs.get]
external value: (Dom.element) => int = "value";
[@bs.send]
external addEventListener: (Dom.element, string, 'event => unit, bool) => unit =
  "addEventListener";
[@bs.send]
external removeEventListener: (Dom.element, string, 'event => unit, bool) => unit =
  "removeEventListener";
[@bs.send]
external setAttribute: (Dom.element, string, string) => unit =
  "setAttribute";

[@bs.val] external document: Dom.element = "document";
[@bs.send]
external createElement: (Dom.element, string) => Dom.element = "createElement";
[@bs.set] external eventReturnValue: ('event, string) => unit = "returnValue";
[@bs.send]
external getElementById: (Dom.element, string) => option(Dom.element) =
  "getElementById";
[@bs.get] external offsetParent: Dom.element => Dom.element = "offsetParent";
type rect = {
  .
  "top": float,
  "height": float,
  "width": float,
  "left": float,
  "bottom": float,
  "right": float,
};
[@bs.send]
external getBoundingClientRect: Dom.element => rect = "getBoundingClientRect";

module Location = {
  type t;
  [@bs.val] external location: t = "location";
  [@bs.get] external hash: t => string = "hash";
  [@bs.set] external setHash: (t, string) => unit = "hash";
  let hash = () => location->hash;
  let setHash = hash => location->setHash(hash);
  let addHashListener = fn => {
    let listener = () => fn(hash());
    Window.window->Window.addEventListener("hashchange", listener, false);
    () => Window.window->Window.removeEventListener("hashchange", listener, false);
  };
};