// on https://en.wikipedia.org/wiki/List_of_animal_names
// having selected the main names table
// copy(JSON.stringify([...$0.querySelectorAll('tr > td:first-child > a:first-child')].map(a => ({name: a.textContent, href: a.href})).filter(m => m.name.match(/^[a-zA-Z]+$/)), null, 2))