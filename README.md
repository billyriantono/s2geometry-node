# s2geometry-node

One day i need to play with S2 Geometry in Node, i found [node-s2](https://github.com/uber/node-s2), unfortunately that library is outdated only support node v0.10.0 so i am trying to make the  addon to be work again.

Bindings for the [S2 Geometry Library](https://code.google.com/p/s2-geometry-library/) into
[node.js](http://nodejs.org/).

For usage information, see the examples in the `examples/` directory.

## Installation Requirements

### All Platforms
* Node.js 14.0.0 or higher
* npm 6.0.0 or higher
* Python 3.6+ (for node-gyp)

### Windows
* Visual Studio Build Tools 2017 or later (or full Visual Studio)
* Windows SDK

### macOS
* Xcode Command Line Tools

### Linux
* GCC 7+ or Clang 5+
* Make
* Python 3.6+

## Recent Updates (v2.0.0)
* ✅ Migrated to NAN v2 for modern Node.js compatibility (14.x - 20.x)
* ✅ Updated all dependencies to latest versions
* ✅ Added Python 3 support for building
* ✅ Fixed deprecated V8 API usage
* ✅ Updated build configuration for modern operating systems

## Todo List
* Port additional S2 classes (S2CellUnion, S2Loop, S2Polyline, S2Polygon)
* Add TypeScript definitions
* Improve test coverage


## Currently Ported
* S2CellId 
* S2LatLng
* S2Point
* S2Cell
* S2Cap
* S2LatLngRect
* S1Interval
* S1Angle

## [Documentation: API.md](API.md)

## Using with Electron

This addon is built with [NAN](https://github.com/nodejs/nan), which is ABI-tied to a specific Node.js version. The binary produced by `npm install` targets your system Node, so loading it from Electron will fail with:

```
A dynamic link library (DLL) initialization routine failed
```

To use the addon inside an Electron app, rebuild it against Electron's headers after install:

```bash
npm install s2geometry-node
npm install --save-dev @electron/rebuild
npx electron-rebuild -f -w s2geometry-node
```

Run `electron-rebuild` again whenever you upgrade Electron. (Migrating this addon from NAN to Node-API would remove this rebuild step — tracked as future work.)

## Notes

Some of the S2 API is not wrapped because it isn't meant to be exposed in this way, and is blocked from wrapping by DISALLOW_COPY_AND_ASSIGN.

* S2CellUnion
* S2Loop
* S2Polyline
* S2Polygon
* S2RegionCoverer
* S2RegionIntersection

## See Also

* [s2-geometry-javascript](https://github.com/jonatkins/s2-geometry-javascript) - an incomplete port of S2 to JavaScript.
* [gos2](https://code.google.com/p/gos2/) - very incomplete port to Go
* [s2-geometry-php](https://github.com/eelf/s2-geometry-library-php) - another very incomplete port to PHP
* [mapbox/node-s2](https://github.com/mapbox/node-s2)
* [nodes2ts](https://github.com/vekexasia/nodes2-ts)

## Thanks
* mapbox team
* [cypherix93](https://github.com/cypherix93) - for make this libs can compile in Windows

## Contact Me 
[@billyriantono](http://twitter.com/kebluk_id) Feel free to contact me for help or anything else

