#ifndef NODES2CAP_HPP
#define NODES2CAP_HPP

#include <nan.h>
#include "s2cap.h"
#include "s2.h"
#include "s2latlngrect.h"
#include "point.h"
#include "cap.h"
#include "latlngrect.h"

namespace s2geo {

class Cap : public Nan::ObjectWrap {
public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    inline S2Cap get() { return this_; }
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Cap ll);

protected:
    Cap();

    static NAN_METHOD(New);
    static NAN_METHOD(GetRectBound);
    static NAN_METHOD(Intersects);
    static NAN_METHOD(InteriorIntersects);
    static NAN_METHOD(Contains);
    static NAN_METHOD(Complement);

    S2Cap this_;
};
}
#endif
