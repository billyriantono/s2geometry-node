#ifndef NODES2LATLNGRECT_HPP
#define NODES2LATLNGRECT_HPP

#include <nan.h>
#include "s2latlngrect.h"
#include "s2cell.h"
#include "s2.h"
#include "latlng.h"
#include "cell.h"
#include "cap.h"
#include "cellid.h"
namespace s2geo{

class LatLngRect : public Nan::ObjectWrap {
public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    inline S2LatLngRect get() { return this_; }
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2LatLngRect ll);

protected:
    LatLngRect();

    static NAN_METHOD(New);
    static NAN_METHOD(GetCenter);
    static NAN_METHOD(GetSize);
    static NAN_METHOD(GetArea);
    static NAN_METHOD(GetVertex);
    static NAN_METHOD(GetCapBound);
    static NAN_METHOD(Contains);
    static NAN_METHOD(IsValid);
    static NAN_METHOD(IsEmpty);
    static NAN_METHOD(IsPoint);
    static NAN_METHOD(Union);
    static NAN_METHOD(Intersection);
    static NAN_METHOD(ApproxEquals);

    S2LatLngRect this_;
};
}
#endif
