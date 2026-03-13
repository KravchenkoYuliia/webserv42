Here is a **real excerpt from nginx’s epoll implementation** and how it manages read/write interest. I’ll show a simplified piece from **NGINX** source so you can see how the design inspired most reactors.

Source file:

```
src/event/modules/ngx_epoll_module.c
```

---

# 1. Core nginx epoll function

This function **adds or modifies interest in an event**.

```c
static ngx_int_t
ngx_epoll_add_event(ngx_event_t *ev, ngx_int_t event, ngx_uint_t flags)
{
    int                  op;
    uint32_t             events, prev;
    ngx_event_t         *e;
    ngx_connection_t    *c;
    struct epoll_event   ee;

    c = ev->data;

    if (event == NGX_READ_EVENT) {
        e = c->write;
        prev = EPOLLOUT;
        events = EPOLLIN | EPOLLRDHUP;
    } else {
        e = c->read;
        prev = EPOLLIN | EPOLLRDHUP;
        events = EPOLLOUT;
    }

    if (e->active) {
        op = EPOLL_CTL_MOD;
        events |= prev;
    } else {
        op = EPOLL_CTL_ADD;
    }

    ee.events = events | EPOLLET;
    ee.data.ptr = (void *) ((uintptr_t) c | ev->instance);

    if (epoll_ctl(epoll_fd, op, c->fd, &ee) == -1) {
        return NGX_ERROR;
    }

    ev->active = 1;

    return NGX_OK;
}
```

---

# 2. What this code is doing

Important parts.

### Determine which event we enable

```c
if (event == NGX_READ_EVENT)
```

nginx internally defines:

```
NGX_READ_EVENT
NGX_WRITE_EVENT
```

---

### Determine existing event

```c
e = c->write;
```

If enabling read, nginx checks if **write is already active**.

---

### If another event already exists → modify

```c
if (e->active)
    op = EPOLL_CTL_MOD;
```

Otherwise:

```
EPOLL_CTL_ADD
```

---

### Build epoll mask

Example when enabling read:

```
EPOLLIN | EPOLLRDHUP
```

If write already active:

```
EPOLLIN | EPOLLRDHUP | EPOLLOUT
```

So nginx **reconstructs the mask based on read/write state**.

---

# 3. Why this design is powerful

Each connection has two independent event objects:

```
connection
   ├── read event
   └── write event
```

Each event tracks:

```
active
ready
handler
```

The reactor backend **rebuilds the epoll mask** automatically.

Handlers never touch epoll flags.

---

# 4. Dispatching events in nginx

When epoll returns events, nginx dispatches like this:

```c
if (revents & EPOLLIN) {
    rev->ready = 1;
    rev->handler(rev);
}

if (revents & EPOLLOUT) {
    wev->ready = 1;
    wev->handler(wev);
}
```

So nginx calls **different callbacks** for read and write.

---

# 5. Mapping nginx → your C++ design

Your simplified version:

| nginx                     | your reactor      |
| ------------------------- | ----------------- |
| `c->read->active`         | `wantRead()`      |
| `c->write->active`        | `wantWrite()`     |
| `ngx_handle_read_event()` | `updateHandler()` |
| `rev->handler()`          | `handleRead()`    |
| `wev->handler()`          | `handleWrite()`   |

So your abstraction is basically a **C++ version of nginx’s event model**.

---

# 6. The most interesting nginx trick

Notice this line:

```c
ee.data.ptr = (void *) ((uintptr_t) c | ev->instance);
```

nginx **packs connection pointer + event instance bits** into the epoll pointer.

This avoids:

* stale events after fd reuse
* race conditions
* extra lookups

It's one of the reasons nginx’s event loop is extremely fast.

---

# 7. Key files worth reading

If you're implementing a serious reactor, the most educational nginx files are:

```
src/event/ngx_event.c
src/event/ngx_event.h
src/event/modules/ngx_epoll_module.c
src/core/ngx_connection.c
```

---

✅ **If you're building a 42 Webserv**, I can also show you **a 120-line minimal nginx-style reactor implementation in C++98** that:

* uses epoll edge-triggered
* supports acceptor + connections
* handles read/write interest like nginx

It's basically the **cleanest architecture for this project**.
