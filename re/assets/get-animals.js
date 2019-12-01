// on https://en.wikipedia.org/wiki/List_of_animal_names
// having selected the main names table
// copy(JSON.stringify([...$0.querySelectorAll('tr > td:first-child > a:first-child')].map(a => ({name: a.textContent, href: a.href})).filter(m => m.name.match(/^[a-zA-Z]+$/)), null, 2))


// on https://en.wikipedia.org/wiki/List_of_United_States_cities_by_population
// with the table selected
// copy(JSON.stringify([...$0.querySelectorAll('tr')].slice(1).map(row => {let cells = row.querySelectorAll('td'); if (cells.length === 0) return null; return {city: cells[1].querySelector('a').textContent.trim(), state: cells[2].textContent.trim(), population: cells[3].textContent.trim()}}), null, 2))