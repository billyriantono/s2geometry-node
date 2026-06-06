#include "latlngrect.h"
#include "point.h"

namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> LatLngRect::constructor;

NAN_MODULE_INIT(LatLngRect::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S2LatLngRect").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    Nan::SetPrototypeMethod(tpl, "center", GetCenter);
    Nan::SetPrototypeMethod(tpl, "size", GetSize);
    Nan::SetPrototypeMethod(tpl, "area", GetArea);
    Nan::SetPrototypeMethod(tpl, "getVertex", GetVertex);
    Nan::SetPrototypeMethod(tpl, "getCapBound", GetCapBound);
    Nan::SetPrototypeMethod(tpl, "contains", Contains);
    Nan::SetPrototypeMethod(tpl, "isValid", IsValid);
    Nan::SetPrototypeMethod(tpl, "isEmpty", IsEmpty);
    Nan::SetPrototypeMethod(tpl, "isPoint", IsPoint);
    Nan::SetPrototypeMethod(tpl, "union", Union);
    Nan::SetPrototypeMethod(tpl, "intersection", Intersection);
    Nan::SetPrototypeMethod(tpl, "approxEquals", ApproxEquals);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S2LatLngRect").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

LatLngRect::LatLngRect() : this_() {}

// Accept either an S2LatLng or an S2Point as an endpoint. S2Point is converted
// to S2LatLng to keep the API consistent while preserving the longstanding
// examples that pass `latlng.toPoint()`.
static bool ExtractLatLng(v8::Local<v8::Object> obj, S2LatLng* out) {
    v8::Local<v8::FunctionTemplate> latlngTpl = Nan::New(LatLng::constructor);
    v8::Local<v8::FunctionTemplate> pointTpl = Nan::New(Point::constructor);
    if (latlngTpl->HasInstance(obj)) {
        *out = Nan::ObjectWrap::Unwrap<LatLng>(obj)->get();
        return true;
    }
    if (pointTpl->HasInstance(obj)) {
        *out = S2LatLng(Nan::ObjectWrap::Unwrap<Point>(obj)->get());
        return true;
    }
    return false;
}

NAN_METHOD(LatLngRect::New) {
    if (!info.IsConstructCall()) {
        Nan::ThrowTypeError("Use the new operator to create instances of this object.");
        return;
    }

    if (info[0]->IsExternal()) {
        v8::Local<v8::External> ext = info[0].As<v8::External>();
        void* ptr = ext->Value();
        LatLngRect* ll = static_cast<LatLngRect*>(ptr);
        ll->Wrap(info.This());
        info.GetReturnValue().Set(info.This());
        return;
    }

    if (info.Length() == 0) {
        Nan::ThrowTypeError("(latlng) or (latlng, latlng) required");
        return;
    }

    LatLngRect* obj = new LatLngRect();
    obj->Wrap(info.This());

    v8::Local<v8::Object> firstObj = Nan::To<v8::Object>(info[0]).ToLocalChecked();
    S2LatLng a;
    if (!ExtractLatLng(firstObj, &a)) {
        Nan::ThrowTypeError("(latlng) required");
        return;
    }

    if (info.Length() == 1) {
        obj->this_ = S2LatLngRect::FromPoint(a);
    } else {
        v8::Local<v8::Object> secondObj = Nan::To<v8::Object>(info[1]).ToLocalChecked();
        S2LatLng b;
        if (!ExtractLatLng(secondObj, &b)) {
            Nan::ThrowTypeError("(latlng) required");
            return;
        }
        obj->this_ = S2LatLngRect::FromPointPair(a, b);
    }

    info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Object> LatLngRect::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2LatLngRect rect) {
    Nan::EscapableHandleScope scope;

    LatLngRect* obj = new LatLngRect();
    obj->this_ = rect;

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(LatLngRect::GetCenter) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    info.GetReturnValue().Set(LatLng::CreateNew(info, rect->this_.GetCenter()));
}

NAN_METHOD(LatLngRect::GetCapBound) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    info.GetReturnValue().Set(Cap::CreateNew(info, rect->this_.GetCapBound()));
}

NAN_METHOD(LatLngRect::GetSize) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    info.GetReturnValue().Set(LatLng::CreateNew(info, rect->this_.GetSize()));
}

NAN_METHOD(LatLngRect::GetArea) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    info.GetReturnValue().Set(Nan::New(rect->this_.Area()));
}

NAN_METHOD(LatLngRect::GetVertex) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    int k = Nan::To<int32_t>(info[0]).FromJust();
    info.GetReturnValue().Set(LatLng::CreateNew(info, rect->this_.GetVertex(k)));
}

NAN_METHOD(LatLngRect::Contains) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    v8::Local<v8::Object> arg = Nan::To<v8::Object>(info[0]).ToLocalChecked();
    S2LatLng other;
    if (!ExtractLatLng(arg, &other)) {
        Nan::ThrowTypeError("(latlng) required");
        return;
    }
    info.GetReturnValue().Set(Nan::New(rect->this_.Contains(other)));
}

NAN_METHOD(LatLngRect::IsValid) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    info.GetReturnValue().Set(Nan::New(rect->this_.is_valid()));
}

NAN_METHOD(LatLngRect::IsEmpty) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    info.GetReturnValue().Set(Nan::New(rect->this_.is_empty()));
}

NAN_METHOD(LatLngRect::IsPoint) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    info.GetReturnValue().Set(Nan::New(rect->this_.is_point()));
}

NAN_METHOD(LatLngRect::Union) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    S2LatLngRect other = Nan::ObjectWrap::Unwrap<LatLngRect>(
        Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    info.GetReturnValue().Set(LatLngRect::CreateNew(info, rect->this_.Union(other)));
}

NAN_METHOD(LatLngRect::Intersection) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    S2LatLngRect other = Nan::ObjectWrap::Unwrap<LatLngRect>(
        Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    info.GetReturnValue().Set(LatLngRect::CreateNew(info, rect->this_.Intersection(other)));
}

NAN_METHOD(LatLngRect::ApproxEquals) {
    LatLngRect* rect = Nan::ObjectWrap::Unwrap<LatLngRect>(info.Holder());
    S2LatLngRect other = Nan::ObjectWrap::Unwrap<LatLngRect>(
        Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    info.GetReturnValue().Set(Nan::New(rect->this_.ApproxEquals(other)));
}

}
