#include "point.h"

namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> Point::constructor;

NAN_MODULE_INIT(Point::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S2Point").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype methods
    Nan::SetPrototypeMethod(tpl, "x", X);
    Nan::SetPrototypeMethod(tpl, "y", Y);
    Nan::SetPrototypeMethod(tpl, "z", Z);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S2Point").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

Point::Point() : this_() {}

NAN_METHOD(Point::New) {
    if (info.IsConstructCall()) {
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = info[0].As<v8::External>();
            void* ptr = ext->Value();
            Point* p = static_cast<Point*>(ptr);
            p->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        }

        if (info.Length() != 3) {
            Nan::ThrowTypeError("(number, number, number) required");
            return;
        }

        Point* obj = new Point();
        obj->Wrap(info.This());

        obj->this_ = S2Point(
            Nan::To<double>(info[0]).FromJust(),
            Nan::To<double>(info[1]).FromJust(),
            Nan::To<double>(info[2]).FromJust()
        );

        info.GetReturnValue().Set(info.This());
    } else {
        const int argc = 3;
        v8::Local<v8::Value> argv[argc] = { info[0], info[1], info[2] };
        v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
        info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }
}

v8::Local<v8::Object> Point::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Point point) {
    Nan::EscapableHandleScope scope;

    Point* obj = new Point();
    obj->this_ = point;

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(Point::X) {
    Point* obj = Nan::ObjectWrap::Unwrap<Point>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.x()));
}

NAN_METHOD(Point::Y) {
    Point* obj = Nan::ObjectWrap::Unwrap<Point>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.y()));
}

NAN_METHOD(Point::Z) {
    Point* obj = Nan::ObjectWrap::Unwrap<Point>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.z()));
}

}