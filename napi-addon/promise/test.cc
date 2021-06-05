#include <node_api.h>
#include <unistd.h>

struct info
{
  napi_async_work worker;
  napi_deferred deferred;
  int sum;
};

void work(napi_env env, void* data) {
  struct info *arg = (struct info *)data;
  int sum = 0;
  for (int i = 0; i < 1000; i++) {
    sum += i;
  }
  arg->sum = sum;
}

void done(napi_env env, napi_status status, void* data) { 
  struct info *arg = (struct info *)data;
  napi_value ret;
  if (true) {
    napi_create_int32(env, arg->sum, &ret);
    napi_resolve_deferred(env, arg->deferred, ret);
  } else {
    napi_create_int32(env, 0, &ret);
    napi_reject_deferred(env, arg->deferred, ret);
  }
  napi_delete_async_work(env, arg->worker);
  arg->deferred = nullptr;
}

static napi_value getPromise(napi_env env, napi_callback_info info) {
  napi_value promise;
  napi_value resource_name;
  struct info data = {nullptr, nullptr, 0};
  struct info * ptr = &data;
  napi_create_promise(env, &ptr->deferred, &promise);
  napi_create_string_utf8(env,"test", NAPI_AUTO_LENGTH, &resource_name);
  napi_create_async_work(env, nullptr, resource_name, work, done, (void *)ptr, &ptr->worker);
  napi_queue_async_work(env, ptr->worker);

  return promise;
}

static napi_value isPromise(napi_env env, napi_callback_info info) {
  napi_value result;
  size_t argc = 1;
  napi_value argv[1];
  bool is_promise;

  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  napi_is_promise(env, argv[0], &is_promise);
  napi_get_boolean(env, is_promise, &result);

  return result;
}

napi_value Init(napi_env env, napi_value exports) {
  napi_value func;
  napi_create_function(env,
                      NULL,
                      NAPI_AUTO_LENGTH,
                      getPromise,
                      NULL,
                      &func);
  napi_set_named_property(env, exports, "getPromise", func);

  napi_value func2;
  napi_create_function(env,
                      NULL,
                      NAPI_AUTO_LENGTH,
                      isPromise,
                      NULL,
                      &func2);
  napi_set_named_property(env, exports, "isPromise", func2);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
