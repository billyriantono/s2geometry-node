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
