#include <algorithm>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <stdio.h>

#define SERVER_PORT 3000
#define INVALID_FD -1
#define SERVER_ADDRESS "127.0.0.1"
#define MAX_CLIENTS 10

int conver2nonblockfd(int fd) {
  int oldflag = fcntl(fd, F_GETFL);
  int newflag = oldflag | O_NONBLOCK;
  int ret = fcntl(fd, F_GETFL, newflag);
  if (ret == -1) {
    perror("fcntl error: ");
    return -1;
  }
  return 0;
}

int main(int argc, char *argv[]) {

  int listenfd = socket(AF_INET, SOCK_STREAM, 0);

  //设置listenfd为nonblock
  int ret = conver2nonblockfd(listenfd);
  if (ret == -1) {
    std::cout << "set listenfd nonblock error." << std::endl;
    return -1;
  }

  //调用bind
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SERVER_PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  ret = bind(listenfd, (const struct sockaddr *)&addr, sizeof(addr));
  if (ret == -1) {
    perror("bind error:");
    return -1;
  }

  ret = listen(listenfd, SOMAXCONN);
  if (ret == -1) {
    perror("listen error:");
    return -1;
  }

  struct pollfd fd;
  fd.fd = listenfd;
  fd.events = POLLIN;

  std::vector<pollfd> fds;
  fds.push_back(fd);

  while (true) {
    //timeout = -1, poll 会一直阻塞到有事件到来
    int ret = poll(&fds[0], fds.size(), -1);
    if (ret == 0) {
      // timeout
      std::cout << "poll time out." << std::endl;
      continue;
    } else if (ret == -1) {
      if (errno == EINTR || errno == EAGAIN) {
        perror("retry poll: ");
        continue;
      } else {
        perror("poll error");
        break;
      }
    }

    assert(ret > 0);

    for (auto &&fd : fds) {
      if (fd.fd == listenfd) {
        if (fd.revents == POLLIN) {
          //有新连接到来
          sockaddr_in clientAddr;
          socklen_t len;
          int clientfd = accept(listenfd, (struct sockaddr *)&clientAddr, &len);
          if (clientfd == -1) {
            if (errno == EINTR) {
              std::cout << "accept interrupted by signal." << std::endl;
              continue;
            } else {
              perror("accept error: ");
              break;
            }
          }
    
          assert(clientfd > 0);
          std::cout << "new client connect fd: " << clientfd << std::endl;
          struct pollfd clientpollfd;
          clientpollfd.fd = clientfd;
          clientpollfd.events = POLLIN;

          int ret = conver2nonblockfd(clientfd);
          if (ret == -1) {
            std::cout << "set clientfd nonblock error." << std::endl;
            return -1;
          }
          //将fd加入到数组，下次循环通过poll加入到监听事件列表
          fds.push_back(clientpollfd);
          break;
        } else {
          continue;
        }
      }
      // recv

      if (fd.fd == INVALID_FD) {
        continue;
      }
      char buf[128] = {0};
      int receiveBytes = recv(fd.fd, buf, 128, 0);
      if (receiveBytes == 0) {
        fd.fd = INVALID_FD;
        continue;
      } else if (receiveBytes == -1) {
        if (errno == EINTR) {
          std::cout << "recv interrupted by signal. fd: " << fd.fd << std::endl;
          continue;
        } else {
          std::cout << "recv data error. fd: " << fd.fd << std::endl;
          fd.fd = INVALID_FD;
          continue;
        }
      } else {
        std::cout << "recv data from fd: " << fd.fd << " data: " << buf
                  << std::endl;
        continue;
      }
    }
    // clear invalid fds
    auto start = std::remove_if(fds.begin(), fds.end(), [](struct pollfd afd) {
      return afd.fd == INVALID_FD;
    });
    fds.erase(start, fds.end());
  }

  close(listenfd);

  return 0;
}
