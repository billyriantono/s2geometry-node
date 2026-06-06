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

    it('handles the dateline (lng = 180 / -180)', function () {
        var east = new s2.S2LatLng(0, 180);
        var west = new s2.S2LatLng(0, -180);
        assert.strictEqual(east.isValid(), true);
        assert.strictEqual(west.isValid(), true);
        // distance across the dateline at the equator is ~0 degrees
        assert(east.distance(west) < 0.01);
    });

    it('handles the poles (lat = +/-90)', function () {
        var north = new s2.S2LatLng(90, 0);
        var south = new s2.S2LatLng(-90, 0);
        assert.strictEqual(north.isValid(), true);
        assert.strictEqual(south.isValid(), true);
        // distance between the poles is 180 degrees
        assert(Math.abs(north.distance(south) - 180) < 1e-9);
    });
});

describe('S2Point', function () {
    it('exposes x/y/z accessors', function () {
        var p = new s2.S2LatLng(SF.lat, SF.lng).toPoint();
        assert.strictEqual(typeof p.x(), 'number');
        assert.strictEqual(typeof p.y(), 'number');
        assert.strictEqual(typeof p.z(), 'number');
    });

    it('can be built directly from raw x,y,z', function () {
        var p = new s2.S2Point(1, 0, 0);
        assert.strictEqual(p.x(), 1);
        assert.strictEqual(p.y(), 0);
        assert.strictEqual(p.z(), 0);
    });

    it('exposes toArray()', function () {
        var p = new s2.S2LatLng(SF.lat, SF.lng).toPoint();
        var arr = p.toArray();
        assert(Array.isArray(arr));
        assert.strictEqual(arr.length, 3);
        assert.strictEqual(arr[0], p.x());
        assert.strictEqual(arr[1], p.y());
        assert.strictEqual(arr[2], p.z());
    });

    it('exposes toString() in [x, y, z] form', function () {
        var p = new s2.S2Point(1, 0, 0);
        assert.strictEqual(p.toString(), '[1, 0, 0]');
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

    // Issue #15: expose internal S2 coordinate values for debugging ports.
    it('exposes face() and toFaceIJ() for coordinate debugging (#15)', function () {
        var cellId = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng)).parent(10);

        var face = cellId.face();
        assert.strictEqual(typeof face, 'number');
        assert(face >= 0 && face < 6, 'face should be 0..5');

        var ij = cellId.toFaceIJ();
        assert.strictEqual(ij.face, face, 'toFaceIJ().face should match face()');
        assert.strictEqual(typeof ij.i, 'number');
        assert.strictEqual(typeof ij.j, 'number');
        assert.strictEqual(typeof ij.orientation, 'number');
        // i and j are 30-bit integers at leaf level
        assert(ij.i >= 0 && ij.i < (1 << 30));
        assert(ij.j >= 0 && ij.j < (1 << 30));
        // orientation is a 2-bit value (Hilbert curve orientation)
        assert(ij.orientation >= 0 && ij.orientation < 4);
    });

    it('toFaceIJ on a face cell reports i=j=2^29 (cell center)', function () {
        var faceCell = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng)).parent(0);
        assert.strictEqual(faceCell.isFace(), true);
        var ij = faceCell.toFaceIJ();
        assert.strictEqual(ij.i, 1 << 29);
        assert.strictEqual(ij.j, 1 << 29);
    });

    it('idString matches id (both stringified decimal)', function () {
        var cellId = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng));
        assert.strictEqual(cellId.idString(), cellId.id());
    });

    it('child_begin and child_end bracket the children at the next level', function () {
        var parent = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng)).parent(10);
        var begin = parent.child_begin();
        var end = parent.child_end();
        assert.strictEqual(begin.level(), 11);
        assert.strictEqual(end.level(), 11);
        assert.notStrictEqual(begin.id(), end.id());
    });

    it('parent at the current level returns self', function () {
        var cellId = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng)).parent(10);
        var sameLevelParent = cellId.parent(10);
        assert.strictEqual(sameLevelParent.id(), cellId.id());
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

    it('can be constructed from an S2CellId', function () {
        var id = new s2.S2CellId(new s2.S2LatLng(SF.lat, SF.lng)).parent(10);
        var cell = new s2.S2Cell(id);
        assert.strictEqual(cell.level(), 10);
        assert.strictEqual(cell.face(), id.face());
    });

    it('exposes id() as an S2CellId', function () {
        var cell = new s2.S2Cell(new s2.S2LatLng(SF.lat, SF.lng));
        var id = cell.id();
        assert.strictEqual(typeof id.toToken, 'function');
        assert.strictEqual(typeof id.level, 'function');
        assert.strictEqual(id.level(), cell.level());
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
    // The constructor accepts either S2LatLng or S2Point endpoints; we use
    // .toPoint() here to also exercise the Point->LatLng auto-conversion path.
    function makeRect() {
        return new s2.S2LatLngRect(
            new s2.S2LatLng(LA.lat, LA.lng).toPoint(),
            new s2.S2LatLng(NY.lat, NY.lng).toPoint());
    }

    // Regression test for issue #22: prior to the latlngrect.cc rewrite, the
    // documented constructor `new S2LatLngRect(latlng, latlng)` didn't work
    // (the inverted HasInstance check threw on actual LatLng inputs).
    it('accepts two S2LatLng endpoints (#22 regression)', function () {
        var p1 = new s2.S2LatLng(44.0378862, 10.0458712);
        var p2 = new s2.S2LatLng(45, 11);
        var rect = new s2.S2LatLngRect(p1, p2);
        assert.strictEqual(rect.isValid(), true);
        assert.strictEqual(rect.isEmpty(), false);
        assert.strictEqual(rect.contains(p1), true);
        assert.strictEqual(rect.contains(p2), true);
        assert.strictEqual(rect.contains(new s2.S2LatLng(0, 0)), false);
    });

    // Regression test for issue #23: prior to the latlngrect.cc rewrite, an
    // inverted HasInstance check accepted non-LatLng inputs and unwrapped them
    // as LatLng (UB), producing rects whose covering landed somewhere in
    // Africa instead of over North America. Verify a US-spanning rect lands
    // where it should regardless of whether endpoints are LatLng or Point.
    it('LA->NY rect centers over North America (#23 regression)', function () {
        var fromLatLng = new s2.S2LatLngRect(
            new s2.S2LatLng(LA.lat, LA.lng),
            new s2.S2LatLng(NY.lat, NY.lng));
        var fromPoint = new s2.S2LatLngRect(
            new s2.S2LatLng(LA.lat, LA.lng).toPoint(),
            new s2.S2LatLng(NY.lat, NY.lng).toPoint());
        [fromLatLng, fromPoint].forEach(function (rect) {
            var c = rect.center();
            assert(c.lat > 30 && c.lat < 45, 'lat should be ~37, got ' + c.lat);
            assert(c.lng > -120 && c.lng < -70, 'lng should be ~-96, got ' + c.lng);
        });
    });

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

describe('S2CellUnion', function () {
    var coverer = new s2.S2RegionCoverer();

    // A covering of a small cap around SF, used as input cell ids.
    function covering() {
        var cap = new s2.S2Cap(
            new s2.S2LatLng(SF.lat, SF.lng).normalized().toPoint(), 0.0005);
        return coverer.getCovering(cap, 5, 12, 20, 1);
    }

    it('is constructed from an array of S2CellId and normalizes', function () {
        var ids = covering();
        var union = new s2.S2CellUnion(ids);
        assert(union.numCells() > 0);
        // Normalization never produces more cells than the raw input.
        assert(union.numCells() <= ids.length);
    });

    it('exposes cellId(i) and cellIds()', function () {
        var union = new s2.S2CellUnion(covering());
        var all = union.cellIds();
        assert(Array.isArray(all));
        assert.strictEqual(all.length, union.numCells());
        assert.strictEqual(union.cellId(0).id(), all[0].id());
    });

    it('throws on out-of-range cellId index', function () {
        var union = new s2.S2CellUnion(covering());
        assert.throws(function () { union.cellId(union.numCells()); });
    });

    it('contains its own member cells', function () {
        var union = new s2.S2CellUnion(covering());
        assert.strictEqual(union.contains(union.cellId(0)), true);
    });

    it('reports containment of a point inside the covered region', function () {
        var union = new s2.S2CellUnion(covering());
        var p = new s2.S2LatLng(SF.lat, SF.lng).toPoint();
        assert.strictEqual(union.contains(p), true);
    });

    it('intersects an overlapping union and supports set operations', function () {
        var a = new s2.S2CellUnion(covering());
        var b = new s2.S2CellUnion(covering());
        assert.strictEqual(a.intersects(b), true);

        var u = a.getUnion(b);
        assert(u instanceof s2.S2CellUnion);
        assert(u.numCells() > 0);

        var inter = a.getIntersection(b);
        assert(inter.numCells() > 0);

        var diff = a.getDifference(b);
        // Identical inputs -> empty difference.
        assert.strictEqual(diff.numCells(), 0);
    });

    it('reports bounds and areas', function () {
        var union = new s2.S2CellUnion(covering());
        assert(union.getCapBound() instanceof s2.S2Cap);
        assert(union.getRectBound() instanceof s2.S2LatLngRect);
        assert(union.leafCellsCovered() > 0);
        assert(union.approxArea() > 0);
        assert(union.exactArea() > 0);
        assert(union.averageBasedArea() > 0);
    });

    it('has a readable toString()', function () {
        var union = new s2.S2CellUnion(covering());
        assert(/^S2CellUnion\[/.test(union.toString()));
    });
});

describe('S2Loop', function () {
    // A small CCW square (~1 degree) near the equator, as unit-length points.
    function squarePoints() {
        return [
            new s2.S2LatLng(0, 0),
            new s2.S2LatLng(0, 1),
            new s2.S2LatLng(1, 1),
            new s2.S2LatLng(1, 0)
        ].map(function (ll) { return ll.toPoint(); });
    }

    function square() {
        return new s2.S2Loop(squarePoints());
    }

    it('constructs from an array of S2Point and is valid', function () {
        var loop = square();
        assert.strictEqual(loop.numVertices(), 4);
        assert.strictEqual(loop.isValid(), true);
    });

    it('constructs from an S2Cell', function () {
        var cell = new s2.S2Cell(new s2.S2LatLng(SF.lat, SF.lng).normalized());
        var loop = new s2.S2Loop(cell);
        assert.strictEqual(loop.numVertices(), 4);
        assert.strictEqual(loop.isValid(), true);
    });

    it('exposes vertices as S2Point instances', function () {
        var loop = square();
        var v = loop.vertex(0);
        assert.strictEqual(typeof v.x, 'function');
        var len = Math.sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
        assert(Math.abs(len - 1) < 1e-9);
    });

    it('throws on out-of-range vertex index', function () {
        var loop = square();
        assert.throws(function () { loop.vertex(loop.numVertices()); });
    });

    it('reports a positive area no greater than 4*Pi', function () {
        var loop = square();
        loop.normalize();
        var area = loop.getArea();
        assert(area > 0 && area <= 4 * Math.PI);
    });

    it('produces a centroid and turning angle', function () {
        var loop = square();
        assert(loop.getCentroid() instanceof s2.S2Point);
        assert.strictEqual(typeof loop.getTurningAngle(), 'number');
    });

    it('contains an interior point but not an exterior one', function () {
        var loop = square();
        loop.normalize();
        var inside = new s2.S2LatLng(0.5, 0.5).toPoint();
        var outside = new s2.S2LatLng(20, 20).toPoint();
        assert.strictEqual(loop.contains(inside), true);
        assert.strictEqual(loop.contains(outside), false);
    });

    it('supports loop-vs-loop containment and intersection', function () {
        var outer = new s2.S2Loop([
            new s2.S2LatLng(0, 0),
            new s2.S2LatLng(0, 4),
            new s2.S2LatLng(4, 4),
            new s2.S2LatLng(4, 0)
        ].map(function (ll) { return ll.toPoint(); }));
        var inner = new s2.S2Loop([
            new s2.S2LatLng(1, 1),
            new s2.S2LatLng(1, 3),
            new s2.S2LatLng(3, 3),
            new s2.S2LatLng(3, 1)
        ].map(function (ll) { return ll.toPoint(); }));
        outer.normalize();
        inner.normalize();
        assert.strictEqual(outer.contains(inner), true);
        assert.strictEqual(outer.intersects(inner), true);
        assert.strictEqual(outer.containsNested(inner), true);
    });

    it('compares boundaries for equality', function () {
        var a = square();
        var b = square();
        assert.strictEqual(a.boundaryEquals(b), true);
        assert.strictEqual(a.boundaryApproxEquals(b), true);
    });

    it('reports bounds and hole/sign metadata', function () {
        var loop = square();
        assert(loop.getCapBound() instanceof s2.S2Cap);
        assert(loop.getRectBound() instanceof s2.S2LatLngRect);
        assert.strictEqual(loop.depth(), 0);
        assert.strictEqual(loop.isHole(), false);
        assert.strictEqual(loop.sign(), 1);
    });

    it('has a readable toString()', function () {
        assert(/^S2Loop\[/.test(square().toString()));
    });
});

describe('S2Polyline', function () {
    // A simple 3-vertex path along the equator.
    function pathLatLngs() {
        return [
            new s2.S2LatLng(0, 0),
            new s2.S2LatLng(0, 1),
            new s2.S2LatLng(0, 2)
        ];
    }

    function pathPoints() {
        return pathLatLngs().map(function (ll) { return ll.toPoint(); });
    }

    function path() {
        return new s2.S2Polyline(pathPoints());
    }

    it('constructs from an array of S2Point', function () {
        var line = path();
        assert.strictEqual(line.numVertices(), 3);
    });

    it('constructs from an array of S2LatLng', function () {
        var line = new s2.S2Polyline(pathLatLngs());
        assert.strictEqual(line.numVertices(), 3);
    });

    it('exposes vertices as S2Point instances', function () {
        var v = path().vertex(1);
        assert(v instanceof s2.S2Point);
    });

    it('throws on out-of-range vertex index', function () {
        var line = path();
        assert.throws(function () { line.vertex(line.numVertices()); });
    });

    it('reports a length in radians matching the spanned arc', function () {
        var len = path().getLength();
        // Two 1-degree segments along the equator => ~2 degrees of arc.
        assert(Math.abs(len - 2 * Math.PI / 180) < 1e-6);
    });

    it('interpolates a unit-length midpoint', function () {
        var p = path().interpolate(0.5);
        var len = Math.sqrt(p.x() * p.x() + p.y() * p.y() + p.z() * p.z());
        assert(Math.abs(len - 1) < 1e-9);
    });

    it('round-trips interpolate/unInterpolate', function () {
        var line = path();
        var suffix = line.getSuffix(0.5);
        assert(suffix.point instanceof s2.S2Point);
        assert(suffix.nextVertex >= 1 && suffix.nextVertex <= line.numVertices());
        var f = line.unInterpolate(suffix.point, suffix.nextVertex);
        assert(Math.abs(f - 0.5) < 1e-6);
    });

    it('projects a nearby point onto the polyline', function () {
        var line = path();
        var off = new s2.S2LatLng(0.1, 1).toPoint();
        var proj = line.project(off);
        assert(proj.point instanceof s2.S2Point);
        assert(typeof proj.nextVertex === 'number');
    });

    it('reports right-hand-sideness', function () {
        var line = path();
        assert.strictEqual(typeof line.isOnRight(new s2.S2LatLng(-1, 1).toPoint()), 'boolean');
    });

    it('detects intersection with a crossing polyline', function () {
        var line = path();
        var crossing = new s2.S2Polyline([
            new s2.S2LatLng(-1, 1),
            new s2.S2LatLng(1, 1)
        ].map(function (ll) { return ll.toPoint(); }));
        assert.strictEqual(line.intersects(crossing), true);
    });

    it('reverses vertex order', function () {
        var line = path();
        var first = line.vertex(0).toArray();
        line.reverse();
        var last = line.vertex(line.numVertices() - 1).toArray();
        assert.deepStrictEqual(first, last);
    });

    it('approxEquals itself', function () {
        assert.strictEqual(path().approxEquals(path()), true);
    });

    it('reports bounds and may-intersect', function () {
        var line = path();
        assert(line.getCapBound() instanceof s2.S2Cap);
        assert(line.getRectBound() instanceof s2.S2LatLngRect);
        var cell = new s2.S2Cell(new s2.S2LatLng(0, 1));
        assert.strictEqual(typeof line.mayIntersect(cell), 'boolean');
    });

    it('has a readable toString()', function () {
        assert(/^S2Polyline\[/.test(path().toString()));
    });
});

describe('S2Polygon', function () {
    // A normalized CCW loop covering the given lat/lng square corners.
    function loopFrom(corners) {
        var loop = new s2.S2Loop(corners.map(function (c) {
            return new s2.S2LatLng(c[0], c[1]).toPoint();
        }));
        loop.normalize();
        return loop;
    }

    function squareLoop() {
        return loopFrom([[0, 0], [0, 2], [2, 2], [2, 0]]);
    }

    function squarePolygon() {
        return new s2.S2Polygon([squareLoop()]);
    }

    it('constructs from an array of S2Loop', function () {
        var poly = squarePolygon();
        assert.strictEqual(poly.numLoops(), 1);
        assert.strictEqual(poly.numVertices(), 4);
        assert.strictEqual(poly.isValid(), true);
    });

    it('constructs from an S2Cell', function () {
        var cell = new s2.S2Cell(new s2.S2LatLng(SF.lat, SF.lng).normalized());
        var poly = new s2.S2Polygon(cell);
        assert.strictEqual(poly.numLoops(), 1);
        assert.strictEqual(poly.numVertices(), 4);
    });

    it('exposes loop(k) as an S2Loop copy', function () {
        var poly = squarePolygon();
        var loop = poly.loop(0);
        assert(loop instanceof s2.S2Loop);
        assert.strictEqual(loop.numVertices(), 4);
    });

    it('throws on out-of-range loop index', function () {
        var poly = squarePolygon();
        assert.throws(function () { poly.loop(poly.numLoops()); });
    });

    it('reports a positive area and a centroid', function () {
        var poly = squarePolygon();
        assert(poly.getArea() > 0 && poly.getArea() <= 4 * Math.PI);
        assert(poly.getCentroid() instanceof s2.S2Point);
    });

    it('contains an interior point but not an exterior one', function () {
        var poly = squarePolygon();
        assert.strictEqual(poly.contains(new s2.S2LatLng(1, 1).toPoint()), true);
        assert.strictEqual(poly.contains(new s2.S2LatLng(20, 20).toPoint()), false);
    });

    it('supports polygon-vs-polygon containment and intersection', function () {
        var big = squarePolygon();
        var small = new s2.S2Polygon([loopFrom([[0.5, 0.5], [0.5, 1.5], [1.5, 1.5], [1.5, 0.5]])]);
        assert.strictEqual(big.contains(small), true);
        assert.strictEqual(big.intersects(small), true);
    });

    it('computes union, intersection, and difference', function () {
        var a = squarePolygon();
        var b = new s2.S2Polygon([loopFrom([[1, 1], [1, 3], [3, 3], [3, 1]])]);

        var union = a.getUnion(b);
        assert(union instanceof s2.S2Polygon);
        assert(union.getArea() > a.getArea());

        var inter = a.getIntersection(b);
        assert(inter.getArea() > 0 && inter.getArea() < a.getArea());

        var diff = a.getDifference(b);
        assert(diff.getArea() > 0 && diff.getArea() < a.getArea());

        // Disjoint polygons => empty intersection.
        var far = new s2.S2Polygon([loopFrom([[40, 40], [40, 42], [42, 42], [42, 40]])]);
        assert.strictEqual(a.getIntersection(far).getArea(), 0);
    });

    it('reports validity, normalization, and boundary equality', function () {
        var a = squarePolygon();
        var b = squarePolygon();
        assert.strictEqual(a.isNormalized(), true);
        assert.strictEqual(a.boundaryEquals(b), true);
        assert.strictEqual(a.boundaryApproxEquals(b), true);
    });

    it('projects an exterior point onto the boundary', function () {
        var poly = squarePolygon();
        var outside = new s2.S2LatLng(1, 5).toPoint();
        var proj = poly.project(outside);
        assert(proj instanceof s2.S2Point);
    });

    it('builds from a cell union border', function () {
        var coverer = new s2.S2RegionCoverer();
        var cap = new s2.S2Cap(new s2.S2LatLng(SF.lat, SF.lng).normalized().toPoint(), 0.0005);
        var union = new s2.S2CellUnion(coverer.getCovering(cap, 5, 12, 20, 1));
        var poly = new s2.S2Polygon([]);
        poly.initToCellUnionBorder(union);
        assert(poly.numLoops() > 0);
        assert(poly.getArea() > 0);
    });

    it('reports bounds, parent, and last descendant', function () {
        var poly = squarePolygon();
        assert(poly.getCapBound() instanceof s2.S2Cap);
        assert(poly.getRectBound() instanceof s2.S2LatLngRect);
        assert.strictEqual(poly.getParent(0), -1);
        assert.strictEqual(poly.getLastDescendant(0), 0);
        assert.strictEqual(poly.getLastDescendant(-1), 0);
    });

    it('validates parent/descendant loop indices', function () {
        var poly = squarePolygon();
        assert.throws(function () { poly.getParent(-1); }, /loop index out of range/);
        assert.throws(function () { poly.getParent(1); }, /loop index out of range/);
        assert.throws(function () { poly.getLastDescendant(-2); }, /loop index out of range/);
        assert.throws(function () { poly.getLastDescendant(1); }, /loop index out of range/);
    });

    it('has a readable toString()', function () {
        assert(/^S2Polygon\[/.test(squarePolygon().toString()));
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
