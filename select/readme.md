# select 
```
int select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
    fd_set *restrict errorfds, struct timeval *restrict timeout);
```

参数：
- nfds：  待检测的fd中 最大的 fd + 1
- readfds： 待检测可读的fd_set指针， 建议每次重新初始化后传入
- writefds： 待检测可写的fd_set指针， 建议每次重新初始化后传入
- errorfds： 待检测异常信息的fd_set指针， 建议每次重新初始化后传入
- timeout： struct timeval 结构体指针，用来指定检测超时时间
    - null， 会阻塞直到有事件发生
    - 为0， 不阻塞立马返回
    - > 0, 等待超时或者有事件发生函数返回
    - 会被select函数修改，建议每次重新初始化后传入

返回值：
- 成功，返回指定的fd_set中就绪的fd数量
- 失败，-1， 根据errno 可以获取错误详情


## 操作fd_set 的宏
```
void
FD_CLR(fd, fd_set *fdset);

void
FD_COPY(fd_set *fdset_orig, fd_set *fdset_copy);

int
FD_ISSET(fd, fd_set *fdset);

void
FD_SET(fd, fd_set *fdset);

void
FD_ZERO(fd_set *fdset);
```

# 注意事项
1. select函数在调用前后可能会修改readfds、writefds和exceptfds这三个集合中的内容，所以如果想在下次调用select函数时复用这些fd_set变量，则要在下次调用前使用FD_ZERO将fd_set清零，然后调用FD_SET将需要检测事件的fd重新添加到fd_set中。
2. select函数也会修改timeval结构体的值，如果想复用这个变量，则必须给timeval变量重新设置值。在上面的例子中调用 select 函数一次后，变量 tv 的值也被修改，具体修改成多少，得看系统的表现。当然，这种特性不是跨平台的，在Linux上这样，在其他操作系统上却不一定这样（在 Windows上就不会修改这个结构体的值），这一点在 Linux man手册select函数的说明中说得很清楚。由于不同系统的实现可能不一样，man手册建议将 select 函数修改 timeval 结构体的行为当作未定义的，即下次使用 select 函数复用这个变量时，要重新赋值。这是使用select函数时需要注意的第2个地方。
3. select函数的timeval结构体的tv_sec和tv_usec如果都被设置为0，即检测事件的总时间被设置为0，则其行为是select检测相关集合中的fd，如果没有需要的事件，则立即返回。

# 缺点
1. select函数在调用前后可能会修改readfds、writefds和exceptfds这三个集合中的内容，所以如果想在下次调用select函数时复用这些fd_set变量，则要在下次调用前使用FD_ZERO将fd_set清零，然后调用FD_SET将需要检测事件的fd重新添加到fd_set中。
2. select函数也会修改timeval结构体的值，如果想复用这个变量，则必须给timeval变量重新设置值。在上面的例子中调用 select 函数一次后，变量 tv 的值也被修改，具体修改成多少，得看系统的表现。当然，这种特性不是跨平台的，在Linux上这样，在其他操作系统上却不一定这样（在 Windows上就不会修改这个结构体的值），这一点在 Linux man手册select函数的说明中说得很清楚。由于不同系统的实现可能不一样，man手册建议将 select 函数修改 timeval 结构体的行为当作未定义的，即下次使用 select 函数复用这个变量时，要重新赋值。这是使用select函数时需要注意的第2个地方。
3. select函数的timeval结构体的tv_sec和tv_usec如果都被设置为0，即检测事件的总时间被设置为0，则其行为是select检测相关集合中的fd，如果没有需要的事件，则立即返回。使用下面这段示例代码来验证一下