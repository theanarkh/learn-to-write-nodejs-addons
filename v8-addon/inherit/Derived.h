#ifndef DERIVED_H
#define DERIVED_H

#include <node.h>
#include <node_object_wrap.h>
#include"Base.h"

using namespace node;
using namespace v8;
class Derived: public Base {
    public:
        
        static void New(const FunctionCallbackInfo<Value>& info) {
            Derived* derived =  new Derived();
            derived->Wrap(info.This());
        }

        static void Hello(const FunctionCallbackInfo<Value>& info) {
            Derived* derived = ObjectWrap::Unwrap<Derived>(info.This());
            // 调用基类的函数
            derived->hello();
        }
};

#endif