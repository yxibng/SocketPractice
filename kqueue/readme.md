# kqueue
kqueue 主要用于BSD 和 MAC 平台 异步 I/O 多路复用
linux 上对应的是 epoll
window上对应的是 IOCP

```
man kqueue
SYNOPSIS
     #include <sys/event.h>

     int kqueue(void);

     int kevent(int	kq, const struct kevent	*changelist, int nchanges,
	 struct	kevent *eventlist, int nevents,
	 const struct timespec *timeout);

     EV_SET(kev, ident,	filter,	flags, fflags, data, udata);

     struct kevent {
             uintptr_t       ident;          /* identifier for this event */
             int16_t         filter;         /* filter for event */
             uint16_t        flags;          /* general flags */
             uint32_t        fflags;         /* filter-specific flags */
             intptr_t        data;           /* filter-specific data */
             void            *udata;         /* opaque user data identifier */
     };
```


# 一般步骤
1. 创建kqueue
```
 kq = kqueue();
```
2. 注册事件

注册struct kevent 到 kqueue

```
struct kevent {
    uintptr_t	ident;	     /*	identifier for this event */
    short     filter;	     /*	filter for event */
    u_short   flags;	     /*	action flags for kqueue	*/
    u_int     fflags;	     /*	filter flag value */
    int64_t   data;	     /*	filter data value */
    void      *udata;	     /*	opaque user data identifier */
    uint64_t  ext[4];	     /*	extensions */
};
```
提供了宏方便初始化kevent

```
EV_SET(kev, ident,	filter,	flags, fflags, data, udata);

```
示例
```
struct kevent events[2];
EV_SET(&events[0], sk, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, obj);
EV_SET(&events[1], sk, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, obj);
kevent(kq, events, 2, NULL, 0, NULL);
```

struct kevent 成员解析
- ident： event 标识，通常是fd
- filter：监听的fd的事件类型，可读，可写...
    - EVFILT_READ
    - EVFILT_WRITE
    - EVFILT_EMPTY
    - EVFILT_AIO
    - EVFILT_VNODE
    - EVFILT_PROC
    - EVFILT_PROCDESC
    - EVFILT_SIGNAL
    - EVFILT_TIMER
    - EVFILT_USER
- flags: 操作动作， 添加、删除，开启，关闭等
    - EV_ADD
    - EV_ENABLE
    - EV_DISABLE
    - EV_DISPATCH
    - EV_DELETE
    - EV_RECEIPT
    - EV_ONESHOT
    - EV_CLEAR
    - EV_EOF
    - EV_ERROR
- fflags：针对filter设置一些标志
    - 例如 filter 为 EVFILT_READ，可以传递`NOTE_LOWAT`标记，通过`data`字段指定`new low water mark`
    - EVFILT_VNODE，可以指定一些列的flag， 查询man page 获取更多信息
        - NOTE_DELETE
        - NOTE_WRITE
        - ....

- data: 使用fflags， data 字段给filter 传值
- udata: 用户自定时的数据，上下文之类的？
3. 从kqueue中获取事件
```
while (!quit) {
    struct timespec *timeout = NULL; // wait indefinitely
    struct kevent events[1];
    int n = kevent(kq, NULL, 0, events, 1, &timeout);
    if (n < 0 && errno == EINTR)
        continue; // kevent() interrupts when UNIX signal is received
}
```
4. 处理事件
```
struct context *o = events[i].udata;
if (events[i].filter == EVFILT_READ)
    o->rhandler(o); // handle read event
else if (events[i].filter == EVFILT_WRITE)
    o->whandler(o); // handle write event
```

# 优点

kqueue 不仅能够处理文件描述符事件，还可以用于各种其他通知，例如文件修改监视、信号、异步 I/O 事件 (AIO)、子进程状态更改监视和支持纳秒级分辨率的计时器，此外kqueue提供了一种方式除了内核提供的事件之外，还可以使用用户定义的事件。

与select, poll 对比避免了多余的轮询， 操作效率更高。


# 参考： 
https://juejin.cn/post/7071118804855554061

这里有完成的kqueue教程 https://habr.com/en/post/600123/