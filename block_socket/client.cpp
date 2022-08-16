#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 3000
#define SEND_DATA "helloworld"

int main(int argc, char *argv[]) {

  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd == -1) {
    perror("create socket error: ");
    return -1;
  }

  struct sockaddr_in serveraddr;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  serveraddr.sin_port = htons(SERVER_PORT);

  socklen_t serveraddrlen = sizeof(serveraddr);
  int ret =
      connect(socketfd, (const struct sockaddr *)&serveraddr, serveraddrlen);
  if (ret == -1) {
    perror("connect error: ");
    return -1;
  }

  // flags, 参考https://blog.csdn.net/yuanfengyun/article/details/50487475
  // 不断向服务器发送数据，直到出错并退出循环
  int count = 0;
  while (true) {
    int ret = send(socketfd, SEND_DATA, strlen(SEND_DATA), 0);
    if (ret == -1) {
      perror("send error: ");
      break;
    } else {
      if (ret != strlen(SEND_DATA)) {
        std::cout << "send msg to sever error." << std::endl;
        break;
      } else {
        count++;
        std::cout << "send data successfully, count =  " << count << std::endl;
      }
    }
  }

  close(socketfd);
  return 0;
}