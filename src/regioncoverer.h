#ifndef REGIONCOVERER_HPP
#define REGIONCOVERER_HPP

#include <nan.h>
#include "s2.h"
#include "s2regioncoverer.h"

namespace s2geo {

class RegionCoverer : public Nan::ObjectWrap {
public:
    RegionCoverer();
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info);

protected:
    ~RegionCoverer();

    static NAN_METHOD(New);
    static NAN_METHOD(GetCovering);

    /*
    static NAN_METHOD(MinLevel);
    static NAN_METHOD(SetLevelMod);
    static NAN_METHOD(LevelMod);
    static NAN_METHOD(GetSimpleCovering);
    static NAN_METHOD(GetInteriorCellUnion);
    static NAN_METHOD(GetCellUnion);
    static NAN_METHOD(GetInteriorCovering);
    */
};

}

#endif