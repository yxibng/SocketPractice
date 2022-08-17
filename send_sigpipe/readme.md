## 对端socket 关闭了接收通道，本端继续调用send发送数据，会触发sigpipe信号，导致程序退出

通常的处理
1. 忽略这个SIGPIPE信号
```
struct sigaction sa;
sa.sa_handler = SIG_IGN;
sigaction( SIGPIPE, &sa, 0 );
```
2. 修改send 函数的第四个参数， 将flags参数设置为MSG_NOSIGNAL
```
res = send (client->sock_fd,  ptr,  sendlen, MSG_NOSIGNAL);
```

参考： https://blog.csdn.net/luxgang/article/details/81217289

## 验证

1. 起一个服务器程序,监听3000端口
```
nc -l 3000
```
2. 客户端不断向服务器发送数据
```
# 编译客户端
clang++ -g -o client -std=c++11 client.cpp 
# 启动客户端
./client
```
3. `ctr+c`关闭服务程序，观察客户端触发了SIGPIPE，此时 `errno == EPIPE`