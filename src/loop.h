#ifndef LOOP_H
#define LOOP_H

#include <nan.h>
#include "s2.h"
#include "s2loop.h"
#include "s2cell.h"
#include "s2latlng.h"
#include "point.h"
#include "cell.h"
#include "cap.h"
#include "latlngrect.h"

namespace s2geo {

class Loop : public Nan::ObjectWrap {
public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    // Returns a borrowed pointer to the underlying loop (owned by this wrapper).
    inline S2Loop* get() { return this_; }
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Loop* loop);

protected:
    Loop();
    ~Loop();

    static NAN_METHOD(New);
    static NAN_METHOD(NumVertices);
    static NAN_METHOD(Vertex);
    static NAN_METHOD(IsValid);
    static NAN_METHOD(IsNormalized);
    static NAN_METHOD(Normalize);
    static NAN_METHOD(Invert);
    static NAN_METHOD(GetArea);
    static NAN_METHOD(GetCentroid);
    static NAN_METHOD(GetTurningAngle);
    static NAN_METHOD(Contains);
    static NAN_METHOD(Intersects);
    static NAN_METHOD(ContainsNested);
    static NAN_METHOD(BoundaryEquals);
    static NAN_METHOD(BoundaryApproxEquals);
    static NAN_METHOD(MayIntersect);
    static NAN_METHOD(GetCapBound);
    static NAN_METHOD(GetRectBound);
    static NAN_METHOD(Depth);
    static NAN_METHOD(IsHole);
    static NAN_METHOD(Sign);
    static NAN_METHOD(ToString);

    S2Loop* this_;
};

}
#endif
