#include "regioncoverer.h"
#include "latlngrect.h"
#include "cap.h"
#include "cell.h"
#include "cellid.h"

namespace s2geo {

Nan::Persistent<v8::FunctionTemplate> RegionCoverer::constructor;

RegionCoverer::RegionCoverer() {}

RegionCoverer::~RegionCoverer() {}

NAN_MODULE_INIT(RegionCoverer::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("S2RegionCoverer").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype methods
    Nan::SetPrototypeMethod(tpl, "getCovering", GetCovering);

    constructor.Reset(tpl);

    Nan::Set(target,
             Nan::New("S2RegionCoverer").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(RegionCoverer::New) {
    if (info.IsConstructCall()) {
        RegionCoverer* obj = new RegionCoverer();
        obj->Wrap(info.This());
        info.GetReturnValue().Set(info.This());
    } else {
        v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
        info.GetReturnValue().Set(Nan::NewInstance(cons, 0, NULL).ToLocalChecked());
    }
}

v8::Local<v8::Object> RegionCoverer::CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    Nan::EscapableHandleScope scope;

    RegionCoverer* obj = new RegionCoverer();
    v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
    v8::Local<v8::Function> cons = Nan::New(constructor)->GetFunction(Nan::GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> instance = Nan::NewInstance(cons, 1, &ext).ToLocalChecked();

    return scope.Escape(instance);
}

string CellToString(const S2CellId& id) {
    return StringPrintf("%d:%s", id.level(), id.ToToken().c_str());
}

NAN_METHOD(RegionCoverer::GetCovering) {
    vector<S2CellId> covering;

    int argsLength = info.Length();
    S2RegionCoverer *cover = new S2RegionCoverer();

    // Set optional parameters
    if (argsLength >= 2 && info[1]->IsNumber())
        cover->set_min_level(Nan::To<int32_t>(info[1]).FromJust());
    if (argsLength >= 3 && info[2]->IsNumber())
        cover->set_max_level(Nan::To<int32_t>(info[2]).FromJust());
    if (argsLength >= 4 && info[3]->IsNumber())
        cover->set_max_cells(Nan::To<int32_t>(info[3]).FromJust());
    if (argsLength >= 5 && info[4]->IsNumber())
        cover->set_level_mod(Nan::To<int32_t>(info[4]).FromJust());

    // Get the persistent constructors
    v8::Local<v8::FunctionTemplate> latLngRect = Nan::New(LatLngRect::constructor);
    v8::Local<v8::FunctionTemplate> cap = Nan::New(Cap::constructor);
    v8::Local<v8::FunctionTemplate> cell = Nan::New(Cell::constructor);

    v8::Local<v8::Object> fromObj = Nan::To<v8::Object>(info[0]).ToLocalChecked();

    // Check instance type and get covering
    if (latLngRect->HasInstance(fromObj)) {
        S2LatLngRect region = Nan::ObjectWrap::Unwrap<LatLngRect>(fromObj)->get();
        cover->GetCovering(region, &covering);
    }
    else if (cap->HasInstance(fromObj)) {
        S2Cap region = Nan::ObjectWrap::Unwrap<Cap>(fromObj)->get();
        cover->GetCovering(region, &covering);
    }
    else if (cell->HasInstance(fromObj)) {
        S2Cell region = Nan::ObjectWrap::Unwrap<Cell>(fromObj)->get();
        cover->GetCovering(region, &covering);
    }

    // Create array of cell IDs
    v8::Local<v8::Array> cellIDs = Nan::New<v8::Array>(covering.size());

    for (size_t ii = 0; ii < covering.size(); ++ii) {
        Nan::Set(cellIDs, ii, CellId::CreateNew(info, covering[ii]));
    }

    delete cover;
    info.GetReturnValue().Set(cellIDs);
}

}