#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <node.h>
#include <node_object_wrap.h>

using namespace node;
using namespace v8;
class Base: public ObjectWrap {
    public:
    
        static void New(const FunctionCallbackInfo<Value>& info) {
            // 新建一个对象，然后包裹到info.This()中，后面会解包出来使用
            Base* base =  new Base();
            base->Wrap(info.This());
        }

        static void Print(const FunctionCallbackInfo<Value>& info) {
            // 解包出来使用
            Base* base = ObjectWrap::Unwrap<Base>(info.This());
            base->print();
        }

        void print() {
            printf("base print\n");
        }

        void hello() {
            printf("base hello\n");
        }
};

#endif