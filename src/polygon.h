#ifndef POLYGON_H
#define POLYGON_H

#include <nan.h>
#include "s2.h"
#include "s1angle.h"
#include "s2polygon.h"
#include "s2loop.h"
#include "s2cell.h"
#include "s2cellunion.h"
#include "point.h"
#include "loop.h"
#include "cell.h"
#include "cap.h"
#include "cellunion.h"
#include "latlngrect.h"

namespace s2geo {

class Polygon : public Nan::ObjectWrap {
public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    // Returns a borrowed pointer to the underlying polygon (owned by wrapper).
    inline S2Polygon* get() { return this_; }
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Polygon* poly);

protected:
    Polygon();
    ~Polygon();

    static NAN_METHOD(New);
    static NAN_METHOD(NumLoops);
    static NAN_METHOD(NumVertices);
    static NAN_METHOD(GetLoop);
    static NAN_METHOD(GetArea);
    static NAN_METHOD(GetCentroid);
    static NAN_METHOD(Contains);
    static NAN_METHOD(ApproxContains);
    static NAN_METHOD(Intersects);
    static NAN_METHOD(GetUnion);
    static NAN_METHOD(GetIntersection);
    static NAN_METHOD(GetDifference);
    static NAN_METHOD(IsValid);
    static NAN_METHOD(IsNormalized);
    static NAN_METHOD(BoundaryEquals);
    static NAN_METHOD(BoundaryApproxEquals);
    static NAN_METHOD(Project);
    static NAN_METHOD(InitToCellUnionBorder);
    static NAN_METHOD(GetParent);
    static NAN_METHOD(GetLastDescendant);
    static NAN_METHOD(GetCapBound);
    static NAN_METHOD(GetRectBound);
    static NAN_METHOD(MayIntersect);
    static NAN_METHOD(ToString);

    S2Polygon* this_;
};

}
#endif
