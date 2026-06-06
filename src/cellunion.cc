#include "cellunion.h"
#include <vector>

namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> CellUnion::constructor;

CellUnion::CellUnion() : this_() {}

NAN_MODULE_INIT(CellUnion::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S2CellUnion").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "numCells", NumCells);
    Nan::SetPrototypeMethod(tpl, "cellId", GetCellId);
    Nan::SetPrototypeMethod(tpl, "cellIds", CellIds);
    Nan::SetPrototypeMethod(tpl, "contains", Contains);
    Nan::SetPrototypeMethod(tpl, "intersects", Intersects);
    Nan::SetPrototypeMethod(tpl, "getUnion", GetUnion);
    Nan::SetPrototypeMethod(tpl, "getIntersection", GetIntersection);
    Nan::SetPrototypeMethod(tpl, "getDifference", GetDifference);
    Nan::SetPrototypeMethod(tpl, "normalize", Normalize);
    Nan::SetPrototypeMethod(tpl, "expand", Expand);
    Nan::SetPrototypeMethod(tpl, "getCapBound", GetCapBound);
    Nan::SetPrototypeMethod(tpl, "getRectBound", GetRectBound);
    Nan::SetPrototypeMethod(tpl, "leafCellsCovered", LeafCellsCovered);
    Nan::SetPrototypeMethod(tpl, "averageBasedArea", AverageBasedArea);
    Nan::SetPrototypeMethod(tpl, "approxArea", ApproxArea);
    Nan::SetPrototypeMethod(tpl, "exactArea", ExactArea);
    Nan::SetPrototypeMethod(tpl, "toString", ToString);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S2CellUnion").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(CellUnion::New) {
    if (info.IsConstructCall()) {
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = info[0].As<v8::External>();
            void* ptr = ext->Value();
            CellUnion* cu = static_cast<CellUnion*>(ptr);
            cu->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        }

        CellUnion* obj = new CellUnion();
        obj->Wrap(info.This());

        // Optional: initialize from an array of S2CellID instances.
        if (info.Length() >= 1 && info[0]->IsArray()) {
            v8::Local<v8::Array> arr = info[0].As<v8::Array>();
            v8::Local<v8::FunctionTemplate> cellIdTpl = Nan::New(CellId::constructor);

            std::vector<S2CellId> ids;
            ids.reserve(arr->Length());
            for (uint32_t i = 0; i < arr->Length(); ++i) {
                v8::Local<v8::Value> elem = Nan::Get(arr, i).ToLocalChecked();
                if (!elem->IsObject()) {
                    Nan::ThrowTypeError("array elements must be S2CellID instances");
                    return;
                }
                v8::Local<v8::Object> el = Nan::To<v8::Object>(elem).ToLocalChecked();
                if (!cellIdTpl->HasInstance(el)) {
                    Nan::ThrowTypeError("array elements must be S2CellID instances");
                    return;
                }
                ids.push_back(Nan::ObjectWrap::Unwrap<CellId>(el)->get());
            }
            obj->this_.Init(ids);
        }

        info.GetReturnValue().Set(info.This());
    } else {
        const int argc = info.Length();
        std::vector<v8::Local<v8::Value>> argv(argc);
        for (int i = 0; i < argc; ++i) {
            argv[i] = info[i];
        }
        v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
        info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv.data()).ToLocalChecked());
    }
}

v8::Local<v8::Object> CellUnion::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, CellUnion* obj) {
    Nan::EscapableHandleScope scope;

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(CellUnion::NumCells) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.num_cells()));
}

NAN_METHOD(CellUnion::GetCellId) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    int i = Nan::To<int32_t>(info[0]).FromJust();
    if (i < 0 || i >= obj->this_.num_cells()) {
        Nan::ThrowRangeError("cell index out of range");
        return;
    }
    info.GetReturnValue().Set(CellId::CreateNew(info, obj->this_.cell_id(i)));
}

NAN_METHOD(CellUnion::CellIds) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    int n = obj->this_.num_cells();
    v8::Local<v8::Array> out = Nan::New<v8::Array>(n);
    for (int i = 0; i < n; ++i) {
        Nan::Set(out, i, CellId::CreateNew(info, obj->this_.cell_id(i)));
    }
    info.GetReturnValue().Set(out);
}

NAN_METHOD(CellUnion::Contains) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());

    if (info.Length() < 1 || !info[0]->IsObject()) {
        Nan::ThrowTypeError("(S2CellID | S2Cell | S2Point | S2CellUnion) required");
        return;
    }
    v8::Local<v8::Object> arg = Nan::To<v8::Object>(info[0]).ToLocalChecked();

    v8::Local<v8::FunctionTemplate> cellIdTpl = Nan::New(CellId::constructor);
    v8::Local<v8::FunctionTemplate> cellTpl = Nan::New(Cell::constructor);
    v8::Local<v8::FunctionTemplate> pointTpl = Nan::New(Point::constructor);
    v8::Local<v8::FunctionTemplate> unionTpl = Nan::New(CellUnion::constructor);

    if (cellIdTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_.Contains(Nan::ObjectWrap::Unwrap<CellId>(arg)->get())));
    } else if (cellTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_.Contains(Nan::ObjectWrap::Unwrap<Cell>(arg)->get())));
    } else if (pointTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_.Contains(Nan::ObjectWrap::Unwrap<Point>(arg)->get())));
    } else if (unionTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_.Contains(&Nan::ObjectWrap::Unwrap<CellUnion>(arg)->this_)));
    } else {
        Nan::ThrowTypeError("(S2CellID | S2Cell | S2Point | S2CellUnion) required");
    }
}

NAN_METHOD(CellUnion::Intersects) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());

    if (info.Length() < 1 || !info[0]->IsObject()) {
        Nan::ThrowTypeError("(S2CellID | S2CellUnion) required");
        return;
    }
    v8::Local<v8::Object> arg = Nan::To<v8::Object>(info[0]).ToLocalChecked();

    v8::Local<v8::FunctionTemplate> cellIdTpl = Nan::New(CellId::constructor);
    v8::Local<v8::FunctionTemplate> unionTpl = Nan::New(CellUnion::constructor);

    if (cellIdTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_.Intersects(Nan::ObjectWrap::Unwrap<CellId>(arg)->get())));
    } else if (unionTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_.Intersects(&Nan::ObjectWrap::Unwrap<CellUnion>(arg)->this_)));
    } else {
        Nan::ThrowTypeError("(S2CellID | S2CellUnion) required");
    }
}

NAN_METHOD(CellUnion::GetUnion) {
    CellUnion* self = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    v8::Local<v8::FunctionTemplate> unionTpl = Nan::New(CellUnion::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !unionTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2CellUnion) required");
        return;
    }
    CellUnion* other = Nan::ObjectWrap::Unwrap<CellUnion>(Nan::To<v8::Object>(info[0]).ToLocalChecked());

    CellUnion* result = new CellUnion();
    result->this_.GetUnion(&self->this_, &other->this_);
    info.GetReturnValue().Set(CreateNew(info, result));
}

NAN_METHOD(CellUnion::GetIntersection) {
    CellUnion* self = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    v8::Local<v8::FunctionTemplate> unionTpl = Nan::New(CellUnion::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !unionTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2CellUnion) required");
        return;
    }
    CellUnion* other = Nan::ObjectWrap::Unwrap<CellUnion>(Nan::To<v8::Object>(info[0]).ToLocalChecked());

    CellUnion* result = new CellUnion();
    result->this_.GetIntersection(&self->this_, &other->this_);
    info.GetReturnValue().Set(CreateNew(info, result));
}

NAN_METHOD(CellUnion::GetDifference) {
    CellUnion* self = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    v8::Local<v8::FunctionTemplate> unionTpl = Nan::New(CellUnion::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !unionTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2CellUnion) required");
        return;
    }
    CellUnion* other = Nan::ObjectWrap::Unwrap<CellUnion>(Nan::To<v8::Object>(info[0]).ToLocalChecked());

    CellUnion* result = new CellUnion();
    result->this_.GetDifference(&self->this_, &other->this_);
    info.GetReturnValue().Set(CreateNew(info, result));
}

NAN_METHOD(CellUnion::Normalize) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.Normalize()));
}

NAN_METHOD(CellUnion::Expand) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    if (info.Length() < 1 || !info[0]->IsNumber()) {
        Nan::ThrowTypeError("(level) required");
        return;
    }
    obj->this_.Expand(Nan::To<int32_t>(info[0]).FromJust());
    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(CellUnion::GetCapBound) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    info.GetReturnValue().Set(Cap::CreateNew(info, obj->this_.GetCapBound()));
}

NAN_METHOD(CellUnion::GetRectBound) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    info.GetReturnValue().Set(LatLngRect::CreateNew(info, obj->this_.GetRectBound()));
}

NAN_METHOD(CellUnion::LeafCellsCovered) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    info.GetReturnValue().Set(Nan::New(static_cast<double>(obj->this_.LeafCellsCovered())));
}

NAN_METHOD(CellUnion::AverageBasedArea) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.AverageBasedArea()));
}

NAN_METHOD(CellUnion::ApproxArea) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.ApproxArea()));
}

NAN_METHOD(CellUnion::ExactArea) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_.ExactArea()));
}

NAN_METHOD(CellUnion::ToString) {
    CellUnion* obj = Nan::ObjectWrap::Unwrap<CellUnion>(info.Holder());
    std::string out = "S2CellUnion[";
    int n = obj->this_.num_cells();
    for (int i = 0; i < n; ++i) {
        if (i > 0) out += ", ";
        out += obj->this_.cell_id(i).ToToken();
    }
    out += "]";
    info.GetReturnValue().Set(Nan::New(out.c_str()).ToLocalChecked());
}

}
