#ifndef LATLNG_HPP
#define LATLNG_HPP

#include "s2latlng.h"
#include <nan.h>

namespace s2geo {

class LatLng : public Nan::ObjectWrap {
 public:
    LatLng();
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    inline S2LatLng get() { return this_; }
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2LatLng ll);

 private:
  ~LatLng();

  static NAN_METHOD(New);
  //method
  static NAN_METHOD(IsValid);
  static NAN_METHOD(Normalized);
  static NAN_METHOD(ToPoint);
  static NAN_METHOD(ToString);
  static NAN_METHOD(Distance);
  //getter method
  static NAN_GETTER(Lat);
  static NAN_GETTER(Lng);

  S2LatLng this_;
};
}
#endif

