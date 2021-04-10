#include <node.h>
/*
20.3 使用V8编写C++插件
N-API为我们封装了底层的一些差异以及提供了更友好、简单的API。当然我们也可以绕过他，直接使用V8编写插件。首先新建一个test.cc


*/
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

static int seq;
// 定义一个工具函数，生成seq
void GenSeq(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(Number::New(isolate, ++seq));
}

// 定义一个加法函数
void Add(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    int a = args[0].As<Int32>()->Value();
    int b = args[1].As<Int32>()->Value();
    args.GetReturnValue().Set(Number::New(isolate, a + b));
}

void Initialize(
  Local<Object> exports,
  Local<Value> module,
  Local<Context> context
) {
  Isolate* isolate = context->GetIsolate();
  // 新建一个函数模版
  Local<FunctionTemplate> func = FunctionTemplate::New(isolate);
  // 新建一个字符串表示函数名
  Local<String> zaylee = String::NewFromUtf8(isolate, "zaylee", v8::NewStringType::kNormal).ToLocalChecked();
  // 设置函数名
  func->SetClassName(zaylee);
  // 设置原型属性
  func->PrototypeTemplate()->Set(isolate, "protoField", Number::New(isolate, 1));
  // 设置对象属性
  func->InstanceTemplate()->Set(isolate, "instanceField", Number::New(isolate, 2));
  func->InstanceTemplate()->Set(isolate, "add", FunctionTemplate::New(isolate, Add));
  // 设置函数对象本身的属性
  func->Set(isolate, "funcField", Number::New(isolate, 3));
  // 根据函数模版创建一个函数
  Local<Function> ret = func->GetFunction(context).ToLocalChecked();
  Local<String> Demo = String::NewFromUtf8(isolate, "Demo", v8::NewStringType::kNormal).ToLocalChecked();
  // 导出函数
  exports->Set(context, Demo, ret).Check();
  // 导出工具函数
  NODE_SET_METHOD(exports, "genSeq", GenSeq);
}

NODE_MODULE_CONTEXT_AWARE(NODE_GYP_MODULE_NAME, Initialize)

}  // namespace demo