#include <node_api.h>
#include <stdio.h>

struct info
{
  int sum;
  napi_ref func;
  napi_async_work worker;
};

void work(napi_env env, void* data) {
  struct info *arg = (struct info *)data;
  printf("doing...\n");
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += i;
  }
  arg->sum = sum;
}

void done(napi_env env, napi_status status, void* data) {
  struct info *arg = (struct info *)data;
  if (status == napi_cancelled) {
    printf("cancel...");
  } else if (status == napi_ok) {
    printf("done...\n");
    napi_value callback;
    napi_value global;  
    napi_value result;
    napi_value sum;
    napi_create_int32(env, arg->sum, &sum);
    napi_get_reference_value(env, arg->func, &callback);
    napi_get_global(env, &global);
    napi_call_function(env, global, callback, 1, &sum, &result);
    napi_delete_reference(env, arg->func);
    napi_delete_async_work(env, arg->worker);
  }
}

static napi_value submitWork(napi_env env, napi_callback_info info) {
  napi_value resource_name;
  napi_status status;
  
  size_t argc = 1;
  napi_value args[1];
  struct info data = {0, nullptr, nullptr};
  struct info * ptr = &data;
  status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  if (status != napi_ok) {
    goto done;
  }
  napi_create_reference(env, args[0], 1, &ptr->func);
    
  // printf("%p %p\n", args[0], args[1]);
  // napi_make_callback(env,
  //                       nullptr,
  //                       ptr->recv,
  //                       ptr->func,
  //                       0 /* argc */, 
  //                       nullptr /* argv */, 
  //                       nullptr /* result */);
  //                       if (status != napi_ok) {
  //                         const napi_extended_error_info *error_info;    
  //                          napi_get_last_error_info(env, &error_info);
  //                          printf("xx%s", error_info->error_message);
  //                       }
  //                       return NULL;
  status = napi_create_string_utf8(env,"test", NAPI_AUTO_LENGTH, &resource_name);
  if (status != napi_ok) {
    goto done;
  }
  
  status = napi_create_async_work(env, nullptr, resource_name, work, done, (void *) ptr, &ptr->worker);
  if (status != napi_ok) {
    goto done;
  }
  status = napi_queue_async_work(env, ptr->worker);

  done: 
    napi_value ret;
    napi_create_int32(env, status == napi_ok ? 0 : -1, &ret);
    return  ret;
}

napi_value Init(napi_env env, napi_value exports) {
  napi_value func;
  napi_create_function(env,
                      NULL,
                      NAPI_AUTO_LENGTH,
                      submitWork,
                      NULL,
                      &func);
  napi_set_named_property(env, exports, "submitWork", func);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
