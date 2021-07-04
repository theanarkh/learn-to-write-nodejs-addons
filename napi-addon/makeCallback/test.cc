#include <node_api.h>
#include <assert.h>
#include <stdio.h>
#define MAX_ARGUMENTS 10

static napi_value MakeCallback(napi_env env, napi_callback_info info) {
  size_t argc = MAX_ARGUMENTS;
  size_t n;
  napi_value args[MAX_ARGUMENTS];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  napi_value func = args[0];
  napi_value recv = args[1];
  napi_value argv[MAX_ARGUMENTS - 2];
  for (n = 2; n < argc; n += 1) {
    argv[n - 2] = args[n];
  }
  napi_value global;
  napi_get_global(env, &global);
  napi_status ret = napi_make_callback(env, nullptr, global, func, 0, nullptr, nullptr);
  
    if (ret != napi_ok) {
        const napi_extended_error_info *error_info = 0;
        napi_get_last_error_info(env, &error_info);
        printf("%s", error_info->error_message);
    }
  return nullptr;
}

static
napi_value Init(napi_env env, napi_value exports) {
  napi_value fn;
  napi_create_function(env, NULL, NAPI_AUTO_LENGTH, MakeCallback, NULL, &fn);
  napi_set_named_property(env, exports, "makeCallback", fn);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
