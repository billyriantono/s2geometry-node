// myobject.h
#ifndef CELLID_H
#define CELLID_H

#include <nan.h>
#include "s2.h"
#include "s2cell.h"
#include "s2cellid.h"
#include "s2latlng.h"
#include "latlng.h"
#include "point.h"
#include "strutil.h"

namespace s2geo{
class CellId : public Nan::ObjectWrap {

public:
    CellId();
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2CellId c);
    inline S2CellId get() { return this_; }

protected:
    ~CellId();

    static NAN_METHOD(New);
    static NAN_METHOD(Level);
    static NAN_METHOD(ToToken);
    static NAN_METHOD(ToPoint);
    static NAN_METHOD(ToLatLng);
    static NAN_METHOD(ToString);
    static NAN_METHOD(FromToken);
    static NAN_METHOD(Parent);
    static NAN_METHOD(Prev);
    static NAN_METHOD(Next);
    static NAN_METHOD(ChildBegin);
    static NAN_METHOD(ChildEnd);
    static NAN_METHOD(Children);
    static NAN_METHOD(Neighbors);
    static NAN_METHOD(IsFace);
    static NAN_METHOD(RangeMin);
    static NAN_METHOD(RangeMax);
    static NAN_METHOD(Id);
    static NAN_METHOD(IdString);
    static NAN_METHOD(Child);
    static NAN_METHOD(Contains);
    static NAN_METHOD(Face);
    static NAN_METHOD(ToFaceIJ);

    S2CellId this_;
};
}
#endif

