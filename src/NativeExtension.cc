#include <nan.h>
#include "point.h"
#include "latlng.h"
#include "cellid.h"
#include "cell.h"
#include "latlngrect.h"
#include "cap.h"
#include "interval.h"
#include "angle.h"
#include "regioncoverer.h"
#include "cellunion.h"
#include "loop.h"
#include "polyline.h"
#include "polygon.h"

namespace s2geo {

NAN_MODULE_INIT(InitAll) {
    Point::Init(target);
    LatLng::Init(target);
    CellId::Init(target);
    Cap::Init(target);
    LatLngRect::Init(target);
    Cell::Init(target);
    Interval::Init(target);
    Angle::Init(target);
    RegionCoverer::Init(target);
    CellUnion::Init(target);
    Loop::Init(target);
    Polyline::Init(target);
    Polygon::Init(target);
}

NODE_MODULE(NativeExtension, InitAll)

}