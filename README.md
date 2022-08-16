# SocketPractice
socket practice in c++, select, poll, epoll, kqueue...

1. 通过man来查看socket相关的函数介绍, mac 平台下
```
man man 

MANUAL SECTIONS
       The standard sections of the manual include:

       1      User Commands

       2      System Calls

       3      C Library Functions

       4      Devices and Special Files

       5      File Formats and Conventions

       6      Games et. Al.

       7      Miscellanea

       8      System Administration tools and Deamons

       Distributions customize the manual section to their specifics, which
       often include additional sections.

```

soket 函数属于系统调用，因此对应的函数在section 2
```
man 2 socket

NAME
     socket – create an endpoint for communication

SYNOPSIS
     #include <sys/socket.h>

     int
     socket(int domain, int type, int protocol);

```
2. `void perror(const char *s);` 函数

 `man 2 bind`查看 bind 函数的返回值
 ```
RETURN VALUES
     Upon successful completion, a value of 0 is returned.  Otherwise, a value of -1 is returned and the global integer variable errno
     is set to indicate the error.
 ```
 如果出错，返回`-1`,并且全局的错误码`errno` 会被赋对应的值来标识错误。 可以用`perror`来打印对应的错误信息。

例如sever A 绑定了`127.0.0.1:3000`， sever B 尝试去绑定该地址，会绑定失败。通过`perror`打印错误信息为
```
Address already in use
```

相应的`bind`,`listen`, `accept`, `connect` 等调用出错，都可以用 `perror`打印错误信息
