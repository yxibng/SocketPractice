#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_PORT 3000
int main(int argc, char *argv[]) {

  // 1. 创建监听socket
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd == -1) {
    perror("create socket error: ");
    return -1;
  }
  // 2. 初始化服务器地址
  struct sockaddr_in bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bindaddr.sin_port = htons(SERVER_PORT);
  if (bind(listenfd, (const struct sockaddr *)&bindaddr, sizeof(bindaddr)) ==
      -1) {
    perror("bind error: ");
    return -1;
  }

  // 3. 启动监听
  if (listen(listenfd, 0) == -1) {
    perror("listen error: ");
    return -1;
  }

  while (true) {
    struct sockaddr_in clientaddr;
    socklen_t clientaddrlen = sizeof(clientaddr);
    // 4. 接受客户端连接
    int clientfd =
        accept(listenfd, (struct sockaddr *)&clientaddr, &clientaddrlen);
    if (clientfd == -1)
    {
      //只接受连接，不调用recv 接收任何数据
        perror("accpet error:");
        break;
    } else {
      std::cout << "new client connected, fd = " << clientfd << std::endl;
    }
  }

  close(listenfd);

  return 0;
}