#ifndef NODES2ANGLE_HPP
#define NODES2ANGLE_HPP

#include <nan.h>
#include "s2.h"
#include "s1angle.h"
#include "latlng.h"
#include "point.h"

namespace s2geo{

class Angle : public Nan::ObjectWrap {
public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    inline S1Angle get() { return this_; }
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S1Angle c);

protected:
    Angle();

    static NAN_METHOD(New);
    static NAN_METHOD(Normalize);

    S1Angle this_;
};
}
#endif
