#include "cap.h"

namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> Cap::constructor;

NAN_MODULE_INIT(Cap::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S2Cap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "getRectBound", GetRectBound);
    Nan::SetPrototypeMethod(tpl, "intersects", Intersects);
    Nan::SetPrototypeMethod(tpl, "interiorIntersects", InteriorIntersects);
    Nan::SetPrototypeMethod(tpl, "contains", Contains);
    Nan::SetPrototypeMethod(tpl, "complement", Complement);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S2Cap").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

Cap::Cap() : this_() {}

NAN_METHOD(Cap::New) {
    if (info.IsConstructCall()) {
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = info[0].As<v8::External>();
            void* ptr = ext->Value();
            Cap* ll = static_cast<Cap*>(ptr);
            ll->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        }

        Cap* obj = new Cap();
        obj->Wrap(info.This());

        if (info.Length() == 2 && info[1]->IsNumber()) {
            v8::Local<v8::Object> fromObj = Nan::To<v8::Object>(info[0]).ToLocalChecked();
            v8::Local<v8::FunctionTemplate> point = Nan::New(Point::constructor);
            if (point->HasInstance(fromObj)) {
                S2Point p = Nan::ObjectWrap::Unwrap<Point>(fromObj)->get();
                obj->this_ = S2Cap::FromAxisHeight(p, Nan::To<double>(info[1]).FromJust());
            } else {
                Nan::ThrowTypeError("S2Cap requires arguments (S2Point, number)");
                return;
            }
        } else {
            Nan::ThrowTypeError("S2Cap requires arguments (S2Point, number)");
            return;
        }

        info.GetReturnValue().Set(info.This());
    } else {
        const int argc = 2;
        v8::Local<v8::Value> argv[argc] = { info[0], info[1] };
        v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
        info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }
}

v8::Local<v8::Object> Cap::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Cap s2cap) {
    Nan::EscapableHandleScope scope;

    Cap* obj = new Cap();
    obj->this_ = s2cap;

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(Cap::GetRectBound) {
    Cap* cap = Nan::ObjectWrap::Unwrap<Cap>(info.Holder());
    info.GetReturnValue().Set(LatLngRect::CreateNew(info, cap->this_.GetRectBound()));
}

NAN_METHOD(Cap::Intersects) {
    Cap* cap = Nan::ObjectWrap::Unwrap<Cap>(info.Holder());
    S2Cap other = Nan::ObjectWrap::Unwrap<Cap>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    info.GetReturnValue().Set(Nan::New(cap->this_.Intersects(other)));
}

NAN_METHOD(Cap::InteriorIntersects) {
    Cap* cap = Nan::ObjectWrap::Unwrap<Cap>(info.Holder());
    S2Cap other = Nan::ObjectWrap::Unwrap<Cap>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    info.GetReturnValue().Set(Nan::New(cap->this_.Intersects(other)));
}

NAN_METHOD(Cap::Contains) {
    Cap* cap = Nan::ObjectWrap::Unwrap<Cap>(info.Holder());
    S2Cap other = Nan::ObjectWrap::Unwrap<Cap>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    info.GetReturnValue().Set(Nan::New(cap->this_.Contains(other)));
}

NAN_METHOD(Cap::Complement) {
    Cap* cap = Nan::ObjectWrap::Unwrap<Cap>(info.Holder());
    info.GetReturnValue().Set(Cap::CreateNew(info, cap->this_.Complement()));
}
}
