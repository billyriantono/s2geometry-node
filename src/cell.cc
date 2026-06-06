#include "cell.h"

namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> Cell::constructor;

NAN_MODULE_INIT(Cell::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S2Cell").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "approxArea", ApproxArea);
    Nan::SetPrototypeMethod(tpl, "exactArea", ExactArea);
    Nan::SetPrototypeMethod(tpl, "averageArea", AverageArea);
    Nan::SetPrototypeMethod(tpl, "face", Face);
    Nan::SetPrototypeMethod(tpl, "level", Level);
    Nan::SetPrototypeMethod(tpl, "orientation", Orientation);
    Nan::SetPrototypeMethod(tpl, "isLeaf", IsLeaf);
    Nan::SetPrototypeMethod(tpl, "getCapBound", GetCapBound);
    Nan::SetPrototypeMethod(tpl, "getVertex", GetVertex);
    Nan::SetPrototypeMethod(tpl, "getCenter", GetCenter);
    Nan::SetPrototypeMethod(tpl, "id", Id);
    Nan::SetPrototypeMethod(tpl, "toString", ToString);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S2Cell").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

Cell::Cell() : this_() {}

NAN_METHOD(Cell::New) {
    if (info.IsConstructCall()) {
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = info[0].As<v8::External>();
            void* ptr = ext->Value();
            Cell* ll = static_cast<Cell*>(ptr);
            ll->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        }

        if (info.Length() != 1) {
            Nan::ThrowTypeError("(lat,lng) required.");
            return;
        }

        Cell* obj = new Cell();
        obj->Wrap(info.This());

        v8::Local<v8::Object> ll = Nan::To<v8::Object>(info[0]).ToLocalChecked();
        v8::Local<v8::FunctionTemplate> latlng = Nan::New(LatLng::constructor);
        v8::Local<v8::FunctionTemplate> cellId = Nan::New(CellId::constructor);
        if (latlng->HasInstance(ll)) {
            obj->this_ = S2Cell(
                S2CellId::FromLatLng(Nan::ObjectWrap::Unwrap<LatLng>(ll)->get()));
        } else if (cellId->HasInstance(ll)) {
            obj->this_ = S2Cell(Nan::ObjectWrap::Unwrap<CellId>(ll)->get());
        } else {
            Nan::ThrowTypeError("(lat,lng) or (cellid) required.");
            return;
        }

        info.GetReturnValue().Set(info.This());
    } else {
        const int argc = 1;
        v8::Local<v8::Value> argv[argc] = { info[0] };
        v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
        info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }
}

v8::Local<v8::Object> Cell::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Cell s2cell) {
    Nan::EscapableHandleScope scope;

    Cell* obj = new Cell();
    obj->this_ = s2cell;

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

v8::Local<v8::Object> Cell::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2CellId s2cellid) {
    Nan::EscapableHandleScope scope;

    Cell* obj = new Cell();
    obj->this_ = S2Cell(s2cellid);

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(Cell::ApproxArea) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.ApproxArea()));
}

NAN_METHOD(Cell::ExactArea) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.ExactArea()));
}

NAN_METHOD(Cell::AverageArea) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.AverageArea(Nan::To<double>(info[0]).FromJust())));
}
NAN_METHOD(Cell::Face) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.face()));
}

NAN_METHOD(Cell::Level) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.level()));
}

NAN_METHOD(Cell::Orientation) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.orientation()));
}

NAN_METHOD(Cell::IsLeaf) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.is_leaf()));
}

NAN_METHOD(Cell::GetCapBound) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(Cap::CreateNew(info, obj->this_.GetCapBound()));
}

NAN_METHOD(Cell::GetCenter) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(Point::CreateNew(info, obj->this_.GetCenterRaw()));
}

NAN_METHOD(Cell::Id) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(CellId::CreateNew(info, obj->this_.id()));
}

NAN_METHOD(Cell::ToString) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.id().ToString().c_str()).ToLocalChecked());
}

NAN_METHOD(Cell::GetVertex) {
    Cell* obj = Nan::ObjectWrap::Unwrap<Cell>(info.Holder());
    info.GetReturnValue().Set(Point::CreateNew(info, obj->this_.GetVertex(Nan::To<double>(info[0]).FromJust())));
}
}