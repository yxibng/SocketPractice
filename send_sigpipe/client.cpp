#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 3000
#define SEND_DATA "helloworld"

int main(int argc, char *argv[]) {

  struct sigaction sa;
  sa.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &sa, 0);

  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd == -1) {
    std::cout << "create socket error." << std::endl;
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
    perror("connect error:");
    return -1;
  }

  //连接成功后，将clientfd 设置为非阻塞模式
  //不能在创建时就设置，这样会影响到connet函数的行为
  int oldSocketFlag = fcntl(socketfd, F_GETFL, 0);
  int newSocketFlag = oldSocketFlag | O_NONBLOCK;
  if (fcntl(socketfd, F_SETFL, newSocketFlag) == -1) {
    close(socketfd);
    perror("fcntl error: ");
    return -1;
  }

  // flags, 参考https://blog.csdn.net/yuanfengyun/article/details/50487475
  // 不断向服务器发送数据，直到出错并退出循环
  int count = 0;
  while (true) {
    ssize_t ret = send(socketfd, SEND_DATA, strlen(SEND_DATA), 0);
    if (ret == -1) {
      //非阻塞模式下，send 函数由于tcp 窗口太小发不出去数据， 错误码是
      // EWOULDBLOCK
      if (errno == EWOULDBLOCK) {
        perror("send error: ");
        continue;
      } else if (errno == EINTR) {
        //如果信号被中断， 则继续重试
        perror("send error:");
        continue;
      } else if (errno == EPIPE) {
        //对面关闭了连接，继续调用send会触发SIGPIPE，默认退出程序
        perror("send error:");
        break;
      } else {
        perror("send error:");
        break;
      }
    } else {
      count++;
      sleep(1);
      std::cout << "send data successfully, count =  " << count << std::endl;
    }
  }
  close(socketfd);
  return 0;
}
