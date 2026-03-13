Yes — but the **exact names `wantRead()` / `wantWrite()` are not in nginx**.
Those are **a conceptual abstraction** people use when explaining nginx-style reactors.

In nginx itself, the idea appears as **separate read and write event objects attached to each connection**.

---

# Official nginx source / documentation

The nginx development guide explains that **each connection has two events**:

* `c->read`
* `c->write`

These represent interest in **read readiness** and **write readiness**. ([Docs4dev][1])

> “Each connection obtained by `ngx_get_connection()` has two attached events,
> `c->read` and `c->write`, used to receive notifications when the socket is ready.” ([Docs4dev][1])

Those events are instances of `ngx_event_t`.

---

# The real nginx structures

## `ngx_connection_t`

```c
typedef struct ngx_connection_s {
    void            *data;

    ngx_event_t     *read;
    ngx_event_t     *write;

    ngx_socket_t     fd;
} ngx_connection_t;
```

Each connection **always owns two events**.

Source explanation: ([blog.krybot.com][2])

---

## `ngx_event_t`

The event object contains flags describing the event state:

* `write` → whether this is a write event
* `active` → registered with epoll/kqueue
* `ready` → OS notified readiness
* `handler` → callback function

([Docs4dev][1])

Example fields:

```c
struct ngx_event_s {
    void                *data;
    ngx_event_handler_pt handler;

    unsigned write:1;
    unsigned active:1;
    unsigned ready:1;
};
```

---

# How nginx enables read/write

Instead of a `wantWrite()` method, nginx calls **helper functions**:

```c
ngx_handle_read_event(ngx_event_t *rev, ngx_uint_t flags);
ngx_handle_write_event(ngx_event_t *wev, ngx_uint_t flags);
```

These functions:

* register the event with epoll/kqueue
* or modify the mask if already active.

They are part of the core event system. ([DeepWiki][3])

---

# Example nginx-style flow

### enable write

```c
ngx_connection_t *c = ...;

ngx_handle_write_event(c->write, 0);
```

### enable read

```c
ngx_handle_read_event(c->read, 0);
```

---

# Conceptual mapping to your C++ reactor

Your design:

```cpp
handler->wantRead()
handler->wantWrite()
```

maps to nginx:

```
connection->read  event
connection->write event
```

And your reactor logic corresponds roughly to nginx’s:

```
ngx_handle_read_event()
ngx_handle_write_event()
```

---

# Real nginx source files you can inspect

If you want the **actual code**, these are the key files:

**Event core**

```
src/event/ngx_event.c
```

**Epoll implementation**

```
src/event/modules/ngx_epoll_module.c
```

**Event structures**

```
src/event/ngx_event.h
```

Example function inside nginx epoll module:

```
ngx_epoll_add_event()
```

which manipulates the epoll mask depending on read/write events.

---

# Why nginx uses two events instead of flags

This design allows nginx to:

* attach **different handlers** to read and write
* manage readiness separately
* avoid rebuilding epoll masks constantly
* support multiple backends (epoll, kqueue, IOCP)

Your `wantRead()/wantWrite()` abstraction is basically **a simplified version of nginx's `read` and `write` event objects**.

---

✅ **Best takeaway for your Webserv reactor**

Your idea should look like:

```cpp
if (handler->wantRead())
    events |= EPOLLIN;

if (handler->wantWrite())
    events |= EPOLLOUT;
```

which is conceptually equivalent to nginx managing:

```
connection->read->active
connection->write->active
```

---

If you'd like, I can also show you **a small excerpt from nginx's actual epoll code** (about 20 lines) that directly inspired most reactor implementations. It's very helpful when implementing a **42 Webserv**.

[1]: https://www.docs4dev.com/docs/en/nginx/current/reference/dev-development_guide.html?utm_source=chatgpt.com "Nginx"
[2]: https://blog.krybot.com/t/nginx-event-framework-summary/39085?utm_source=chatgpt.com "Nginx Event Framework Summary - Web Technologies - Krybot Blog"
[3]: https://deepwiki.com/nginx/nginx/2.2-event-processing-system?utm_source=chatgpt.com "Event Processing System | nginx/nginx | DeepWiki"
