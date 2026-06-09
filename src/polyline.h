#ifndef POLYLINE_H
#define POLYLINE_H

#include <nan.h>
#include "s2.h"
#include "s1angle.h"
#include "s2polyline.h"
#include "s2latlng.h"
#include "point.h"
#include "latlng.h"
#include "cell.h"
#include "cap.h"
#include "latlngrect.h"

namespace s2geo {

class Polyline : public Nan::ObjectWrap {
public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    // Returns a borrowed pointer to the underlying polyline (owned by wrapper).
    inline S2Polyline* get() { return this_; }
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Polyline* line);

protected:
    Polyline();
    ~Polyline();

    static NAN_METHOD(New);
    static NAN_METHOD(NumVertices);
    static NAN_METHOD(Vertex);
    static NAN_METHOD(GetLength);
    static NAN_METHOD(GetCentroid);
    static NAN_METHOD(Interpolate);
    static NAN_METHOD(GetSuffix);
    static NAN_METHOD(UnInterpolate);
    static NAN_METHOD(Project);
    static NAN_METHOD(IsOnRight);
    static NAN_METHOD(Intersects);
    static NAN_METHOD(Reverse);
    static NAN_METHOD(ApproxEquals);
    static NAN_METHOD(GetCapBound);
    static NAN_METHOD(GetRectBound);
    static NAN_METHOD(MayIntersect);
    static NAN_METHOD(ToString);

    S2Polyline* this_;
};

}
#endif
