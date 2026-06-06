// TypeScript definitions for s2geometry-node 2.0.0
// Bindings for the S2 Geometry Library.

declare module 's2geometry-node' {
    /**
     * A point on the unit sphere as an (x, y, z) vector.
     */
    export class S2Point {
        constructor(x: number, y: number, z: number);
        x(): number;
        y(): number;
        z(): number;
        toArray(): [number, number, number];
        toString(): string;
    }

    /**
     * A latitude/longitude pair, stored internally as radians.
     */
    export class S2LatLng {
        constructor(lat: number, lng: number);
        constructor(point: S2Point);
        readonly lat: number;
        readonly lng: number;
        isValid(): boolean;
        normalized(): S2LatLng;
        toPoint(): S2Point;
        distance(other: S2LatLng): number;
        toString(): string;
    }

    /**
     * A 64-bit identifier for a cell in the S2 cell decomposition.
     */
    export class S2CellId {
        constructor(input?: S2LatLng | S2Point | string);
        level(): number;
        id(): string;
        idString(): string;
        toToken(): string;
        fromToken(token: string): this;
        toPoint(): S2Point;
        toLatLng(): S2LatLng;
        toString(): string;
        parent(level?: number): S2CellId;
        prev(): S2CellId;
        next(): S2CellId;
        child(index: number): S2CellId;
        children(): string[];
        child_begin(): S2CellId;
        child_end(): S2CellId;
        neighbors(): [S2CellId, S2CellId, S2CellId, S2CellId];
        isFace(): boolean;
        rangeMin(): S2CellId;
        rangeMax(): S2CellId;
        contains(other: S2CellId): boolean;
        /** Face number 0..5 of the S2 cube containing this cell. */
        face(): number;
        /** Raw (face, i, j, orientation) tuple underlying this cell id. */
        toFaceIJ(): { face: number; i: number; j: number; orientation: number };
    }

    /**
     * A cell on the S2 sphere supporting efficient containment and area queries.
     */
    export class S2Cell {
        constructor(input: S2LatLng | S2CellId);
        approxArea(): number;
        exactArea(): number;
        averageArea(level: number): number;
        face(): number;
        level(): number;
        orientation(): number;
        isLeaf(): boolean;
        getCapBound(): S2Cap;
        getCenter(): S2Point;
        getVertex(index: number): S2Point;
        id(): S2CellId;
        toString(): string;
    }

    /**
     * A spherical cap, defined by an axis (S2Point) and a height.
     */
    export class S2Cap {
        constructor(axis: S2Point, height: number);
        getRectBound(): S2LatLngRect;
        intersects(other: S2Cap): boolean;
        interiorIntersects(other: S2Cap): boolean;
        contains(other: S2Cap): boolean;
        complement(): S2Cap;
    }

    /**
     * A closed latitude/longitude rectangle. Endpoints may be supplied as
     * either S2LatLng (the documented form) or S2Point (legacy examples).
     */
    export class S2LatLngRect {
        constructor(a: S2LatLng | S2Point, b?: S2LatLng | S2Point);
        center(): S2LatLng;
        size(): S2LatLng;
        area(): number;
        getVertex(index: number): S2LatLng;
        getCapBound(): S2Cap;
        contains(ll: S2LatLng): boolean;
        isValid(): boolean;
        isEmpty(): boolean;
        isPoint(): boolean;
        union(other: S2LatLngRect): S2LatLngRect;
        intersection(other: S2LatLngRect): S2LatLngRect;
        approxEquals(other: S2LatLngRect): boolean;
    }

    /**
     * An angle between two points, with conversion helpers.
     */
    export class S1Angle {
        constructor(a: S2Point, b: S2Point);
        normalize(): this;
    }

    /**
     * A closed interval on the unit circle.
     */
    export class S1Interval {
        constructor(point: number);
        length(): number;
        hi(): number;
        lo(): number;
        center(): number;
        complementLength(): number;
        contains(value: number): boolean;
    }

    /**
     * Builds a covering (a set of S2CellIds) for a region. Use this class
     * directly for positional-argument control, or call the top-level
     * `getCovering` for the options-object form.
     */
    export class S2RegionCoverer {
        constructor();
        getCovering(
            region: S2LatLngRect | S2Cap | S2Cell,
            minLevel?: number,
            maxLevel?: number,
            maxCells?: number | null,
            levelMod?: number
        ): S2CellId[];
    }

    /**
     * A region consisting of cells of various sizes, typically used to
     * approximate another shape. Construct one from an array of S2CellId
     * (the input is normalized) or obtain one from set operations.
     */
    export class S2CellUnion {
        constructor(cellIds?: S2CellId[]);
        /** Number of cells in the union. */
        numCells(): number;
        /** The i-th cell id (0 <= i < numCells()). */
        cellId(i: number): S2CellId;
        /** All cell ids in the union as an array. */
        cellIds(): S2CellId[];
        /** Containment with respect to regions (a cell contains its children). */
        contains(other: S2CellId | S2Cell | S2Point | S2CellUnion): boolean;
        intersects(other: S2CellId | S2CellUnion): boolean;
        /** Union of this and another cell union. */
        getUnion(other: S2CellUnion): S2CellUnion;
        /** Intersection of this and another cell union. */
        getIntersection(other: S2CellUnion): S2CellUnion;
        /** Difference (this - other) of two cell unions. */
        getDifference(other: S2CellUnion): S2CellUnion;
        /** Normalize in place; returns true if the number of cells was reduced. */
        normalize(): boolean;
        /** Expand the union by a rim of cells at the given level. Returns this. */
        expand(level: number): this;
        getCapBound(): S2Cap;
        getRectBound(): S2LatLngRect;
        /** Number of leaf cells covered by the union. */
        leafCellsCovered(): number;
        averageBasedArea(): number;
        approxArea(): number;
        exactArea(): number;
        toString(): string;
    }

    /**
     * A simple spherical polygon: a single chain of vertices where the last
     * vertex is implicitly connected to the first. Loops are CCW-oriented
     * (interior on the left of each edge) and must have at least 3 vertices.
     */
    export class S2Loop {
        constructor(vertices: S2Point[]);
        constructor(cell: S2Cell);
        numVertices(): number;
        vertex(i: number): S2Point;
        isValid(): boolean;
        isNormalized(): boolean;
        /** Invert if needed so the enclosed area is at most 2*Pi. Returns this. */
        normalize(): this;
        /** Reverse vertex order, complementing the region. Returns this. */
        invert(): this;
        /** Area of the loop interior, between 0 and 4*Pi. */
        getArea(): number;
        /** True centroid scaled by area (not unit length). */
        getCentroid(): S2Point;
        /** Sum of turning angles: positive if CCW, negative if CW. */
        getTurningAngle(): number;
        contains(other: S2Loop | S2Cell | S2Point): boolean;
        intersects(other: S2Loop): boolean;
        containsNested(other: S2Loop): boolean;
        boundaryEquals(other: S2Loop): boolean;
        boundaryApproxEquals(other: S2Loop, maxError?: number): boolean;
        mayIntersect(cell: S2Cell): boolean;
        getCapBound(): S2Cap;
        getRectBound(): S2LatLngRect;
        /** Nesting level within a containing polygon (0 = outer shell). */
        depth(): number;
        isHole(): boolean;
        /** -1 if this loop is a hole, +1 otherwise. */
        sign(): number;
        toString(): string;
    }

    /** Result of S2Polyline#getSuffix / #project: a point plus the index of
     *  the next polyline vertex after it. */
    export interface PolylinePointResult {
        point: S2Point;
        nextVertex: number;
    }

    /**
     * A sequence of vertices connected by geodesic edges. Adjacent vertices
     * must not be identical or antipodal, and all vertices must be unit length.
     */
    export class S2Polyline {
        constructor(vertices: S2Point[]);
        constructor(vertices: S2LatLng[]);
        numVertices(): number;
        vertex(k: number): S2Point;
        /** Total length of the polyline, in radians of arc. */
        getLength(): number;
        /** Centroid scaled by length (not unit length). */
        getCentroid(): S2Point;
        /** Unit-length point at the given fraction (0..1, clamped) of the length. */
        interpolate(fraction: number): S2Point;
        /** Like interpolate(), also returning the next vertex index. */
        getSuffix(fraction: number): PolylinePointResult;
        /** Inverse of interpolate: fraction of length at the given point. */
        unInterpolate(point: S2Point, nextVertex: number): number;
        /** Closest point on the polyline to the query point, plus next vertex. */
        project(point: S2Point): PolylinePointResult;
        /** Whether the point is on the right-hand side (needs >= 2 vertices). */
        isOnRight(point: S2Point): boolean;
        intersects(other: S2Polyline): boolean;
        /** Reverse the vertex order in place. Returns this. */
        reverse(): this;
        approxEquals(other: S2Polyline, maxError?: number): boolean;
        getCapBound(): S2Cap;
        getRectBound(): S2LatLngRect;
        mayIntersect(cell: S2Cell): boolean;
        toString(): string;
    }

    /**
     * A polygon consisting of zero or more CCW-oriented loops representing
     * shells and holes. A point is inside the polygon if it is contained by an
     * odd number of loops. Construct from an array of S2Loop (loops are cloned)
     * or from a single S2Cell, or derive one via set operations.
     */
    export class S2Polygon {
        constructor(loops: S2Loop[]);
        constructor(cell: S2Cell);
        numLoops(): number;
        /** Total number of vertices across all loops. */
        numVertices(): number;
        /** A copy of the k-th loop. */
        loop(k: number): S2Loop;
        /** Area of the polygon interior, between 0 and 4*Pi. */
        getArea(): number;
        /** True centroid scaled by area (not unit length). */
        getCentroid(): S2Point;
        contains(other: S2Polygon | S2Cell | S2Point): boolean;
        /** Containment allowing B's vertices to move up to the merge radius (radians). */
        approxContains(other: S2Polygon, vertexMergeRadiusRadians: number): boolean;
        intersects(other: S2Polygon): boolean;
        /** Union of this and another polygon. */
        getUnion(other: S2Polygon): S2Polygon;
        /** Intersection of this and another polygon. */
        getIntersection(other: S2Polygon): S2Polygon;
        /** Difference (this - other) of two polygons. */
        getDifference(other: S2Polygon): S2Polygon;
        isValid(): boolean;
        isNormalized(): boolean;
        boundaryEquals(other: S2Polygon): boolean;
        boundaryApproxEquals(other: S2Polygon, maxError?: number): boolean;
        /** Closest boundary point if outside, else the point itself (non-empty only). */
        project(point: S2Point): S2Point;
        /** Reset this polygon to the outline of the given cell union. Returns this. */
        initToCellUnionBorder(cells: S2CellUnion): this;
        /** Index of the parent of loop k, or -1. */
        getParent(k: number): number;
        /** Index of the last loop contained within loop k. */
        getLastDescendant(k: number): number;
        getCapBound(): S2Cap;
        getRectBound(): S2LatLngRect;
        mayIntersect(cell: S2Cell): boolean;
        toString(): string;
    }

    /** Region types that `getCovering` accepts. */
    export type S2Region = S2LatLngRect | S2Cap | S2Cell;

    /** Options for the top-level `getCovering` helper. */
    export interface GetCoveringOptions {
        /** Minimum cell level (default 1). */
        min?: number;
        /** Maximum cell level (default 30). */
        max?: number;
        /** Approximate maximum number of cells (default no limit). */
        max_cells?: number | null;
        /** Modulo for skipping levels: 1 = 4x, 2 = 16x, 3 = 64x (default 1). */
        level_mod?: number;
        /**
         * How to encode each cell in the result. Defaults to `'cell'`.
         * - `cell`   -> S2Cell[]
         * - `cellId` -> S2CellId[]
         * - `string` -> string[] (numeric id strings)
         * - `token`  -> string[] (compact tokens)
         * - `point`  -> S2Point[] (cell centers)
         */
        result_type?: 'cell' | 'cellId' | 'string' | 'token' | 'point';
    }

    /**
     * Top-level convenience that covers a region using an options object.
     * Underneath, it constructs an S2RegionCoverer and dispatches based on
     * `options.result_type`.
     */
    export function getCovering(region: S2Region, options?: GetCoveringOptions & { result_type?: 'cell' }): S2Cell[];
    export function getCovering(region: S2Region, options: GetCoveringOptions & { result_type: 'cellId' }): S2CellId[];
    export function getCovering(region: S2Region, options: GetCoveringOptions & { result_type: 'string' }): string[];
    export function getCovering(region: S2Region, options: GetCoveringOptions & { result_type: 'token' }): string[];
    export function getCovering(region: S2Region, options: GetCoveringOptions & { result_type: 'point' }): S2Point[];
}
