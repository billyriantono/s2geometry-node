#include "interval.h"

namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> Interval::constructor;

NAN_MODULE_INIT(Interval::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S1Interval").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype methods
    Nan::SetPrototypeMethod(tpl, "length", GetLength);
    Nan::SetPrototypeMethod(tpl, "hi", GetHi);
    Nan::SetPrototypeMethod(tpl, "lo", GetLo);
    Nan::SetPrototypeMethod(tpl, "center", GetCenter);
    Nan::SetPrototypeMethod(tpl, "complementLength", GetComplementCenter);
    Nan::SetPrototypeMethod(tpl, "contains", Contains);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S1Interval").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

Interval::Interval() : this_() {}

NAN_METHOD(Interval::New) {
    if (info.IsConstructCall()) {
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = info[0].As<v8::External>();
            void* ptr = ext->Value();
            Interval* ll = static_cast<Interval*>(ptr);
            ll->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        }

        if (info.Length() != 1) {
            Nan::ThrowTypeError("(number) required.");
            return;
        }

        Interval* obj = new Interval();
        obj->Wrap(info.This());
        obj->this_ = S1Interval::FromPoint(Nan::To<double>(info[0]).FromJust());
        info.GetReturnValue().Set(info.This());
    } else {
        const int argc = 1;
        v8::Local<v8::Value> argv[argc] = { info[0] };
        v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
        info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }
}

v8::Local<v8::Object> Interval::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S1Interval s1interval) {
    Nan::EscapableHandleScope scope;

    Interval* obj = new Interval();
    obj->this_ = s1interval;

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(Interval::GetLength) {
    Interval* obj = Nan::ObjectWrap::Unwrap<Interval>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.GetLength()));
}

NAN_METHOD(Interval::GetCenter) {
    Interval* obj = Nan::ObjectWrap::Unwrap<Interval>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.GetCenter()));
}

NAN_METHOD(Interval::GetComplementCenter) {
    Interval* obj = Nan::ObjectWrap::Unwrap<Interval>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.GetComplementCenter()));
}

NAN_METHOD(Interval::GetHi) {
    Interval* obj = Nan::ObjectWrap::Unwrap<Interval>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.hi()));
}

NAN_METHOD(Interval::GetLo) {
    Interval* obj = Nan::ObjectWrap::Unwrap<Interval>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.lo()));
}

NAN_METHOD(Interval::Contains) {
    Interval* obj = Nan::ObjectWrap::Unwrap<Interval>(info.Holder());

    if (info.Length() != 1) {
        Nan::ThrowTypeError("(number) required.");
        return;
    }

    if (!info[0]->IsNumber()) {
        Nan::ThrowTypeError("(number) required.");
        return;
    }

    info.GetReturnValue().Set(Nan::New(obj->this_.Contains(Nan::To<double>(info[0]).FromJust())));
}

}