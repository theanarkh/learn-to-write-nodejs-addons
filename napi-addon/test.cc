#include <node_api.h>
static napi_value newArray(napi_env env, napi_callback_info info) {
   size_t argc = 1;
   napi_value args[1];
   napi_get_cb_info(env, info, &argc, args, NULL, NULL);
   int len;
   napi_get_value_int32(env, args[0], &len);
   napi_value ret;
   napi_create_array(env, &ret);
   for (int i = 0; i < len; i++) {
     napi_value num;
     napi_create_int32(env, i, &num);
     napi_set_element(env, ret, i, num);
   }

  return ret;
}

napi_value Init(napi_env env, napi_value exports) {
  napi_value func;
  napi_create_function(env,
                      NULL,
                      NAPI_AUTO_LENGTH,
                      newArray,
                      NULL,
                      &func);
  napi_set_named_property(env, exports, "getArray", func);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
