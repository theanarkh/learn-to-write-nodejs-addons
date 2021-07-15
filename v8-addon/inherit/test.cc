#include <node.h>
#include "Base.h"
#include "Derived.h"

namespace demo {

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::FunctionTemplate;
using v8::Function;
using v8::Number;
using v8::MaybeLocal;
using v8::Context;
using v8::Int32;
using v8::NewStringType;

void Initialize(
  Local<Object> exports,
  Local<Value> module,
  Local<Context> context
) {
  Isolate * isolate = context->GetIsolate();
  // 新建两个函数模板，基类和子类，js层New导出的函数时，V8会执行New函数并传入一个对象
  Local<FunctionTemplate> base = FunctionTemplate::New(isolate, Base::New);
  Local<FunctionTemplate> derived = FunctionTemplate::New(isolate, Derived::New);
  
  // js层使用的类名
  NewStringType type = NewStringType::kNormal;
  Local<String> base_string = String::NewFromUtf8(isolate, "Base", type).ToLocalChecked();
  Local<String> derived_string = String::NewFromUtf8(isolate, "Derived", type).ToLocalChecked();
  
  // 预留一个指针空间
  base->InstanceTemplate()->SetInternalFieldCount(1);
  derived->InstanceTemplate()->SetInternalFieldCount(1);

  // 定义两个函数模板，用于属性的值
  Local<FunctionTemplate> BasePrint = FunctionTemplate::New(isolate, Base::Print);
  Local<FunctionTemplate> Hello = FunctionTemplate::New(isolate, Derived::Hello);
  
  // 给基类定义一个print函数
  base->PrototypeTemplate()->Set(isolate, "print", BasePrint);
  // 给子类定义一个hello函数
  derived->PrototypeTemplate()->Set(isolate, "hello", Hello);
  // 建立继承关系
  derived->Inherit(base);
  // 导出两个函数给js层
  exports->Set(context, base_string, base->GetFunction(context).ToLocalChecked()).Check();
  exports->Set(context, derived_string, derived->GetFunction(context).ToLocalChecked()).Check();
}

NODE_MODULE_CONTEXT_AWARE(NODE_GYP_MODULE_NAME, Initialize)

}
