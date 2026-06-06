#ifndef CELLUNION_H
#define CELLUNION_H

#include <nan.h>
#include "s2.h"
#include "s2cell.h"
#include "s2cellid.h"
#include "s2cellunion.h"
#include "s2latlng.h"
#include "cellid.h"
#include "cell.h"
#include "point.h"
#include "cap.h"
#include "latlngrect.h"

namespace s2geo {

class CellUnion : public Nan::ObjectWrap {
public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static NAN_MODULE_INIT(Init);
    inline S2CellUnion& get() { return this_; }
    static v8::Local<v8::Object> CreateNew(const Nan::FunctionCallbackInfo<v8::Value>& info, CellUnion* obj);

protected:
    CellUnion();

    static NAN_METHOD(New);
    static NAN_METHOD(NumCells);
    static NAN_METHOD(GetCellId);
    static NAN_METHOD(CellIds);
    static NAN_METHOD(Contains);
    static NAN_METHOD(Intersects);
    static NAN_METHOD(GetUnion);
    static NAN_METHOD(GetIntersection);
    static NAN_METHOD(GetDifference);
    static NAN_METHOD(Normalize);
    static NAN_METHOD(Expand);
    static NAN_METHOD(GetCapBound);
    static NAN_METHOD(GetRectBound);
    static NAN_METHOD(LeafCellsCovered);
    static NAN_METHOD(AverageBasedArea);
    static NAN_METHOD(ApproxArea);
    static NAN_METHOD(ExactArea);
    static NAN_METHOD(ToString);

    S2CellUnion this_;
};

}
#endif
