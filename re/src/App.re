// [%%debugger.chrome];
[%bs.raw "require('./ui/FormatBelt.js')"];

// ReactDOMRe.renderToElementWithId(<Play />, "root");

let zoos = Book.makeZoos();
ReactDOMRe.renderToElementWithId(<Book zoos />, "root");