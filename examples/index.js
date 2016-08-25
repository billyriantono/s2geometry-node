var s2 = require('../');

var origin = new s2.S2CellId(new s2.S2LatLng(44.0378862, 10.0458712));

console.log(origin.parent(15).id());
console.log(origin.parent(15).child_end().id());

var latlng = new s2.S2LatLng(44.0378862, 10.0458712);
console.log(latlng.normalized().lat);
console.log(latlng.normalized().lng);

var origin = new s2.S2CellId(new s2.S2LatLng(37.7879938 ,-122.4074374));
console.dir(origin.parent(15).children());

var los_angeles = {lat: 34.0522, long: 118.2437}
var new_york = {lat: 40.7128, long: 74.0059}
var latLngLA = new s2.S2LatLng(los_angeles.lat, los_angeles.long);
var latLngNY = new s2.S2LatLng(new_york.lat, new_york.long);

var latLngRect = new s2.S2LatLngRect(latLngLA.toPoint(), latLngNY.toPoint());

var region = new s2.S2RegionCoverer();
var minLevel = 5;
var maxLevel = 10;
var maxCells = 20;
var levelMod = 2;
var results = region.getCovering(latLngRect, minLevel, maxLevel, maxCells, levelMod);
for(var i = 0; i < results.length; i++)
{
	console.log(results[i].id())
}
