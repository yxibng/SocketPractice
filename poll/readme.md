# 运行程序
编译服务程序
```
clang++ -g -o server -std=c++11 server.cpp
./server
```
通过nc命令开几个客户端, 发送数据
```
nc localhost 3000
# input
```
`ctr+c`关闭客户程序，观察服务端输出

# poll

```
int poll(struct pollfd fds[], nfds_t nfds, int timeout);

struct pollfd {
    int    fd;       /* file descriptor */
    short  events;   /* events to look for */
    short  revents;  /* events returned */
};

```

参数：
- fds: struct pollfd 结构体数组（保存要检测的fd和希望检测的该fd上的事件）
- nfds： fds 数组的大小
- timeout：
    - =0 不阻塞，立马返回
    - >0 设置为超时事件（毫秒），规定事件内没有事件发生，就超时返回
    - -1 阻塞，直到有事件发生

返回值
- 正常，返回有多少个I/O就绪的fd
    - 此时遍历 fds 数组，读取 revents 对应的标志位，判断是否有事件发生，并获取事件详情
- 出错，-1，可以通过 errno 来获得错误信息。

## 缺点
（1）在调用poll函数时，不管有没有意义，大量 fd的数组在用户态和内核地址空间之间被整体复制；
（2）与 select 函数一样，poll 函数返回后，需要遍历 fd 集合来获取就绪的fd，这样会使性能下降；
（3）同时连接的大量客户端在某一时刻可能只有很少的就绪状态，因此随着监视的描述符数量的增长，其效率也会线性下降。

