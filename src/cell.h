#ifndef CELL_HPP
#define CELL_HPP

#include <nan.h>
#include "s2cell.h"
#include "s2cellid.h"
#include "s2.h"
#include "s2latlng.h"
#include "latlng.h"
#include "point.h"
#include "cap.h"
#include "cellid.h"
namespace s2geo{
class Cell : public Nan::ObjectWrap {
public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    inline S2Cell get() { return this_; }
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Cell c);
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2CellId c);

protected:
    Cell();

    static NAN_METHOD(New);
    static NAN_METHOD(ApproxArea);
    static NAN_METHOD(ExactArea);
    static NAN_METHOD(Face);
    static NAN_METHOD(Level);
    static NAN_METHOD(AverageArea);
    static NAN_METHOD(Orientation);
    static NAN_METHOD(IsLeaf);
    static NAN_METHOD(GetCapBound);
    static NAN_METHOD(GetCenter);
    static NAN_METHOD(GetVertex);
    static NAN_METHOD(ToString);
    static NAN_METHOD(Id);

    S2Cell this_;
};
}
#endif
