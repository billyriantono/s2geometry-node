var NativeExtension = require('bindings')('NativeExtension');

// Top-level convenience wrapper matching the API.md signature for getCovering.
// Underneath it just uses S2RegionCoverer (the underlying C++ binding) and
// then transforms the result based on `options.result_type`.
NativeExtension.getCovering = function (region, options) {
    options = options || {};
    var min = options.min != null ? options.min : 1;
    var max = options.max != null ? options.max : 30;
    var maxCells = options.max_cells != null ? options.max_cells : null;
    var levelMod = options.level_mod != null ? options.level_mod : 1;
    var resultType = options.result_type || 'cell';

    var coverer = new NativeExtension.S2RegionCoverer();
    var cellIds = coverer.getCovering(region, min, max, maxCells, levelMod);

    switch (resultType) {
        case 'cellId':
            return cellIds;
        case 'cell':
            return cellIds.map(function (id) { return new NativeExtension.S2Cell(id); });
        case 'string':
            return cellIds.map(function (id) { return id.id(); });
        case 'token':
            return cellIds.map(function (id) { return id.toToken(); });
        case 'point':
            return cellIds.map(function (id) { return id.toPoint(); });
        default:
            throw new Error('Unknown result_type: ' + resultType +
                ' (expected one of cell, cellId, string, token, point)');
    }
};

module.exports = NativeExtension;
