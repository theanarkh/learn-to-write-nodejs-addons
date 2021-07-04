#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <liburing.h>
#include <stdlib.h>
#include <uv.h>
#include <node_api.h>

#define QUEUE_DEPTH 1
#define BLOCK_SZ    1024

// 管理一个文件读取请求的结构体
struct request {
    int fd;
    // 回调
    napi_env env;
    napi_ref func;
    // 字节大小
    int count;
    int offset;
    int buf_size;
    int nvecs;
    // 数据
    struct iovec iovecs[];     
};

// io_uring相关的结构体
struct io_uring_info {
  int fd;
  int32_t pending;
  struct io_uring ring;
  uv_poll_t poll_handle;
};

// io_uring完成任务后，Libuv执行的回调
void io_uring_done(uv_poll_t* handle, int status, int events) {
    
    struct io_uring* ring;
    struct io_uring_info* io_uring_data;
    struct io_uring_cqe* cqe;
    struct request* req;
    // 获取Libuv中保存的io_uring信息
    io_uring_data = (io_uring_info *)handle->loop->data;
    ring = &io_uring_data->ring;
    // 处理每一个完成的请求
    while (1) { 
        io_uring_peek_cqe(ring, &cqe);

        if (cqe == NULL)
            break;
        // 全部处理完则注销事件
        if (--io_uring_data->pending == 0)
           uv_poll_stop(handle);
        // 拿到请求上下文
        req = (struct request *) (uintptr_t) cqe->user_data;
        // 记录读取的大小
        req->count = cqe->res;

        io_uring_cq_advance(ring, 1);
        napi_handle_scope scope;
        napi_open_handle_scope(req->env, &scope);
        // 执行回调
        napi_value global;
        napi_get_global(req->env, &global);
        napi_value argv[2];
        napi_get_null(req->env, &argv[0]);
        napi_create_int32(req->env, req->count, &argv[1]);
        napi_value callback;
        
        napi_get_reference_value(req->env, req->func, &callback);
        
        napi_status ret = napi_call_function(req->env, global, callback, 2, argv, nullptr);
        napi_delete_reference(req->env, req->func);
        if (ret != napi_ok) {
            const napi_extended_error_info *error_info;                          
            napi_get_last_error_info((req->env), &error_info);  
            printf("%s", error_info->error_message);
        }
        napi_close_handle_scope(req->env, scope);
        free(req);
    }
}

// 向内核提交一个请求
int submit_read_request(int op, struct request* req, struct io_uring *ring) {
    // 获取一个io_uring的请求结构体
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    // 填充请求
    io_uring_prep_readv(sqe, req->fd, req->iovecs, req->nvecs, req->offset);
    // 保存请求上下文，响应的时候用
    io_uring_sqe_set_data(sqe, (void *)req);
    uv_loop_t* loop;
    napi_get_uv_event_loop(req->env, &loop);
    struct io_uring_info *io_uring_data = (io_uring_info *)loop->data;
    io_uring_data->pending++;
    if (io_uring_data->pending == 1)
        // 注册事件和回调
        uv_poll_start(&io_uring_data->poll_handle, UV_READABLE, io_uring_done);
    // 提交请求给内核
    io_uring_submit(ring);

    return 0;
}



static napi_value read(napi_env env, napi_callback_info info) {
  size_t argc = 5;
  napi_value args[5];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  int32_t fd;
  napi_get_value_int32(env, args[0], &fd);
  char *bufferData;
  size_t bufferLength;
  napi_get_buffer_info(
                env,
                args[1],
                (void**)(&bufferData),
                &bufferLength);
  int32_t offset;
  napi_get_value_int32(env, args[3], &offset);
  
    uv_loop_t* loop;
    napi_get_uv_event_loop(env, &loop);
    struct io_uring_info *io_uring_data = (io_uring_info *)loop->data;
    // 申请内存
    struct request *req = (struct request *)malloc(sizeof(*req) + (sizeof(struct iovec) * 1));
    req->fd = fd;
    req->offset = offset;
    // 保存回调
    napi_create_reference(env, args[2], 1, &req->func);
    req->env = env;
    req->nvecs = 1;
    // 记录buffer大小
    req->iovecs[0].iov_len = bufferLength;
    // 记录内存地址
    req->iovecs[0].iov_base = bufferData;

    submit_read_request(IORING_OP_READV, req, &io_uring_data->ring);              
  return nullptr;
}

napi_value Init(napi_env env, napi_value exports) {
    uv_loop_t* loop;
    napi_get_uv_event_loop(env, &loop);
    // 申请一个io_uring相关的结构体
    struct io_uring_info *io_uring_data = (struct io_uring_info *)malloc(sizeof(*io_uring_data));
    // 初始化io_uring
    io_uring_queue_init(1, &io_uring_data->ring, 0);
    // 初始化poll handle，保存监听的fd
    uv_poll_init(loop, &io_uring_data->poll_handle, io_uring_data->ring.ring_fd);

    // 保存io_uring的上下文在loop中
    loop->data = (void *)io_uring_data;
  napi_value func;
  napi_create_function(env,
                      NULL,
                      NAPI_AUTO_LENGTH,
                      read,
                      NULL,
                      &func);
  napi_set_named_property(env, exports, "read", func);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)