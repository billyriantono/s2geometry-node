#include "angle.h"

namespace s2geo{

Nan::Persistent<v8::FunctionTemplate> Angle::constructor;

NAN_MODULE_INIT(Angle::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S1Angle").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    Nan::SetPrototypeMethod(tpl, "normalize", Normalize);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S1Angle").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

Angle::Angle() : this_() {}

NAN_METHOD(Angle::New) {
    if (info.IsConstructCall()) {
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = info[0].As<v8::External>();
            void* ptr = ext->Value();
            Angle* ll = static_cast<Angle*>(ptr);
            ll->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        }

        if (info.Length() != 2) {
            Nan::ThrowTypeError("(point,point) required.");
            return;
        }

        Angle* obj = new Angle();
        obj->Wrap(info.This());

        v8::Local<v8::Object> a = Nan::To<v8::Object>(info[0]).ToLocalChecked();
        v8::Local<v8::Object> b = Nan::To<v8::Object>(info[1]).ToLocalChecked();

        v8::Local<v8::FunctionTemplate> point = Nan::New(Point::constructor);
        if (!point->HasInstance(a) || !point->HasInstance(b)) {
            Nan::ThrowTypeError("(point,point) required.");
            return;
        }

        obj->this_ = S1Angle(
            Nan::ObjectWrap::Unwrap<Point>(a)->get(),
            Nan::ObjectWrap::Unwrap<Point>(b)->get());

        info.GetReturnValue().Set(info.This());
    } else {
        const int argc = 2;
        v8::Local<v8::Value> argv[argc] = { info[0], info[1] };
        v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
        info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }
}

v8::Local<v8::Object> Angle::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S1Angle s1angle) {
    Nan::EscapableHandleScope scope;

    Angle* obj = new Angle();
    obj->this_ = s1angle;

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(Angle::Normalize) {
    Angle* obj = Nan::ObjectWrap::Unwrap<Angle>(info.Holder());
    obj->this_.Normalize();
    info.GetReturnValue().Set(info.This());
}
}
