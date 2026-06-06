#ifndef NODES2INTERVAL_HPP
#define NODES2INTERVAL_HPP

#include <nan.h>
#include "s1interval.h"
#include "s2.h"
#include "s1angle.h"
#include "latlng.h"
#include "point.h"

namespace s2geo {

class Interval : public Nan::ObjectWrap {
public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    inline S1Interval get() { return this_; }
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S1Interval c);

protected:
    Interval();

    static NAN_METHOD(New);
    static NAN_METHOD(GetLength);
    static NAN_METHOD(GetCenter);
    static NAN_METHOD(GetComplementCenter);
    static NAN_METHOD(GetHi);
    static NAN_METHOD(GetLo);
    static NAN_METHOD(Contains);

    S1Interval this_;
};

}

#endif