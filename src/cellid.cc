#include "cellid.h"
#include <vector>


namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> CellId::constructor;

CellId::CellId() : this_() {}

CellId::~CellId() {
}

NAN_MODULE_INIT(CellId::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("S2CellID").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "level", Level);
  Nan::SetPrototypeMethod(tpl, "toToken", ToToken);
  Nan::SetPrototypeMethod(tpl, "fromToken", FromToken);
  Nan::SetPrototypeMethod(tpl, "toPoint", ToPoint);
  Nan::SetPrototypeMethod(tpl, "toString", ToString);
  Nan::SetPrototypeMethod(tpl, "toLatLng", ToLatLng);
  Nan::SetPrototypeMethod(tpl, "parent", Parent);
  Nan::SetPrototypeMethod(tpl, "prev", Prev);
  Nan::SetPrototypeMethod(tpl, "next", Next);
  Nan::SetPrototypeMethod(tpl, "child_begin", ChildBegin);
  Nan::SetPrototypeMethod(tpl, "child_end", ChildEnd);
  Nan::SetPrototypeMethod(tpl, "neighbors", Neighbors);
  Nan::SetPrototypeMethod(tpl, "isFace", IsFace);
  Nan::SetPrototypeMethod(tpl, "rangeMin", RangeMin);
  Nan::SetPrototypeMethod(tpl, "rangeMax", RangeMax);
  Nan::SetPrototypeMethod(tpl, "id", Id);
  Nan::SetPrototypeMethod(tpl, "idString", IdString);
  Nan::SetPrototypeMethod(tpl, "child", Child);
  Nan::SetPrototypeMethod(tpl, "children", Children);
  Nan::SetPrototypeMethod(tpl, "contains", Contains);
  Nan::SetPrototypeMethod(tpl, "face", Face);
  Nan::SetPrototypeMethod(tpl, "toFaceIJ", ToFaceIJ);

  constructor.Reset(tpl);

  Nan::Set(target,
           Nan::New("S2CellId").ToLocalChecked(),
           Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(CellId::New) {
    if (info.IsConstructCall()) {
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = info[0].As<v8::External>();
            void* ptr = ext->Value();
            CellId* cellid = static_cast<CellId*>(ptr);
            cellid->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        }

        CellId* obj = new CellId();
        obj->Wrap(info.This());

        if (info.Length() == 1) {
            v8::Local<v8::Object> fromObj = Nan::To<v8::Object>(info[0]).ToLocalChecked();
            v8::Local<v8::FunctionTemplate> point = Nan::New(Point::constructor);
            v8::Local<v8::FunctionTemplate> latlng = Nan::New(LatLng::constructor);
            if (point->HasInstance(fromObj)) {
                S2Point p = Nan::ObjectWrap::Unwrap<Point>(fromObj)->get();
                obj->this_ = S2CellId::FromPoint(p);
            } else if (latlng->HasInstance(fromObj)) {
                S2LatLng ll = Nan::ObjectWrap::Unwrap<LatLng>(fromObj)->get();
                obj->this_ = S2CellId::FromLatLng(ll);
            } else if (info[0]->IsString()) {
                Nan::Utf8String str(info[0]);
                std::string strnum {*str};
                obj->this_ = S2CellId(ParseLeadingUInt64Value(strnum, 0));
            } else {
                Nan::ThrowTypeError("Invalid input.");
                return;
            }
        } else {
            obj->this_ = S2CellId();
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

v8::Local<v8::Object> CellId::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2CellId s2cellid) {
    Nan::EscapableHandleScope scope;

    CellId* obj = new CellId();
    obj->this_ = s2cellid;

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(CellId::FromToken) {
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("(str) required");
        return;
    }
    Nan::Utf8String str(info[0]);
    std::string strtoken {*str};
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    obj->this_ = S2CellId::FromToken(strtoken);
    info.GetReturnValue().Set(info.This());
}

NAN_METHOD(CellId::Level) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.level()));
}

NAN_METHOD(CellId::ToToken) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.ToToken().c_str()).ToLocalChecked());
}

NAN_METHOD(CellId::ToString) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.ToString().c_str()).ToLocalChecked());
}

NAN_METHOD(CellId::ToPoint) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(Point::CreateNew(info, obj->this_.ToPoint()));
}

NAN_METHOD(CellId::Parent) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    if (info.Length() == 1 && info[0]->IsNumber()) {
        info.GetReturnValue().Set(CellId::CreateNew(info, obj->this_.parent(Nan::To<double>(info[0]).FromJust())));
    } else {
        info.GetReturnValue().Set(CellId::CreateNew(info, obj->this_.parent()));
    }
}

NAN_METHOD(CellId::Prev) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(CellId::CreateNew(info, obj->this_.prev()));
}

NAN_METHOD(CellId::Next) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(CellId::CreateNew(info, obj->this_.next()));
}

NAN_METHOD(CellId::ChildBegin) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(CellId::CreateNew(info, obj->this_.child_begin()));
}

NAN_METHOD(CellId::ChildEnd) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(CellId::CreateNew(info, obj->this_.child_end()));
}

NAN_METHOD(CellId::Neighbors) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    S2CellId neighbors[4];
    obj->this_.GetEdgeNeighbors(neighbors);
    v8::Local<v8::Array> neighborsArray = Nan::New<v8::Array>(4);
    info.GetReturnValue().Set(neighborsArray);
    for (int ii = 0; ii < 4; ++ii) {
        Nan::Set(neighborsArray, ii, CellId::CreateNew(info, neighbors[ii]));
    }
}

NAN_METHOD(CellId::IsFace) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.is_face()));
}

NAN_METHOD(CellId::RangeMin) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(CellId::CreateNew(info, obj->this_.range_min()));
}

NAN_METHOD(CellId::RangeMax) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(CellId::CreateNew(info, obj->this_.range_max()));
}

NAN_METHOD(CellId::Contains) {
    CellId* cellid = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    S2CellId other = Nan::ObjectWrap::Unwrap<CellId>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    info.GetReturnValue().Set(Nan::New(cellid->this_.contains(other)));
}

NAN_METHOD(CellId::Children) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    S2CellId cell_id = obj->this_.child_begin();
    S2CellId end = obj->this_.child_end();
    v8::Local<v8::Array> nodes = Nan::New<v8::Array>();
    int index = 0;

    while(cell_id != end){
        char str[21];
        sprintf(str, "%llu", cell_id.id());
        v8::Local<v8::String> strValue = Nan::New(str).ToLocalChecked();
        Nan::Set(nodes, index++, strValue);
        cell_id = cell_id.next();
    }
    info.GetReturnValue().Set(nodes);
}

NAN_METHOD(CellId::Id) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    char str[21];
    sprintf(str, "%llu", obj->this_.id());
    info.GetReturnValue().Set(Nan::New(str).ToLocalChecked());
}

NAN_METHOD(CellId::IdString) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(Nan::New(std::to_string(obj->this_.id()).c_str()).ToLocalChecked());
}

NAN_METHOD(CellId::Child) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    if (info.Length() != 1) {
        Nan::ThrowTypeError("(number) required");
        return;
    }
    info.GetReturnValue().Set(CellId::CreateNew(info, obj->this_.child(Nan::To<double>(info[0]).FromJust())));
}


NAN_METHOD(CellId::ToLatLng) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(LatLng::CreateNew(info, obj->this_.ToLatLng()));
}

NAN_METHOD(CellId::Face) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.face()));
}

// Exposes the (face, i, j, orientation) tuple that underlies the cell id.
// Useful for debugging coordinate conversions against another S2 port.
NAN_METHOD(CellId::ToFaceIJ) {
    CellId* obj = Nan::ObjectWrap::Unwrap<CellId>(info.Holder());
    int i, j, orientation;
    int face = obj->this_.ToFaceIJOrientation(&i, &j, &orientation);
    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    Nan::Set(result, Nan::New("face").ToLocalChecked(), Nan::New(face));
    Nan::Set(result, Nan::New("i").ToLocalChecked(), Nan::New(i));
    Nan::Set(result, Nan::New("j").ToLocalChecked(), Nan::New(j));
    Nan::Set(result, Nan::New("orientation").ToLocalChecked(), Nan::New(orientation));
    info.GetReturnValue().Set(result);
}
}
