#include <algorithm>
#include <assert.h>
#include <bitset>
#include <cmath>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <string>
#include <time.h>
#include <vector>

#include <errno.h>
#include <limits.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define H2O_USE_LIBUV 0

extern "C" {
  #include "h2o.h"
  #include "h2o/http1.h"
  #include "h2o/http2.h"
  #include "h2o/multithread.h"
}

static h2o_globalconf_t config;
static h2o_context_t ctx;
static h2o_accept_ctx_t accept_ctx;

static void on_accept(h2o_socket_t *listener, const char *err) {
  h2o_socket_t *sock;

  if (err != NULL) {
    return;
  }

  if ((sock = h2o_evloop_socket_accept(listener)) == NULL) {
    return;
  }

  h2o_accept(&accept_ctx, sock);
}


static int helloHandler(h2o_handler_t* handler, h2o_req_t* req) {
  std::cout << "helloHandler" << std::endl;
  std::cout << req->method.len << std::endl;
  // std::cout << req->method.base << std::endl;
  if (!h2o_memis(req->method.base, req->method.len, H2O_STRLIT("GET"))) {
    std::cout << "h2o_memis" << std::endl;
    return -1;
  }
  req->res.status = 200;
  req->res.reason = "OK";
  h2o_add_header(
    &req->pool,
    &req->res.headers,
    H2O_TOKEN_CONTENT_TYPE,
    NULL,
    H2O_STRLIT("text/plain")
  );
  h2o_send_inline(req, H2O_STRLIT("Hello, world\n"));

  return 0;
}

int main(int argc, char **argv) {

  h2o_config_init(&config);
  h2o_hostconf_t* hostconf = h2o_config_register_host(
    &config,
    h2o_iovec_init(H2O_STRLIT("default")),
    65535
  );

  h2o_pathconf_t *pathconf = h2o_config_register_path(hostconf, "/hello", 0);
  h2o_handler_t *handler = h2o_create_handler(pathconf, sizeof(*handler));
  handler->on_req = helloHandler;

  h2o_context_init(&ctx, h2o_evloop_create(), &config);
  accept_ctx.ctx = &ctx;
  accept_ctx.hosts = config.hosts;

  struct sockaddr_in addr;
  int fd, reuseaddr_flag = 1;
  h2o_socket_t *sock;

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(0x7f000001);
  addr.sin_port = htons(12345);

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ||
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_flag, sizeof(reuseaddr_flag)) != 0 ||
    bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0 || listen(fd, SOMAXCONN) != 0) {
    return -1;
  }

  sock = h2o_evloop_socket_create(ctx.loop, fd, H2O_SOCKET_FLAG_DONT_READ);
  h2o_socket_read_start(sock, on_accept);

  while (h2o_evloop_run(ctx.loop, INT32_MAX) == 0)
    ;

}

