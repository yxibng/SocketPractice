nc 使用

mac 下通过 `man nc` 查看使用介绍。

>    The nc (or netcat) utility is used for just about anything under the sun
     involving TCP or UDP.  It can open TCP connections, send UDP packets,
     listen on arbitrary TCP and UDP ports, do port scanning, and deal with both
     IPv4 and IPv6.  Unlike telnet(1), nc scripts nicely, and separates error
     messages onto standard error instead of sending them to standard output, as
     telnet(1) does with some.


## 模拟tcp/udp 客户端和服务器
1. tcp 服务器

使用`-l` 指定要监听的端口，模拟服务器程序

在本地监听1234 tcp 端口
```
nc -l 1234
```
tcp 客户端
```
# 随机端口
nc 127.0.0.1 1234
# 通过 -p 指定本地端口
nc -p 4321 127.0.0.1 1234
```

2. udp服务器
`nc`默认使用tcp连接，通过`-u` 指定使用`udp`协议。

```
# udp 监听
nc -ul 1234
# udp 客户端
nc -u 127.0.0.1 1234
```

更多可以参考：
[nc工具使用](https://www.cnblogs.com/zhaijiahui/p/9028402.html)


