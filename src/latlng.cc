#include "latlng.h"
#include "point.h"
#include <vector>

namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> LatLng::constructor;

NAN_MODULE_INIT(LatLng::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S2LatLng").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    Nan::SetPrototypeMethod(tpl, "normalized", Normalized);
    Nan::SetPrototypeMethod(tpl, "isValid", IsValid);
    Nan::SetPrototypeMethod(tpl, "toPoint", ToPoint);
    Nan::SetPrototypeMethod(tpl, "distance", Distance);
    Nan::SetPrototypeMethod(tpl, "toString", ToString);

    v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();
    Nan::SetAccessor(proto, Nan::New("lat").ToLocalChecked(), Lat);
    Nan::SetAccessor(proto, Nan::New("lng").ToLocalChecked(), Lng);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S2LatLng").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

LatLng::LatLng() : this_() {}
LatLng::~LatLng() {
}

NAN_METHOD(LatLng::New) {
    if (info.IsConstructCall()) {
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = info[0].As<v8::External>();
            void* ptr = ext->Value();
            LatLng* ll = static_cast<LatLng*>(ptr);
            ll->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        }

        LatLng* obj = new LatLng();
        obj->Wrap(info.This());

        if (info.Length() == 2) {
            if (info[0]->IsNumber() && info[1]->IsNumber()) {
                obj->this_ = S2LatLng::FromDegrees(
                    Nan::To<double>(info[0]).FromJust(),
                    Nan::To<double>(info[1]).FromJust());
            }
        } else if (info.Length() == 1) {
            v8::Local<v8::Object> fromObj = Nan::To<v8::Object>(info[0]).ToLocalChecked();
            v8::Local<v8::FunctionTemplate> point = Nan::New(Point::constructor);
            if (point->HasInstance(fromObj)) {
                S2Point p = Nan::ObjectWrap::Unwrap<Point>(fromObj)->get();
                obj->this_ = S2LatLng(p);
            } else {
                Nan::ThrowTypeError("Use the new operator to create instances of this object.");
                return;
            }
        }

        info.GetReturnValue().Set(info.This());
    } else {
        const int argc = info.Length();
        std::vector<v8::Local<v8::Value>> argv(argc);
        for (int i = 0; i < argc; i++) {
            argv[i] = info[i];
        }
        v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
        info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv.data()).ToLocalChecked());
    }
}

v8::Local<v8::Object> LatLng::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2LatLng ll) {
    Nan::EscapableHandleScope scope;

    LatLng* obj = new LatLng();
    obj->this_ = ll;

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_GETTER(LatLng::Lat) {
    LatLng* obj = Nan::ObjectWrap::Unwrap<LatLng>(info.This());
    info.GetReturnValue().Set(Nan::New(obj->this_.lat().degrees()));
}

NAN_GETTER(LatLng::Lng) {
    LatLng* obj = Nan::ObjectWrap::Unwrap<LatLng>(info.This());
    info.GetReturnValue().Set(Nan::New(obj->this_.lng().degrees()));
}

NAN_METHOD(LatLng::IsValid) {
    LatLng* obj = Nan::ObjectWrap::Unwrap<LatLng>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.is_valid()));
}

NAN_METHOD(LatLng::Normalized) {
    LatLng* obj = Nan::ObjectWrap::Unwrap<LatLng>(info.Holder());
    info.GetReturnValue().Set(LatLng::CreateNew(info, obj->this_.Normalized()));
}

NAN_METHOD(LatLng::ToPoint) {
    LatLng* obj = Nan::ObjectWrap::Unwrap<LatLng>(info.Holder());
    info.GetReturnValue().Set(Point::CreateNew(info, obj->this_.ToPoint()));
}

NAN_METHOD(LatLng::Distance) {
    LatLng* latlng = Nan::ObjectWrap::Unwrap<LatLng>(info.Holder());
    S2LatLng other = Nan::ObjectWrap::Unwrap<LatLng>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    info.GetReturnValue().Set(Nan::New(latlng->this_.GetDistance(other).degrees()));
}

NAN_METHOD(LatLng::ToString) {
    LatLng* obj = Nan::ObjectWrap::Unwrap<LatLng>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.ToStringInDegrees().c_str()).ToLocalChecked());
}
}
