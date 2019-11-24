
const allNames = {
  m: {},
  f: {},
}

const fs = require('fs')
fs.readdirSync('./ssa-names')
.filter(name => name.endsWith('.txt'))
.forEach(name => {
  const year = +name.slice(3).split('.')[0]
  const full = './ssa-names/' + name;
  const lines = fs.readFileSync(full, 'utf8').split('\r\n')
  .filter(line => line.trim())
  .map(line => line.split(','));
  lines.forEach(([name, sex, count]) => {
    sex = sex.toLowerCase();
    allNames[sex][name] = (allNames[sex][name] || 0) + (+count)
  })
})

console.log(Object.keys(allNames.m).length)
console.log(Object.keys(allNames.f).length)
fs.writeFileSync('./names.json', JSON.stringify({
  m: Object.keys(allNames.m).map(name => [allNames.m[name], name])
  .sort((a, b) => a[0] - b[0]).slice(-2000, -1000)
  .map(n => n[1])
  ,
  f: Object.keys(allNames.f).map(name => [allNames.f[name], name])
  .sort((a, b) => a[0] - b[0]).slice(-2000, -1000)
  .map(n => n[1])
  ,
}, null, 2))
