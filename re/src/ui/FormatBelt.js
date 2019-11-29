
if (!window.devtoolsFormatters) {
  window.devtoolsFormatters = []
}

const mapSize = map => {
  if (!map || !map.value) return 0;
  return mapSize(map.left) + mapSize(map.right) + 1
}

const collectMapItems = (dest, map) => {
  if (!map || map.key == null) return
  dest.push([map.key, map.value])
  collectMapItems(dest, map.left);
  collectMapItems(dest, map.right);
}

const mapFormatter2 = {
  header: obj => {
    if (
      typeof obj !== 'object' ||
      obj.key === 'undefined' ||
      typeof obj.left !== 'object' ||
      typeof obj.right !== 'object' ||
      typeof obj.height !== 'number' ||
      obj.value === undefined
    ) {
      return null
    }
    const items = [];
    collectMapItems(items, obj)
    return ['div', {}, 'Belt.Map', ['object', {object: items}]]
    // return ['array', {items: items}]
    // return ['span', {}, `Map(count=${mapSize(obj)})`]
  },
  hasBody: x => false,
  body: x => [],
}

window.devtoolsFormatters.push(mapFormatter2)
