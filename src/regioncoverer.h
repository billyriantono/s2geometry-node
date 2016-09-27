#ifndef REGIONCOVERER_HPP
#define REGIONCOVERER_HPP

#include <node.h>
#include <node_object_wrap.h>
#include "s2.h"
#include "s2regioncoverer.h"

namespace s2geo {
class RegionCoverer : public node::ObjectWrap {
public:
	RegionCoverer();
	static v8::Persistent<v8::FunctionTemplate> constructor;
    static void Init(v8::Local<v8::Object> exports);
   	static v8::Local<v8::Object> CreateNew(const v8::FunctionCallbackInfo<v8::Value>& args);

protected:
	 ~RegionCoverer();

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

	
	static void GetCovering(const v8::FunctionCallbackInfo<v8::Value>& args);
	/*
	static void MinLevel(const v8::FunctionCallbackInfo<v8::Value>& args);

	static void SetLevelMod(const v8::FunctionCallbackInfo<v8::Value>& args);
  	static void LevelMod(const v8::FunctionCallbackInfo<v8::Value>& args);

	static void GetSimpleCovering(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void GetInteriorCellUnion(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void GetCellUnion(const v8::FunctionCallbackInfo<v8::Value>& args);

	static void GetInteriorCovering(const v8::FunctionCallbackInfo<v8::Value>& args);

	static void GetCovering(const v8::FunctionCallbackInfo<v8::Value>& args);
	*/
};
}
#endif
