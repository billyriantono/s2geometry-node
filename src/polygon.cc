#include "polygon.h"
#include <vector>

namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> Polygon::constructor;

Polygon::Polygon() : this_(new S2Polygon()) {}

Polygon::~Polygon() {
    delete this_;
}

NAN_MODULE_INIT(Polygon::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S2Polygon").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "numLoops", NumLoops);
    Nan::SetPrototypeMethod(tpl, "numVertices", NumVertices);
    Nan::SetPrototypeMethod(tpl, "loop", GetLoop);
    Nan::SetPrototypeMethod(tpl, "getArea", GetArea);
    Nan::SetPrototypeMethod(tpl, "getCentroid", GetCentroid);
    Nan::SetPrototypeMethod(tpl, "contains", Contains);
    Nan::SetPrototypeMethod(tpl, "approxContains", ApproxContains);
    Nan::SetPrototypeMethod(tpl, "intersects", Intersects);
    Nan::SetPrototypeMethod(tpl, "getUnion", GetUnion);
    Nan::SetPrototypeMethod(tpl, "getIntersection", GetIntersection);
    Nan::SetPrototypeMethod(tpl, "getDifference", GetDifference);
    Nan::SetPrototypeMethod(tpl, "isValid", IsValid);
    Nan::SetPrototypeMethod(tpl, "isNormalized", IsNormalized);
    Nan::SetPrototypeMethod(tpl, "boundaryEquals", BoundaryEquals);
    Nan::SetPrototypeMethod(tpl, "boundaryApproxEquals", BoundaryApproxEquals);
    Nan::SetPrototypeMethod(tpl, "project", Project);
    Nan::SetPrototypeMethod(tpl, "initToCellUnionBorder", InitToCellUnionBorder);
    Nan::SetPrototypeMethod(tpl, "getParent", GetParent);
    Nan::SetPrototypeMethod(tpl, "getLastDescendant", GetLastDescendant);
    Nan::SetPrototypeMethod(tpl, "getCapBound", GetCapBound);
    Nan::SetPrototypeMethod(tpl, "getRectBound", GetRectBound);
    Nan::SetPrototypeMethod(tpl, "mayIntersect", MayIntersect);
    Nan::SetPrototypeMethod(tpl, "toString", ToString);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S2Polygon").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Polygon::New) {
    if (info.IsConstructCall()) {
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = info[0].As<v8::External>();
            Polygon* poly = static_cast<Polygon*>(ext->Value());
            poly->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        }

        Polygon* obj = new Polygon();
        obj->Wrap(info.This());

        if (info.Length() >= 1 && info[0]->IsArray()) {
            v8::Local<v8::Array> arr = info[0].As<v8::Array>();
            v8::Local<v8::FunctionTemplate> loopTpl = Nan::New(Loop::constructor);

            // S2Polygon::Init takes ownership of the loops it is given, so we
            // hand it *clones* of each wrapped loop and keep the originals.
            std::vector<S2Loop*> loops;
            loops.reserve(arr->Length());
            for (uint32_t i = 0; i < arr->Length(); ++i) {
                v8::Local<v8::Value> elem = Nan::Get(arr, i).ToLocalChecked();
                if (!elem->IsObject()) {
                    for (size_t j = 0; j < loops.size(); ++j) delete loops[j];
                    Nan::ThrowTypeError("array elements must be S2Loop instances");
                    return;
                }
                v8::Local<v8::Object> el = Nan::To<v8::Object>(elem).ToLocalChecked();
                if (!loopTpl->HasInstance(el)) {
                    for (size_t j = 0; j < loops.size(); ++j) delete loops[j];
                    Nan::ThrowTypeError("array elements must be S2Loop instances");
                    return;
                }
                loops.push_back(Nan::ObjectWrap::Unwrap<Loop>(el)->get()->Clone());
            }
            obj->this_->Init(&loops);  // clears `loops` and takes ownership
        } else if (info.Length() >= 1 && info[0]->IsObject()) {
            v8::Local<v8::Object> el = Nan::To<v8::Object>(info[0]).ToLocalChecked();
            v8::Local<v8::FunctionTemplate> cellTpl = Nan::New(Cell::constructor);
            if (cellTpl->HasInstance(el)) {
                delete obj->this_;
                obj->this_ = new S2Polygon(Nan::ObjectWrap::Unwrap<Cell>(el)->get());
            } else {
                Nan::ThrowTypeError("(S2Loop[] | S2Cell) required");
                return;
            }
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

v8::Local<v8::Object> Polygon::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Polygon* poly) {
    Nan::EscapableHandleScope scope;

    Polygon* obj = new Polygon();
    delete obj->this_;       // discard the empty polygon allocated by the constructor
    obj->this_ = poly;       // take ownership of the provided polygon

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(Polygon::NumLoops) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->num_loops()));
}

NAN_METHOD(Polygon::NumVertices) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->num_vertices()));
}

NAN_METHOD(Polygon::GetLoop) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    int k = Nan::To<int32_t>(info[0]).FromJust();
    if (k < 0 || k >= obj->this_->num_loops()) {
        Nan::ThrowRangeError("loop index out of range");
        return;
    }
    // Clone so the returned S2Loop wrapper owns its own copy (the polygon
    // retains ownership of its internal loops).
    info.GetReturnValue().Set(Loop::CreateNew(info, obj->this_->loop(k)->Clone()));
}

NAN_METHOD(Polygon::GetArea) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->GetArea()));
}

NAN_METHOD(Polygon::GetCentroid) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    info.GetReturnValue().Set(Point::CreateNew(info, obj->this_->GetCentroid()));
}

NAN_METHOD(Polygon::Contains) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());

    if (info.Length() < 1 || !info[0]->IsObject()) {
        Nan::ThrowTypeError("(S2Polygon | S2Cell | S2Point) required");
        return;
    }
    v8::Local<v8::Object> arg = Nan::To<v8::Object>(info[0]).ToLocalChecked();

    v8::Local<v8::FunctionTemplate> polyTpl = Nan::New(Polygon::constructor);
    v8::Local<v8::FunctionTemplate> cellTpl = Nan::New(Cell::constructor);
    v8::Local<v8::FunctionTemplate> pointTpl = Nan::New(Point::constructor);

    if (polyTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_->Contains(Nan::ObjectWrap::Unwrap<Polygon>(arg)->get())));
    } else if (cellTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_->Contains(Nan::ObjectWrap::Unwrap<Cell>(arg)->get())));
    } else if (pointTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_->Contains(Nan::ObjectWrap::Unwrap<Point>(arg)->get())));
    } else {
        Nan::ThrowTypeError("(S2Polygon | S2Cell | S2Point) required");
    }
}

NAN_METHOD(Polygon::ApproxContains) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    v8::Local<v8::FunctionTemplate> polyTpl = Nan::New(Polygon::constructor);
    if (info.Length() < 2 || !info[0]->IsObject() ||
        !polyTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked()) ||
        !info[1]->IsNumber()) {
        Nan::ThrowTypeError("(S2Polygon, vertexMergeRadiusRadians) required");
        return;
    }
    Polygon* other = Nan::ObjectWrap::Unwrap<Polygon>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    S1Angle radius = S1Angle::Radians(Nan::To<double>(info[1]).FromJust());
    info.GetReturnValue().Set(Nan::New(obj->this_->ApproxContains(other->get(), radius)));
}

NAN_METHOD(Polygon::Intersects) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    v8::Local<v8::FunctionTemplate> polyTpl = Nan::New(Polygon::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !polyTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Polygon) required");
        return;
    }
    Polygon* other = Nan::ObjectWrap::Unwrap<Polygon>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    info.GetReturnValue().Set(Nan::New(obj->this_->Intersects(other->get())));
}

NAN_METHOD(Polygon::GetUnion) {
    Polygon* self = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    v8::Local<v8::FunctionTemplate> polyTpl = Nan::New(Polygon::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !polyTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Polygon) required");
        return;
    }
    Polygon* other = Nan::ObjectWrap::Unwrap<Polygon>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    S2Polygon* result = new S2Polygon();
    result->InitToUnion(self->this_, other->this_);
    info.GetReturnValue().Set(CreateNew(info, result));
}

NAN_METHOD(Polygon::GetIntersection) {
    Polygon* self = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    v8::Local<v8::FunctionTemplate> polyTpl = Nan::New(Polygon::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !polyTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Polygon) required");
        return;
    }
    Polygon* other = Nan::ObjectWrap::Unwrap<Polygon>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    S2Polygon* result = new S2Polygon();
    result->InitToIntersection(self->this_, other->this_);
    info.GetReturnValue().Set(CreateNew(info, result));
}

NAN_METHOD(Polygon::GetDifference) {
    Polygon* self = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    v8::Local<v8::FunctionTemplate> polyTpl = Nan::New(Polygon::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !polyTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Polygon) required");
        return;
    }
    Polygon* other = Nan::ObjectWrap::Unwrap<Polygon>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    S2Polygon* result = new S2Polygon();
    result->InitToDifference(self->this_, other->this_);
    info.GetReturnValue().Set(CreateNew(info, result));
}

NAN_METHOD(Polygon::IsValid) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->IsValid()));
}

NAN_METHOD(Polygon::IsNormalized) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->IsNormalized()));
}

NAN_METHOD(Polygon::BoundaryEquals) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    v8::Local<v8::FunctionTemplate> polyTpl = Nan::New(Polygon::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !polyTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Polygon) required");
        return;
    }
    Polygon* other = Nan::ObjectWrap::Unwrap<Polygon>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    info.GetReturnValue().Set(Nan::New(obj->this_->BoundaryEquals(other->get())));
}

NAN_METHOD(Polygon::BoundaryApproxEquals) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    v8::Local<v8::FunctionTemplate> polyTpl = Nan::New(Polygon::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !polyTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Polygon[, maxError]) required");
        return;
    }
    Polygon* other = Nan::ObjectWrap::Unwrap<Polygon>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    double maxError = 1e-15;
    if (info.Length() >= 2 && info[1]->IsNumber()) {
        maxError = Nan::To<double>(info[1]).FromJust();
    }
    info.GetReturnValue().Set(Nan::New(obj->this_->BoundaryApproxEquals(other->get(), maxError)));
}

NAN_METHOD(Polygon::Project) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    v8::Local<v8::FunctionTemplate> pointTpl = Nan::New(Point::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !pointTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Point) required");
        return;
    }
    if (obj->this_->num_loops() == 0) {
        Nan::ThrowError("polygon is empty");
        return;
    }
    S2Point p = Nan::ObjectWrap::Unwrap<Point>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    info.GetReturnValue().Set(Point::CreateNew(info, obj->this_->Project(p)));
}

NAN_METHOD(Polygon::InitToCellUnionBorder) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    v8::Local<v8::FunctionTemplate> unionTpl = Nan::New(CellUnion::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !unionTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2CellUnion) required");
        return;
    }
    CellUnion* cu = Nan::ObjectWrap::Unwrap<CellUnion>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    // Reset to an empty polygon before initializing from the cell union border.
    delete obj->this_;
    obj->this_ = new S2Polygon();
    obj->this_->InitToCellUnionBorder(cu->get());
    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Polygon::GetParent) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    int k = Nan::To<int32_t>(info[0]).FromJust();
    info.GetReturnValue().Set(Nan::New(obj->this_->GetParent(k)));
}

NAN_METHOD(Polygon::GetLastDescendant) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    int k = Nan::To<int32_t>(info[0]).FromJust();
    info.GetReturnValue().Set(Nan::New(obj->this_->GetLastDescendant(k)));
}

NAN_METHOD(Polygon::GetCapBound) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    info.GetReturnValue().Set(Cap::CreateNew(info, obj->this_->GetCapBound()));
}

NAN_METHOD(Polygon::GetRectBound) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    info.GetReturnValue().Set(LatLngRect::CreateNew(info, obj->this_->GetRectBound()));
}

NAN_METHOD(Polygon::MayIntersect) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    v8::Local<v8::FunctionTemplate> cellTpl = Nan::New(Cell::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !cellTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Cell) required");
        return;
    }
    Cell* cell = Nan::ObjectWrap::Unwrap<Cell>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    info.GetReturnValue().Set(Nan::New(obj->this_->MayIntersect(cell->get())));
}

NAN_METHOD(Polygon::ToString) {
    Polygon* obj = Nan::ObjectWrap::Unwrap<Polygon>(info.Holder());
    char buf[80];
    snprintf(buf, sizeof(buf), "S2Polygon[%d loops, %d vertices]",
             obj->this_->num_loops(), obj->this_->num_vertices());
    info.GetReturnValue().Set(Nan::New(buf).ToLocalChecked());
}

}
