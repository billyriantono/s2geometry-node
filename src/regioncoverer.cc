#include "regioncoverer.h"
#include "latlngrect.h"
namespace s2geo {

using namespace v8;

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;

Persistent<FunctionTemplate> RegionCoverer::constructor;

RegionCoverer::RegionCoverer()
    : ObjectWrap(){}

RegionCoverer::~RegionCoverer() {
}

void RegionCoverer::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "S2RegionCoverer"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype


	NODE_SET_PROTOTYPE_METHOD(tpl, "getCovering", GetCovering);
    //NODE_SET_PROTOTYPE_METHOD(tpl, "setMaxLevel", SetMaxLevel);
    //NODE_SET_PROTOTYPE_METHOD(tpl, "getMaxLevel", GetMaxLevel);
   /* NODE_SET_PROTOTYPE_METHOD(tpl, "setMinLevel", SetMinLevel);
    NODE_SET_PROTOTYPE_METHOD(tpl, "minLevel", MinLevel);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setLevelMod", SetLevelMod);
    NODE_SET_PROTOTYPE_METHOD(tpl, "levelMod", LevelMod);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getSimpleCovering", GetSimpleCovering);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getInteriorCellUnion", GetInteriorCellUnion);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getCellUnion", GetCellUnion);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getInteriorCovering", GetInteriorCovering);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getCovering", GetCovering);
	*/


  constructor.Reset(isolate, tpl);
  exports->Set(String::NewFromUtf8(isolate, "S2RegionCoverer"),
               tpl->GetFunction());
}
void RegionCoverer::New(const FunctionCallbackInfo<Value>& args) {
   Isolate* isolate = args.GetIsolate();
	if (!args.IsConstructCall()) {
		isolate->ThrowException(Exception::TypeError(
						String::NewFromUtf8(isolate, "Use the new operator to create instances of this object.")));
					return;
	}

	//RegionCoverer* obj = new RegionCoverer();
	args.GetReturnValue().Set(args.This());
}

v8::Local<Object> RegionCoverer::CreateNew(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    RegionCoverer* obj = new RegionCoverer();
    v8::Local<Value> ext = External::New(isolate,obj);
    v8::Local<Context> context = isolate->GetCurrentContext();
    v8::Local<FunctionTemplate> cons = v8::Local<FunctionTemplate>::New(isolate,constructor);
    v8::Local<Object> handleObject = cons->GetFunction()->NewInstance(context, 1, &ext).ToLocalChecked();
    return handleObject;
}

/*
void RegionCoverer::SetMaxLevel(const FunctionCallbackInfo<Value>& args) {
 	Isolate* isolate = args.GetIsolate();
    RegionCoverer* obj = node::ObjectWrap::Unwrap<RegionCoverer>(args.Holder());
	if(args.Length() != 1){
    	isolate->ThrowException(Exception::TypeError(
        	String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return
	}
 	if (!args[0]->IsNumber()) {
    	isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong arguments")));
    	return;
  	}
    obj->this_.set_max_level(args[0]->NumberValue());
}
void RegionCoverer::GetMaxLevel(const FunctionCallbackInfo<Value>& args) {
 	Isolate* isolate = args.GetIsolate();
    RegionCoverer* obj = ObjectWrap::Unwrap<RegionCoverer>(args.Holder());
    args.GetReturnValue().Set(Number::New(isolate,obj->this_.max_level()));
}
*/

string CellToString(const S2CellId& id) {
  return StringPrintf("%d:%s", id.level(), id.ToToken().c_str());
}

void RegionCoverer::GetCovering(const FunctionCallbackInfo<Value>& args) {
 	//Isolate* isolate = args.GetIsolate();
    //RegionCoverer* obj = ObjectWrap::Unwrap<RegionCoverer>(args.Holder());

	vector<S2CellId> covering;

	int argsLength = args.Length();
	S2RegionCoverer *cover = new S2RegionCoverer();
	
	if(argsLength >= 2 && args[1]->IsNumber())
		cover->set_min_level(args[1]->ToNumber()->Value());
	if(argsLength >= 3 && args[2]->IsNumber())
		cover->set_max_level(args[2]->ToNumber()->Value());
	if(argsLength >= 4 && args[3]->IsNumber())
		cover->set_max_cells(args[3]->ToNumber()->Value());
	if(argsLength >= 5 && args[4]->IsNumber())
		cover->set_level_mod(args[4]->ToNumber()->Value());

	Isolate* isolate = args.GetIsolate();
	Local<FunctionTemplate> latLngRect = Local<FunctionTemplate>::New(isolate,LatLngRect::constructor);

	Local<FunctionTemplate> cap = Local<FunctionTemplate>::New(isolate,Cap::constructor);

	Local<FunctionTemplate> cell = Local<FunctionTemplate>::New(isolate,Cell::constructor);

	Local<Object> fromObj = args[0]->ToObject();
	if(latLngRect->HasInstance(fromObj))
	{
		S2LatLngRect region = node::ObjectWrap::Unwrap<LatLngRect>(fromObj)->get();
		cover->GetCovering(region, &covering);
	}
	else if(cap->HasInstance(fromObj))
	{
		S2Cap region = node::ObjectWrap::Unwrap<Cap>(fromObj)->get();
		cover->GetCovering(region, &covering);
	}
	else if(cell->HasInstance(fromObj))
	{
		S2Cell region = node::ObjectWrap::Unwrap<Cell>(fromObj)->get();
		cover->GetCovering(region, &covering);
	}
  	Local<Array> cellIDs = Array::New(isolate, covering.size());


	for (int ii = 0; ii < covering.size(); ++ii) {
		cellIDs->Set(ii, CellId::CreateNew(args, covering[ii]));
	}

  	args.GetReturnValue().Set(cellIDs);
	
}

}