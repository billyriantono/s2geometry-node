#ifndef POINT_HPP
#define POINT_HPP

#include <nan.h>
#include "s2.h"

namespace s2geo {

class Point : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init);
    inline S2Point get() { return this_; }
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Point c);

    static Nan::Persistent<v8::FunctionTemplate> constructor;

protected:
    Point();

    static NAN_METHOD(New);
    static NAN_METHOD(X);
    static NAN_METHOD(Y);
    static NAN_METHOD(Z);
    static NAN_METHOD(ToArray);
    static NAN_METHOD(ToString);

    S2Point this_;
};

}

#endif