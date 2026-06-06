#include "loop.h"
#include <vector>

namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> Loop::constructor;

Loop::Loop() : this_(new S2Loop()) {}

Loop::~Loop() {
    delete this_;
}

NAN_MODULE_INIT(Loop::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S2Loop").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "numVertices", NumVertices);
    Nan::SetPrototypeMethod(tpl, "vertex", Vertex);
    Nan::SetPrototypeMethod(tpl, "isValid", IsValid);
    Nan::SetPrototypeMethod(tpl, "isNormalized", IsNormalized);
    Nan::SetPrototypeMethod(tpl, "normalize", Normalize);
    Nan::SetPrototypeMethod(tpl, "invert", Invert);
    Nan::SetPrototypeMethod(tpl, "getArea", GetArea);
    Nan::SetPrototypeMethod(tpl, "getCentroid", GetCentroid);
    Nan::SetPrototypeMethod(tpl, "getTurningAngle", GetTurningAngle);
    Nan::SetPrototypeMethod(tpl, "contains", Contains);
    Nan::SetPrototypeMethod(tpl, "intersects", Intersects);
    Nan::SetPrototypeMethod(tpl, "containsNested", ContainsNested);
    Nan::SetPrototypeMethod(tpl, "boundaryEquals", BoundaryEquals);
    Nan::SetPrototypeMethod(tpl, "boundaryApproxEquals", BoundaryApproxEquals);
    Nan::SetPrototypeMethod(tpl, "mayIntersect", MayIntersect);
    Nan::SetPrototypeMethod(tpl, "getCapBound", GetCapBound);
    Nan::SetPrototypeMethod(tpl, "getRectBound", GetRectBound);
    Nan::SetPrototypeMethod(tpl, "depth", Depth);
    Nan::SetPrototypeMethod(tpl, "isHole", IsHole);
    Nan::SetPrototypeMethod(tpl, "sign", Sign);
    Nan::SetPrototypeMethod(tpl, "toString", ToString);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S2Loop").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Loop::New) {
    if (info.IsConstructCall()) {
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = info[0].As<v8::External>();
            Loop* loop = static_cast<Loop*>(ext->Value());
            loop->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        }

        Loop* obj = new Loop();
        obj->Wrap(info.This());

        if (info.Length() >= 1 && info[0]->IsArray()) {
            v8::Local<v8::Array> arr = info[0].As<v8::Array>();
            v8::Local<v8::FunctionTemplate> pointTpl = Nan::New(Point::constructor);

            std::vector<S2Point> verts;
            verts.reserve(arr->Length());
            for (uint32_t i = 0; i < arr->Length(); ++i) {
                v8::Local<v8::Value> elem = Nan::Get(arr, i).ToLocalChecked();
                if (!elem->IsObject()) {
                    Nan::ThrowTypeError("array elements must be S2Point instances");
                    return;
                }
                v8::Local<v8::Object> el = Nan::To<v8::Object>(elem).ToLocalChecked();
                if (!pointTpl->HasInstance(el)) {
                    Nan::ThrowTypeError("array elements must be S2Point instances");
                    return;
                }
                verts.push_back(Nan::ObjectWrap::Unwrap<Point>(el)->get());
            }
            obj->this_->Init(verts);
        } else if (info.Length() >= 1 && info[0]->IsObject()) {
            v8::Local<v8::Object> el = Nan::To<v8::Object>(info[0]).ToLocalChecked();
            v8::Local<v8::FunctionTemplate> cellTpl = Nan::New(Cell::constructor);
            if (cellTpl->HasInstance(el)) {
                delete obj->this_;
                obj->this_ = new S2Loop(Nan::ObjectWrap::Unwrap<Cell>(el)->get());
            } else {
                Nan::ThrowTypeError("(S2Point[] | S2Cell) required");
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

v8::Local<v8::Object> Loop::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Loop* loop) {
    Nan::EscapableHandleScope scope;

    Loop* obj = new Loop();
    delete obj->this_;       // discard the empty loop allocated by the constructor
    obj->this_ = loop;       // take ownership of the provided loop

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(Loop::NumVertices) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->num_vertices()));
}

NAN_METHOD(Loop::Vertex) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    int i = Nan::To<int32_t>(info[0]).FromJust();
    if (i < 0 || i >= obj->this_->num_vertices()) {
        Nan::ThrowRangeError("vertex index out of range");
        return;
    }
    info.GetReturnValue().Set(Point::CreateNew(info, obj->this_->vertex(i)));
}

NAN_METHOD(Loop::IsValid) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->IsValid()));
}

NAN_METHOD(Loop::IsNormalized) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->IsNormalized()));
}

NAN_METHOD(Loop::Normalize) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    obj->this_->Normalize();
    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Loop::Invert) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    obj->this_->Invert();
    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Loop::GetArea) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->GetArea()));
}

NAN_METHOD(Loop::GetCentroid) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    info.GetReturnValue().Set(Point::CreateNew(info, obj->this_->GetCentroid()));
}

NAN_METHOD(Loop::GetTurningAngle) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->GetTurningAngle()));
}

NAN_METHOD(Loop::Contains) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());

    if (info.Length() < 1 || !info[0]->IsObject()) {
        Nan::ThrowTypeError("(S2Loop | S2Cell | S2Point) required");
        return;
    }
    v8::Local<v8::Object> arg = Nan::To<v8::Object>(info[0]).ToLocalChecked();

    v8::Local<v8::FunctionTemplate> loopTpl = Nan::New(Loop::constructor);
    v8::Local<v8::FunctionTemplate> cellTpl = Nan::New(Cell::constructor);
    v8::Local<v8::FunctionTemplate> pointTpl = Nan::New(Point::constructor);

    if (loopTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_->Contains(Nan::ObjectWrap::Unwrap<Loop>(arg)->get())));
    } else if (cellTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_->Contains(Nan::ObjectWrap::Unwrap<Cell>(arg)->get())));
    } else if (pointTpl->HasInstance(arg)) {
        info.GetReturnValue().Set(Nan::New(obj->this_->Contains(Nan::ObjectWrap::Unwrap<Point>(arg)->get())));
    } else {
        Nan::ThrowTypeError("(S2Loop | S2Cell | S2Point) required");
    }
}

NAN_METHOD(Loop::Intersects) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    v8::Local<v8::FunctionTemplate> loopTpl = Nan::New(Loop::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !loopTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Loop) required");
        return;
    }
    Loop* other = Nan::ObjectWrap::Unwrap<Loop>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    info.GetReturnValue().Set(Nan::New(obj->this_->Intersects(other->get())));
}

NAN_METHOD(Loop::ContainsNested) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    v8::Local<v8::FunctionTemplate> loopTpl = Nan::New(Loop::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !loopTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Loop) required");
        return;
    }
    Loop* other = Nan::ObjectWrap::Unwrap<Loop>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    info.GetReturnValue().Set(Nan::New(obj->this_->ContainsNested(other->get())));
}

NAN_METHOD(Loop::BoundaryEquals) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    v8::Local<v8::FunctionTemplate> loopTpl = Nan::New(Loop::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !loopTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Loop) required");
        return;
    }
    Loop* other = Nan::ObjectWrap::Unwrap<Loop>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    info.GetReturnValue().Set(Nan::New(obj->this_->BoundaryEquals(other->get())));
}

NAN_METHOD(Loop::BoundaryApproxEquals) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    v8::Local<v8::FunctionTemplate> loopTpl = Nan::New(Loop::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !loopTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Loop[, maxError]) required");
        return;
    }
    Loop* other = Nan::ObjectWrap::Unwrap<Loop>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    double maxError = 1e-15;
    if (info.Length() >= 2 && info[1]->IsNumber()) {
        maxError = Nan::To<double>(info[1]).FromJust();
    }
    info.GetReturnValue().Set(Nan::New(obj->this_->BoundaryApproxEquals(other->get(), maxError)));
}

NAN_METHOD(Loop::MayIntersect) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    v8::Local<v8::FunctionTemplate> cellTpl = Nan::New(Cell::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !cellTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Cell) required");
        return;
    }
    Cell* cell = Nan::ObjectWrap::Unwrap<Cell>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    info.GetReturnValue().Set(Nan::New(obj->this_->MayIntersect(cell->get())));
}

NAN_METHOD(Loop::GetCapBound) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    info.GetReturnValue().Set(Cap::CreateNew(info, obj->this_->GetCapBound()));
}

NAN_METHOD(Loop::GetRectBound) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    info.GetReturnValue().Set(LatLngRect::CreateNew(info, obj->this_->GetRectBound()));
}

NAN_METHOD(Loop::Depth) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->depth()));
}

NAN_METHOD(Loop::IsHole) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->is_hole()));
}

NAN_METHOD(Loop::Sign) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->sign()));
}

NAN_METHOD(Loop::ToString) {
    Loop* obj = Nan::ObjectWrap::Unwrap<Loop>(info.Holder());
    char buf[64];
    snprintf(buf, sizeof(buf), "S2Loop[%d vertices]", obj->this_->num_vertices());
    info.GetReturnValue().Set(Nan::New(buf).ToLocalChecked());
}

}
