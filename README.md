# s2geometry-node

[![CI](https://github.com/billyriantono/s2geometry-node/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/billyriantono/s2geometry-node/actions/workflows/ci.yml)
[![npm version](https://img.shields.io/npm/v/s2geometry-node.svg)](https://www.npmjs.com/package/s2geometry-node)
[![Node.js](https://img.shields.io/node/v/s2geometry-node.svg)](https://www.npmjs.com/package/s2geometry-node)
[![License: ISC](https://img.shields.io/badge/license-ISC-blue.svg)](LICENSE)

Node.js bindings for Google's [S2 Geometry Library](https://s2geometry.io/) — a hierarchical, spherical decomposition of the Earth used for spatial indexing, region coverings, and high-precision geometric predicates.

```js
const s2 = require('s2geometry-node');

const sf = new s2.S2LatLng(37.7879938, -122.4074374);
const cell = new s2.S2CellId(sf).parent(10);

console.log(cell.toToken());   // -> "8085b" (level-10 cell containing SF)
console.log(cell.face());      // -> 4 (S2 cube face)
```

---

## Highlights

- **Modern toolchain.** Tested on Node 18 / 20 / 22 across Linux, macOS (arm64), and Windows.
- **TypeScript ready.** Ships with `index.d.ts`; no `@types/` package needed.
- **Covers the practical S2 surface.** Cells, cell IDs, lat/lng rects, caps, region coverers — enough to do real spatial indexing.
- **Audited.** No `npm audit` advisories on the dependency tree.

## Install

```bash
npm install s2geometry-node
```

The addon is built from source at install time via `node-gyp`. You'll need:

| Platform | Toolchain                                                                 |
| -------- | ------------------------------------------------------------------------- |
| All      | Node 14+ &nbsp;·&nbsp; Python 3.6+                                        |
| Linux    | GCC 7+ or Clang 5+ &nbsp;·&nbsp; `make`                                   |
| macOS    | Xcode Command Line Tools                                                  |
| Windows  | Visual Studio Build Tools 2017+ &nbsp;·&nbsp; Windows SDK                 |

## Usage

### Convert between coordinate systems

```js
const ll = new s2.S2LatLng(37.78, -122.41);
const p  = ll.toPoint();             // -> S2Point (unit vector)
const id = new s2.S2CellId(ll);      // -> leaf-level S2CellId
const cell = new s2.S2Cell(id);      // -> S2Cell at the same level

console.log(id.level());             // 30
console.log(id.parent(10).toToken()); // "8085b"
```

### Cover a region

```js
const la = new s2.S2LatLng(34.0522, -118.2437);
const ny = new s2.S2LatLng(40.7128,  -74.0059);
const rect = new s2.S2LatLngRect(la, ny);

// Options-object form (closes #13):
const cells = s2.getCovering(rect, {
    min: 5,
    max: 10,
    max_cells: 20,
    level_mod: 2,
    result_type: 'token'    // 'cell' | 'cellId' | 'string' | 'token' | 'point'
});
// -> ['80c', '8087', '8089', ...]
```

### Inspect the cell pipeline

For debugging conversions against another S2 port (closes #15):

```js
const id = new s2.S2CellId(new s2.S2LatLng(37.78, -122.41)).parent(10);
console.log(id.toFaceIJ());
// -> { face: 4, i: 33030143, j: 158859263, orientation: 2 }
```

See the [API reference](API.md) for the full method list.

## What's exposed

| Class                | What it represents                                      |
| -------------------- | ------------------------------------------------------- |
| `S2Point`            | A 3D point on the unit sphere                           |
| `S2LatLng`           | A latitude/longitude pair                               |
| `S2CellId`           | A 64-bit hierarchical cell identifier                   |
| `S2Cell`             | A cell with efficient containment/area queries          |
| `S2Cap`              | A spherical cap (axis + height)                         |
| `S2LatLngRect`       | A closed lat/lng rectangle                              |
| `S1Angle`            | A one-dimensional angle                                 |
| `S1Interval`         | An interval on the unit circle                          |
| `S2RegionCoverer`    | Builds cell-set coverings of regions                    |
| `s2.getCovering()`   | Top-level convenience wrapping `S2RegionCoverer`        |

Several S2 types (`S2CellUnion`, `S2Loop`, `S2Polyline`, `S2Polygon`, `S2RegionIntersection`) are not wrapped yet — see the roadmap below.

## Using with Electron

This addon is built with [NAN](https://github.com/nodejs/nan), which is ABI-tied to a specific Node.js version. The binary `npm install` produces targets your system Node, so loading it from Electron will fail with:

```
A dynamic link library (DLL) initialization routine failed
```

Rebuild it against Electron's headers after install:

```bash
npm install s2geometry-node
npm install --save-dev @electron/rebuild
npx electron-rebuild -f -w s2geometry-node
```

Run `electron-rebuild` again whenever you upgrade Electron. (Migrating to Node-API would remove this rebuild step — tracked under "roadmap".)

## Roadmap

- Port additional region types: `S2CellUnion`, `S2Loop`, `S2Polyline`, `S2Polygon`.
- Migrate the C++ layer from NAN to Node-API so prebuilt binaries can ship per-platform via `prebuildify` and Electron rebuilds become unnecessary.
- Expand coordinate accessors (`STtoUV`, `UVtoST`, `FaceUVtoXYZ`, …) for full pipeline exposure.

## Contributing

1. `git clone` the repo, then `npm install` (this builds the native addon).
2. `npm test` runs the Mocha suite.
3. Every push and PR runs the CI matrix (Linux / macOS / Windows × Node 18 / 20 / 22).
4. Open an issue or PR — bug reports with reproductions are especially welcome.

## Credits

- The original [`mapbox/node-s2`](https://github.com/mapbox/node-s2) team for the initial bindings work.
- [@cypherix93](https://github.com/cypherix93) for the early Windows support.

## See also

- [`s2-geometry-javascript`](https://github.com/jonatkins/s2-geometry-javascript) — partial pure-JS port
- [`nodes2ts`](https://github.com/vekexasia/nodes2-ts) — TypeScript port
- [`gos2`](https://github.com/golang/geo) — Go port (now hosted on `golang/geo`)

## License

[ISC](LICENSE) © Billy Riantono. Contact: [@billyriantono](http://twitter.com/kebluk_id).
