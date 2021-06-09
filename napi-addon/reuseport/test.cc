#include <node_api.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <uv.h>


int uv__cloexec(int fd, int set) {
  int flags;
  int r;

  do
    r = fcntl(fd, F_GETFD);
  while (r == -1 && errno == EINTR);

  if (r == -1)
    return (errno);

  /* Bail out now if already set/clear. */
  if (!!(r & FD_CLOEXEC) == !!set)
    return 0;

  if (set)
    flags = r | FD_CLOEXEC;
  else
    flags = r & ~FD_CLOEXEC;

  do
    r = fcntl(fd, F_SETFD, flags);
  while (r == -1 && errno == EINTR);

  if (r)
    return (errno);

  return 0;
}

int uv__nonblock(int fd, int set) {
  int flags;
  int r;

  do
    r = fcntl(fd, F_GETFL);
  while (r == -1 && errno == EINTR);

  if (r == -1)
    return (errno);

  /* Bail out now if already set/clear. */
  if (!!(r & O_NONBLOCK) == !!set)
    return 0;

  if (set)
    flags = r | O_NONBLOCK;
  else
    flags = r & ~O_NONBLOCK;

  do
    r = fcntl(fd, F_SETFL, flags);
  while (r == -1 && errno == EINTR);

  if (r)
    return (errno);

  return 0;
}


int uv__socket(int domain, int type, int protocol) {
  int sockfd;
  int err;

#if defined(SOCK_NONBLOCK) && defined(SOCK_CLOEXEC)
  sockfd = socket(domain, type | SOCK_NONBLOCK | SOCK_CLOEXEC, protocol);
  if (sockfd != -1)
    return sockfd;

  if (errno != EINVAL)
    return (errno);
#endif

  sockfd = socket(domain, type, protocol);
  if (sockfd == -1)
    return (errno);

  err = uv__nonblock(sockfd, 1);
  if (err == 0)
    err = uv__cloexec(sockfd, 1);

  if (err) {
    return err;
  }

#if defined(SO_NOSIGPIPE)
  {
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on));
  }
#endif
  struct sockaddr_in saddr;
  uv_ip4_addr("127.0.0.1", 8888, &saddr);
  bind(sockfd, (struct sockaddr*) &saddr, sizeof(saddr));
  int on = 1;
  u_int32_t ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
  return sockfd;
}


static napi_value Socket(napi_env env, napi_callback_info info) {
  int32_t fd = uv__socket(AF_INET, SOCK_STREAM, 0);
  napi_value ret;
  napi_create_int32(env, fd, &ret);
  return ret;
}
napi_value Init(napi_env env, napi_value exports) {
  napi_value func;
  napi_create_function(env,
                      NULL,
                      NAPI_AUTO_LENGTH,
                      Socket,
                      NULL,
                      &func);
  napi_set_named_property(env, exports, "socket", func);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
