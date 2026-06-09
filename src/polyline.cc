#include "polyline.h"
#include <vector>

namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> Polyline::constructor;

Polyline::Polyline() : this_(new S2Polyline()) {}

Polyline::~Polyline() {
    delete this_;
}

NAN_MODULE_INIT(Polyline::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S2Polyline").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "numVertices", NumVertices);
    Nan::SetPrototypeMethod(tpl, "vertex", Vertex);
    Nan::SetPrototypeMethod(tpl, "getLength", GetLength);
    Nan::SetPrototypeMethod(tpl, "getCentroid", GetCentroid);
    Nan::SetPrototypeMethod(tpl, "interpolate", Interpolate);
    Nan::SetPrototypeMethod(tpl, "getSuffix", GetSuffix);
    Nan::SetPrototypeMethod(tpl, "unInterpolate", UnInterpolate);
    Nan::SetPrototypeMethod(tpl, "project", Project);
    Nan::SetPrototypeMethod(tpl, "isOnRight", IsOnRight);
    Nan::SetPrototypeMethod(tpl, "intersects", Intersects);
    Nan::SetPrototypeMethod(tpl, "reverse", Reverse);
    Nan::SetPrototypeMethod(tpl, "approxEquals", ApproxEquals);
    Nan::SetPrototypeMethod(tpl, "getCapBound", GetCapBound);
    Nan::SetPrototypeMethod(tpl, "getRectBound", GetRectBound);
    Nan::SetPrototypeMethod(tpl, "mayIntersect", MayIntersect);
    Nan::SetPrototypeMethod(tpl, "toString", ToString);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S2Polyline").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Polyline::New) {
    if (info.IsConstructCall()) {
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = info[0].As<v8::External>();
            Polyline* line = static_cast<Polyline*>(ext->Value());
            line->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        }

        Polyline* obj = new Polyline();
        obj->Wrap(info.This());

        if (info.Length() >= 1 && info[0]->IsArray()) {
            v8::Local<v8::Array> arr = info[0].As<v8::Array>();
            v8::Local<v8::FunctionTemplate> pointTpl = Nan::New(Point::constructor);
            v8::Local<v8::FunctionTemplate> latlngTpl = Nan::New(LatLng::constructor);

            std::vector<S2Point> points;
            std::vector<S2LatLng> latlngs;
            points.reserve(arr->Length());

            for (uint32_t i = 0; i < arr->Length(); ++i) {
                v8::Local<v8::Value> elem = Nan::Get(arr, i).ToLocalChecked();
                if (!elem->IsObject()) {
                    Nan::ThrowTypeError("array elements must be S2Point or S2LatLng instances");
                    return;
                }
                v8::Local<v8::Object> el = Nan::To<v8::Object>(elem).ToLocalChecked();
                if (pointTpl->HasInstance(el)) {
                    points.push_back(Nan::ObjectWrap::Unwrap<Point>(el)->get());
                } else if (latlngTpl->HasInstance(el)) {
                    latlngs.push_back(Nan::ObjectWrap::Unwrap<LatLng>(el)->get());
                } else {
                    Nan::ThrowTypeError("array elements must be S2Point or S2LatLng instances");
                    return;
                }
            }

            if (!latlngs.empty() && !points.empty()) {
                Nan::ThrowTypeError("array must contain a single vertex type (S2Point or S2LatLng)");
                return;
            }
            if (!latlngs.empty()) {
                obj->this_->Init(latlngs);
            } else {
                obj->this_->Init(points);
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

v8::Local<v8::Object> Polyline::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, S2Polyline* line) {
    Nan::EscapableHandleScope scope;

    Polyline* obj = new Polyline();
    delete obj->this_;       // discard the empty polyline allocated by the constructor
    obj->this_ = line;       // take ownership of the provided polyline

    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

NAN_METHOD(Polyline::NumVertices) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->this_->num_vertices()));
}

NAN_METHOD(Polyline::Vertex) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    int k = Nan::To<int32_t>(info[0]).FromJust();
    if (k < 0 || k >= obj->this_->num_vertices()) {
        Nan::ThrowRangeError("vertex index out of range");
        return;
    }
    info.GetReturnValue().Set(Point::CreateNew(info, obj->this_->vertex(k)));
}

NAN_METHOD(Polyline::GetLength) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    // Return the length in radians (an arc length on the unit sphere).
    info.GetReturnValue().Set(Nan::New(obj->this_->GetLength().radians()));
}

NAN_METHOD(Polyline::GetCentroid) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    info.GetReturnValue().Set(Point::CreateNew(info, obj->this_->GetCentroid()));
}

NAN_METHOD(Polyline::Interpolate) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    if (info.Length() < 1 || !info[0]->IsNumber()) {
        Nan::ThrowTypeError("(fraction) required");
        return;
    }
    if (obj->this_->num_vertices() == 0) {
        Nan::ThrowError("polyline is empty");
        return;
    }
    double fraction = Nan::To<double>(info[0]).FromJust();
    info.GetReturnValue().Set(Point::CreateNew(info, obj->this_->Interpolate(fraction)));
}

NAN_METHOD(Polyline::GetSuffix) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    if (info.Length() < 1 || !info[0]->IsNumber()) {
        Nan::ThrowTypeError("(fraction) required");
        return;
    }
    if (obj->this_->num_vertices() == 0) {
        Nan::ThrowError("polyline is empty");
        return;
    }
    double fraction = Nan::To<double>(info[0]).FromJust();
    int nextVertex = 0;
    S2Point p = obj->this_->GetSuffix(fraction, &nextVertex);

    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    Nan::Set(result, Nan::New("point").ToLocalChecked(), Point::CreateNew(info, p));
    Nan::Set(result, Nan::New("nextVertex").ToLocalChecked(), Nan::New(nextVertex));
    info.GetReturnValue().Set(result);
}

NAN_METHOD(Polyline::UnInterpolate) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    v8::Local<v8::FunctionTemplate> pointTpl = Nan::New(Point::constructor);
    if (info.Length() < 2 || !info[0]->IsObject() ||
        !pointTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked()) ||
        !info[1]->IsNumber()) {
        Nan::ThrowTypeError("(S2Point, nextVertex) required");
        return;
    }
    S2Point p = Nan::ObjectWrap::Unwrap<Point>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    int nextVertex = Nan::To<int32_t>(info[1]).FromJust();
    info.GetReturnValue().Set(Nan::New(obj->this_->UnInterpolate(p, nextVertex)));
}

NAN_METHOD(Polyline::Project) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    v8::Local<v8::FunctionTemplate> pointTpl = Nan::New(Point::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !pointTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Point) required");
        return;
    }
    if (obj->this_->num_vertices() == 0) {
        Nan::ThrowError("polyline is empty");
        return;
    }
    S2Point query = Nan::ObjectWrap::Unwrap<Point>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    int nextVertex = 0;
    S2Point p = obj->this_->Project(query, &nextVertex);

    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    Nan::Set(result, Nan::New("point").ToLocalChecked(), Point::CreateNew(info, p));
    Nan::Set(result, Nan::New("nextVertex").ToLocalChecked(), Nan::New(nextVertex));
    info.GetReturnValue().Set(result);
}

NAN_METHOD(Polyline::IsOnRight) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    v8::Local<v8::FunctionTemplate> pointTpl = Nan::New(Point::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !pointTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Point) required");
        return;
    }
    if (obj->this_->num_vertices() < 2) {
        Nan::ThrowError("polyline must have at least 2 vertices");
        return;
    }
    S2Point p = Nan::ObjectWrap::Unwrap<Point>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->get();
    info.GetReturnValue().Set(Nan::New(obj->this_->IsOnRight(p)));
}

NAN_METHOD(Polyline::Intersects) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    v8::Local<v8::FunctionTemplate> lineTpl = Nan::New(Polyline::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !lineTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Polyline) required");
        return;
    }
    Polyline* other = Nan::ObjectWrap::Unwrap<Polyline>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    info.GetReturnValue().Set(Nan::New(obj->this_->Intersects(other->get())));
}

NAN_METHOD(Polyline::Reverse) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    obj->this_->Reverse();
    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Polyline::ApproxEquals) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    v8::Local<v8::FunctionTemplate> lineTpl = Nan::New(Polyline::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !lineTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Polyline[, maxError]) required");
        return;
    }
    Polyline* other = Nan::ObjectWrap::Unwrap<Polyline>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    double maxError = 1e-15;
    if (info.Length() >= 2 && info[1]->IsNumber()) {
        maxError = Nan::To<double>(info[1]).FromJust();
    }
    info.GetReturnValue().Set(Nan::New(obj->this_->ApproxEquals(other->get(), maxError)));
}

NAN_METHOD(Polyline::GetCapBound) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    info.GetReturnValue().Set(Cap::CreateNew(info, obj->this_->GetCapBound()));
}

NAN_METHOD(Polyline::GetRectBound) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    info.GetReturnValue().Set(LatLngRect::CreateNew(info, obj->this_->GetRectBound()));
}

NAN_METHOD(Polyline::MayIntersect) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    v8::Local<v8::FunctionTemplate> cellTpl = Nan::New(Cell::constructor);
    if (info.Length() < 1 || !info[0]->IsObject() ||
        !cellTpl->HasInstance(Nan::To<v8::Object>(info[0]).ToLocalChecked())) {
        Nan::ThrowTypeError("(S2Cell) required");
        return;
    }
    Cell* cell = Nan::ObjectWrap::Unwrap<Cell>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    info.GetReturnValue().Set(Nan::New(obj->this_->MayIntersect(cell->get())));
}

NAN_METHOD(Polyline::ToString) {
    Polyline* obj = Nan::ObjectWrap::Unwrap<Polyline>(info.Holder());
    char buf[64];
    snprintf(buf, sizeof(buf), "S2Polyline[%d vertices]", obj->this_->num_vertices());
    info.GetReturnValue().Set(Nan::New(buf).ToLocalChecked());
}

}
