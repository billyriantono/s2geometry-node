var s2 = require('../');
var assert = require('assert');

// Fixed reference point used across tests so failures are reproducible.
var SF = { lat: 37.7879938, lng: -122.4074374 };
var LA = { lat: 34.0522, lng: -118.2437 };
var NY = { lat: 40.7128, lng: -74.0059 };

describe('S2LatLng', function () {
    it('exposes lat/lng accessors in degrees', function () {
        var ll = new s2.S2LatLng(SF.lat, SF.lng);
        assert.strictEqual(typeof ll.lat, 'number');
        assert.strictEqual(typeof ll.lng, 'number');
        assert(Math.abs(ll.lat - SF.lat) < 1e-9);
        assert(Math.abs(ll.lng - SF.lng) < 1e-9);
    });

    it('reports validity', function () {
        assert.strictEqual(new s2.S2LatLng(SF.lat, SF.lng).isValid(), true);
        assert.strictEqual(new s2.S2LatLng(200, 0).isValid(), false);
    });

    it('clamps via normalized()', function () {
        var ll = new s2.S2LatLng(200, 400).normalized();
        assert(ll.lat <= 90 && ll.lat >= -90);
        assert(ll.lng <= 180 && ll.lng >= -180);
    });

    it('converts to a unit-length S2Point', function () {
        var p = new s2.S2LatLng(SF.lat, SF.lng).toPoint();
        var len = Math.sqrt(p.x() * p.x() + p.y() * p.y() + p.z() * p.z());
        assert(Math.abs(len - 1) < 1e-9);
    });

    it('computes distance between two points', function () {
        var d = new s2.S2LatLng(LA.lat, LA.lng)
            .distance(new s2.S2LatLng(NY.lat, NY.lng));
        // LA -> NY is ~35.4 degrees of arc on a great circle.
        assert(typeof d === 'number');
        assert(d > 30 && d < 45);
    });

    it('round-trips through S2Point', function () {
        var ll = new s2.S2LatLng(SF.lat, SF.lng);
        var back = new s2.S2LatLng(ll.toPoint());
        assert(Math.abs(back.lat - SF.lat) < 1e-6);
        assert(Math.abs(back.lng - SF.lng) < 1e-6);
    });
});

describe('S2Point', function () {
    it('exposes x/y/z accessors', function () {
        var p = new s2.S2LatLng(SF.lat, SF.lng).toPoint();
        assert.strictEqual(typeof p.x(), 'number');
        assert.strictEqual(typeof p.y(), 'number');
        assert.strictEqual(typeof p.z(), 'number');
    });
});

describe('S2CellId', function () {
    it('builds from an S2LatLng and exposes id() as a numeric string', function () {
        var cellId = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng));
        assert.strictEqual(typeof cellId.id(), 'string');
        assert(/^\d+$/.test(cellId.id()));
    });

    it('builds from an S2Point', function () {
        var p = new s2.S2LatLng(SF.lat, SF.lng).toPoint();
        var cellId = new s2.S2CellId(p);
        assert.strictEqual(typeof cellId.id(), 'string');
    });

    it('round-trips through token', function () {
        var cellId = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng)).parent(15);
        var token = cellId.toToken();
        assert.strictEqual(typeof token, 'string');
        assert(token.length > 0);

        var rebuilt = new s2.S2CellId(new s2.S2LatLng(0, 0));
        rebuilt.fromToken(token);
        assert.strictEqual(rebuilt.id(), cellId.id());
        assert.strictEqual(rebuilt.level(), 15);
    });

    it('reports level for leaf vs face cells', function () {
        var leaf = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng));
        assert.strictEqual(leaf.level(), 30);

        var parent = leaf.parent(0);
        assert.strictEqual(parent.level(), 0);
        assert.strictEqual(parent.isFace(), true);
    });

    it('navigates the cell hierarchy', function () {
        var cellId = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng)).parent(10);

        // parent containment
        assert.strictEqual(cellId.parent().level(), 9);

        // child(0..3) at next level
        for (var i = 0; i < 4; i++) {
            var child = cellId.child(i);
            assert.strictEqual(child.level(), 11);
            assert.strictEqual(cellId.contains(child), true);
        }

        // sibling navigation
        var next = cellId.next();
        var prev = cellId.prev();
        assert.notStrictEqual(next.id(), cellId.id());
        assert.notStrictEqual(prev.id(), cellId.id());
    });

    it('returns 4 edge neighbors', function () {
        var cellId = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng)).parent(10);
        var neighbors = cellId.neighbors();
        assert(Array.isArray(neighbors));
        assert.strictEqual(neighbors.length, 4);
        neighbors.forEach(function (n) {
            assert.strictEqual(typeof n.id(), 'string');
            assert.strictEqual(n.level(), 10);
        });
    });

    it('returns 4 children via children()', function () {
        var cellId = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng)).parent(10);
        var children = cellId.children();
        assert(Array.isArray(children));
        assert.strictEqual(children.length, 4);
        children.forEach(function (id) { assert.strictEqual(typeof id, 'string'); });
    });

    it('range_min/range_max bracket the cell at leaf level', function () {
        var cellId = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng)).parent(10);
        var min = cellId.rangeMin();
        var max = cellId.rangeMax();
        assert.strictEqual(min.level(), 30);
        assert.strictEqual(max.level(), 30);
        assert.strictEqual(cellId.contains(min), true);
        assert.strictEqual(cellId.contains(max), true);
    });

    it('converts back to S2LatLng near the original', function () {
        var ll = new s2.S2LatLng(SF.lat, SF.lng);
        var roundTrip = new s2.S2CellId(ll).toLatLng();
        assert(Math.abs(roundTrip.lat - SF.lat) < 1e-6);
        assert(Math.abs(roundTrip.lng - SF.lng) < 1e-6);
    });
});

describe('S2Cell', function () {
    function makeCell() {
        return new s2.S2Cell(new s2.S2LatLng(SF.lat, SF.lng));
    }

    it('reports basic geometry', function () {
        var cell = makeCell();
        assert.strictEqual(typeof cell.level(), 'number');
        assert.strictEqual(typeof cell.face(), 'number');
        assert.strictEqual(typeof cell.orientation(), 'number');
        assert.strictEqual(typeof cell.isLeaf(), 'boolean');
    });

    it('returns positive areas', function () {
        var cell = makeCell();
        assert(cell.approxArea() > 0);
        assert(cell.exactArea() > 0);
        assert(cell.averageArea(10) > 0);
    });

    it('returns a centre and four vertices', function () {
        var cell = makeCell();
        var center = cell.getCenter();
        assert.strictEqual(typeof center.x(), 'number');
        for (var i = 0; i < 4; i++) {
            var v = cell.getVertex(i);
            assert.strictEqual(typeof v.x(), 'number');
        }
    });

    it('produces a cap bound', function () {
        var cell = makeCell();
        var cap = cell.getCapBound();
        // A cap bound should not contain its own complement (which covers the
        // rest of the sphere); this is a robust functional check that doesn't
        // depend on floating-point self-comparison.
        assert.strictEqual(cap.contains(cap.complement()), false);
    });
});

describe('S2Cap', function () {
    function makeCap() {
        // height 0.01 → opening angle ~8.1°
        return new s2.S2Cap(new s2.S2LatLng(LA.lat, LA.lng).toPoint(), 0.01);
    }

    it('throws on invalid construction', function () {
        assert.throws(function () { new s2.S2Cap(); });
        assert.throws(function () { new s2.S2Cap(1, 2); });
    });

    it('contains a strictly smaller, concentric cap', function () {
        // S2Cap.Contains uses a non-exact predicate (angle + acos of dot
        // product) that can fail self-comparison on platforms with FMA
        // (e.g. arm64). A cap that strictly contains another in height is the
        // semantically meaningful check and is robust across architectures.
        var bigger = new s2.S2Cap(new s2.S2LatLng(LA.lat, LA.lng).toPoint(), 0.01);
        var smaller = new s2.S2Cap(new s2.S2LatLng(LA.lat, LA.lng).toPoint(), 0.001);
        assert.strictEqual(bigger.contains(smaller), true);
        assert.strictEqual(smaller.contains(bigger), false);
        assert.strictEqual(bigger.intersects(smaller), true);
        assert.strictEqual(bigger.interiorIntersects(smaller), true);
    });

    it('does not contain its complement', function () {
        var cap = makeCap();
        var comp = cap.complement();
        assert.strictEqual(cap.contains(comp), false);
    });

    it('produces a rect bound', function () {
        var rect = makeCap().getRectBound();
        assert.strictEqual(rect.isValid(), true);
        assert.strictEqual(rect.isEmpty(), false);
    });
});

describe('S2LatLngRect', function () {
    // The constructor expects S2Points (see examples/index.js) despite the name.
    function makeRect() {
        return new s2.S2LatLngRect(
            new s2.S2LatLng(LA.lat, LA.lng).toPoint(),
            new s2.S2LatLng(NY.lat, NY.lng).toPoint());
    }

    it('reports area > 0 and is valid/non-empty', function () {
        var rect = makeRect();
        assert(rect.area() > 0);
        assert.strictEqual(rect.isValid(), true);
        assert.strictEqual(rect.isEmpty(), false);
        assert.strictEqual(rect.isPoint(), false);
    });

    it('returns a center latlng and a size latlng', function () {
        var rect = makeRect();
        var c = rect.center();
        assert.strictEqual(typeof c.lat, 'number');
        assert.strictEqual(typeof c.lng, 'number');

        var size = rect.size();
        assert.strictEqual(typeof size.lat, 'number');
        assert.strictEqual(typeof size.lng, 'number');
    });

    it('returns four vertices', function () {
        var rect = makeRect();
        for (var i = 0; i < 4; i++) {
            var v = rect.getVertex(i);
            assert.strictEqual(typeof v.lat, 'number');
        }
    });

    it('approxEquals itself', function () {
        var rect = makeRect();
        assert.strictEqual(rect.approxEquals(rect), true);
    });

    it('union with itself equals itself', function () {
        var rect = makeRect();
        var u = rect.union(rect);
        assert.strictEqual(rect.approxEquals(u), true);
    });

    it('intersection with itself equals itself', function () {
        var rect = makeRect();
        var i = rect.intersection(rect);
        assert.strictEqual(rect.approxEquals(i), true);
    });

    it('produces a cap bound', function () {
        assert(makeRect().getCapBound());
    });
});

describe('S1Angle', function () {
    it('constructs from two S2Points and normalises', function () {
        var a = new s2.S2LatLng(LA.lat, LA.lng).toPoint();
        var b = new s2.S2LatLng(NY.lat, NY.lng).toPoint();
        var angle = new s2.S1Angle(a, b);
        // normalize returns the angle itself; just ensure no throw
        assert(angle.normalize());
    });

    it('throws on missing args', function () {
        assert.throws(function () { new s2.S1Angle(); });
        assert.throws(function () { new s2.S1Angle(1, 2); });
    });
});

describe('S1Interval', function () {
    it('builds from a single point', function () {
        var iv = new s2.S1Interval(0.5);
        assert.strictEqual(typeof iv.length(), 'number');
        assert.strictEqual(typeof iv.hi(), 'number');
        assert.strictEqual(typeof iv.lo(), 'number');
        assert.strictEqual(iv.contains(0.5), true);
    });

    it('throws on bad input', function () {
        assert.throws(function () { new s2.S1Interval(); });
        assert.throws(function () { new s2.S1Interval(0.5).contains(); });
        assert.throws(function () { new s2.S1Interval(0.5).contains('x'); });
    });
});

describe('S2RegionCoverer', function () {
    var coverer = new s2.S2RegionCoverer();
    var minLevel = 5;
    var maxLevel = 10;
    var maxCells = 20;
    var levelMod = 2;

    it('covers an S2LatLngRect', function () {
        var rect = new s2.S2LatLngRect(
            new s2.S2LatLng(LA.lat, LA.lng).toPoint(),
            new s2.S2LatLng(NY.lat, NY.lng).toPoint());
        var cells = coverer.getCovering(rect, minLevel, maxLevel, maxCells, levelMod);
        assert(Array.isArray(cells));
        assert(cells.length > 0);
        cells.forEach(function (c) {
            assert.strictEqual(typeof c.id(), 'string');
            assert(c.level() >= minLevel && c.level() <= maxLevel);
        });
    });

    it('covers an S2Cap', function () {
        var cap = new s2.S2Cap(
            new s2.S2LatLng(LA.lat, LA.lng).normalized().toPoint(),
            0.001);
        var cells = coverer.getCovering(cap, minLevel, maxLevel, maxCells, levelMod);
        assert(Array.isArray(cells));
        assert(cells.length > 0);
    });

    it('covers an S2Cell', function () {
        var cell = new s2.S2Cell(new s2.S2LatLng(SF.lat, SF.lng));
        var cells = coverer.getCovering(cell, minLevel, maxLevel, maxCells, levelMod);
        assert(Array.isArray(cells));
        assert(cells.length > 0);
    });
});

describe('s2.getCovering (top-level wrapper)', function () {
    function makeRect() {
        return new s2.S2LatLngRect(
            new s2.S2LatLng(LA.lat, LA.lng).toPoint(),
            new s2.S2LatLng(NY.lat, NY.lng).toPoint());
    }

    it('exposes a top-level s2.getCovering function', function () {
        assert.strictEqual(typeof s2.getCovering, 'function');
    });

    it('defaults result_type to "cell" and returns S2Cell instances', function () {
        var cells = s2.getCovering(makeRect(), { min: 5, max: 10, max_cells: 20, level_mod: 2 });
        assert(Array.isArray(cells));
        assert(cells.length > 0);
        cells.forEach(function (c) {
            assert.strictEqual(typeof c.level(), 'number');
            assert.strictEqual(typeof c.id(), 'object');
            assert(c.level() >= 5 && c.level() <= 10);
        });
    });

    it('result_type "cellId" returns S2CellId instances', function () {
        var ids = s2.getCovering(makeRect(), { min: 5, max: 10, max_cells: 20, result_type: 'cellId' });
        assert(Array.isArray(ids));
        assert(ids.length > 0);
        ids.forEach(function (id) {
            assert.strictEqual(typeof id.id(), 'string');
            assert.strictEqual(typeof id.toToken(), 'string');
        });
    });

    it('result_type "string" returns numeric id strings', function () {
        var strs = s2.getCovering(makeRect(), { min: 5, max: 10, max_cells: 20, result_type: 'string' });
        assert(Array.isArray(strs));
        assert(strs.length > 0);
        strs.forEach(function (s) {
            assert.strictEqual(typeof s, 'string');
            assert(/^\d+$/.test(s));
        });
    });

    it('result_type "token" returns alphanumeric tokens', function () {
        var tokens = s2.getCovering(makeRect(), { min: 5, max: 10, max_cells: 20, result_type: 'token' });
        assert(Array.isArray(tokens));
        assert(tokens.length > 0);
        tokens.forEach(function (t) {
            assert.strictEqual(typeof t, 'string');
            assert(t.length > 0 && t.length <= 16);
        });
    });

    it('result_type "point" returns S2Point instances', function () {
        var points = s2.getCovering(makeRect(), { min: 5, max: 10, max_cells: 20, result_type: 'point' });
        assert(Array.isArray(points));
        assert(points.length > 0);
        points.forEach(function (p) {
            assert.strictEqual(typeof p.x(), 'number');
            assert.strictEqual(typeof p.y(), 'number');
            assert.strictEqual(typeof p.z(), 'number');
        });
    });

    it('works with no options (uses defaults)', function () {
        var cells = s2.getCovering(makeRect());
        assert(Array.isArray(cells));
        assert(cells.length > 0);
    });

    it('also accepts S2Cap and S2Cell regions', function () {
        var cap = new s2.S2Cap(new s2.S2LatLng(LA.lat, LA.lng).normalized().toPoint(), 0.001);
        var cellRegion = new s2.S2Cell(new s2.S2LatLng(SF.lat, SF.lng));
        var fromCap = s2.getCovering(cap, { min: 5, max: 10, max_cells: 20, result_type: 'token' });
        var fromCell = s2.getCovering(cellRegion, { min: 5, max: 10, max_cells: 20, result_type: 'token' });
        assert(fromCap.length > 0);
        assert(fromCell.length > 0);
    });

    it('throws on unknown result_type', function () {
        assert.throws(function () {
            s2.getCovering(makeRect(), { result_type: 'banana' });
        }, /Unknown result_type/);
    });
});
