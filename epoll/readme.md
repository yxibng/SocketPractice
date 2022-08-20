
源码参考地址：https://github.com/onestraw/epoll-example/blob/master/epoll.c
# epoll
linux 平台的异步I/O函数，mac没有epoll，对应的是kqueue

基本用法
```
#include <sys/epoll.h>
//创建epoll
int epoll_create(int size);int epoll_create1(int flags);
//向epoll注册事件，管理注册的事件
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

//等待事件发生
int epoll_wait(int epfd, struct epoll_event *events,
               int maxevents, int timeout);
//等待事件发生
int epoll_pwait(int epfd, struct epoll_event *events,
               int maxevents, int timeout,
               const sigset_t *sigmask);
```

用法在`man page`里面已经很清楚了
```
#define MAX_EVENTS 10
struct epoll_event ev, events[MAX_EVENTS];
int listen_sock, conn_sock, nfds, epollfd;

/* Set up listening socket, 'listen_sock' (socket(),
   bind(), listen()) */

epollfd = epoll_create(10);
if (epollfd == -1) {
    perror("epoll_create");
    exit(EXIT_FAILURE);
}

ev.events = EPOLLIN;
ev.data.fd = listen_sock;
if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
    perror("epoll_ctl: listen_sock");
    exit(EXIT_FAILURE);
}

for (;;) {
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
        perror("epoll_pwait");
        exit(EXIT_FAILURE);
    }

   for (n = 0; n < nfds; ++n) {
        if (events[n].data.fd == listen_sock) {
            conn_sock = accept(listen_sock,
                            (struct sockaddr *) &local, &addrlen);
            if (conn_sock == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            setnonblocking(conn_sock);
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = conn_sock;
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
                        &ev) == -1) {
                perror("epoll_ctl: conn_sock");
                exit(EXIT_FAILURE);
            }
        } else {
            do_use_fd(events[n].data.fd);
        }
    }
}

```

# 水平触发，边缘触发

（1）对于水平触发模式，一个事件只要有，就会一直触发；
（2）对于边缘触发模式，在一个事件从无到有时才会触发


以 socket 的读事件为例，对于水平模式，只要在 socket 上有未读完的数据，就会一直产生EPOLLIN事件；而对于边缘模式，socket上每新来一次数据就会触发一次，如果上一次触发后未将socket上的数据读完，也不会再触发，除非再新来一次数据。对于socket写事件，如果socket的TCP窗口一直不饱和，就会一直触发EPOLLOUT事件；而对于边缘模式，只会触发一次，除非 TCP 窗口由不饱和变成饱和再一次变成不饱和，才会再次触发EPOLLOUT事件。
socket可读事件的水平模式触发条件：①socket上无数据→socket上有数据；②socket处于有数据状态。
socket可读事件的边缘模式触发条件：①socket上无数据→socket上有数据；②socket又新来一次数据。
socket可写事件的水平模式触发条件：①socket可写→socket不可写；②socket不可写→ socket可写。
socket可写事件的边缘模式触发条件：socket不可写→socket可写。
也就是说，对于一个非阻塞 socket，如果使用 epoll边缘模式检测数据是否可读，则触发可读事件后，一定要一次性地把 socket上的数据收取干净。也就是说，一定要循环调用 recv函数直到recv出错，错误码是 EWOULDBLOCK（EAGAIN也一样，此时表示socket 上的本次数据已经读完）；如果使用水平模式，则我们可以根据业务一次性地收取固定的字节数，或者到收完为止。


# mac上用docker运行linux容器来调试epoll

编译docker镜像
```
docker build -t="myubuntu" ./
```
运行docker 容器
```
docker run -it -p 3000:3000 -v $SocketPractice:/usr/codes/SocketPractice myubuntu /bin/bash
```

# 编译程序
```
clang -o epoll epoll.c
```

# 运行服务器程序
```
# 启动服务程序
./epoll -s 
# 启动客户程序 ./epoll -c
```
在mac上打开终端，用`nc`命令开几个客户端,连接服务器，发送消息
```
nc 127.0.0.1 3000
...
```




