#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define INVALID_FD -1
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 3000

int main(int argc, char *argv[]) {

  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd == -1) {

    std::cout << "create listen socket error." << std::endl;
    return -1;
  }

  //初始化服务器
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(3000);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  int ret = bind(listenfd, (const struct sockaddr *)&addr, sizeof(addr));
  if (ret == -1) {
    perror("bind error: ");
    close(listenfd);
    return -1;
  }

  ret = listen(listenfd, SOMAXCONN);
  if (ret == -1) {
    perror("listen error: ");
    close(listenfd);
    return -1;
  }

  std::vector<int> clientfds;

  int maxfd;

  while (true) {

    /*
    select 会修改传入的fd_set
    每次循环，调用FD_ZERO将readset置空，随后将待检测的fd重新加入到fd_set
    */
    fd_set readset;
    FD_ZERO(&readset);
    //将监听socket 加入待检测的fd_set
    FD_SET(listenfd, &readset);
    maxfd = listenfd;
    //将客户端fd加入待检测的可读事件中
    int clientfdslength = clientfds.size();
    for (const auto fd : clientfds) {
      if (fd != INVALID_FD) {
        FD_SET(fd, &readset);
        if (maxfd < fd) {
          maxfd = fd;
        }
      }
    }
    //设置超时时间1秒
    timeval tm;
    tm.tv_sec = 1;
    tm.tv_usec = 0;
    //暂时只检测可读事件，不检测可写和异常事件
    int ret = select(maxfd + 1, &readset, NULL, NULL, &tm);
    if (ret == -1) {
      perror("select error: ");
      if (errno != EINTR) {
        break;
      }
    } else if (ret == 0) {
      // select 函数超时，下次继续
      std::cout << "time out." << std::endl;
      continue;
    } else {
      //检测到某个socket有事件
      if (FD_ISSET(listenfd, &readset)) {
        //监听到socket的可读事件，表明有新的连接到来
        struct sockaddr_in clientaddr;
        socklen_t clientaddrlength = sizeof(clientaddr);
        //接收客户端连接
        int clientfd =
            accept(listenfd, (sockaddr *)&clientaddr, &clientaddrlength);

        if (clientfd == INVALID_FD) {
          //连接出错， 退出程序
          perror("accept error: ");
          break;
        } else {
          //将fd加入clientfds中，下次循环传递给select检测可读事件
          std::cout << "accept a client connection, fd: " << clientfd
                    << std::endl;
          clientfds.push_back(clientfd);
        }

      } else {
        //假设对端发来的数据长度不超过64个字符
        char buf[64];
        for (auto &fd : clientfds) {
          if (fd != INVALID_FD && FD_ISSET(fd, &readset)) {
            memset(buf, 0, sizeof(buf));
            //非监听socket，接收数据
            int length = recv(fd, buf, 64, 0);
            if (length <= 0) {
              //接收数据出错
              std::cout << "recv data error, clientfd: " << fd << std::endl;
              close(fd);
              //不直接删除该元素， 将该位置的元素标记为INVALID_FD
              fd = INVALID_FD;
              continue;
            }
            std::cout << "clientfd: " << fd << ", recv data: " << buf
                      << std::endl;
          }
        }
      }
    }
    for (const auto fd : clientfds) {
      if (fd != INVALID_FD) {
        close(fd);
      }
    }
  }

  close(listenfd);
  return 0;
}