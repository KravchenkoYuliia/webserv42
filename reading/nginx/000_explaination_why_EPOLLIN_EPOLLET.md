Short answer: **Yes, `EPOLLIN | EPOLLOUT` is correct in most reactor designs.**
Registering **only `EPOLLOUT` would usually be wrong** for a connection handler.

Let’s break down why.

---

# 1. What `epoll_ctl(..., EPOLL_CTL_MOD, ...)` actually does

When you call something like:

```cpp
updateHandler(handler, EPOLLIN | EPOLLOUT);
```

you are **replacing the entire event mask** for that FD.

It does **not add** events. It **sets them**.

So if the socket was previously registered as:

```
EPOLLIN
```

and you do:

```
EPOLLOUT
```

then **EPOLLIN disappears**.

You would stop receiving read events.

---

# 2. Typical connection lifecycle

For a TCP connection handler:

### Initially

You usually register:

```cpp
EPOLLIN | EPOLLET
```

because you're waiting for a request.

---

### When you need to write

You enable write notifications:

```cpp
EPOLLIN | EPOLLOUT | EPOLLET
```

Why keep `EPOLLIN`?

Because the peer may still send data (pipelining, half-close, etc.).

Removing `EPOLLIN` can cause bugs like:

* unread client data
* missed disconnect events
* protocol desync

---

# 3. Your function logic

Your function:

```cpp
void Reactor::enableWrite(IEventHandler *handler)
{
    if (handler->isWritable())
        return;

    handler->setWritable(true);
    updateHandler(handler, EPOLLIN | EPOLLOUT);
}
```

Conceptually correct **if**:

```
isWritable() == EPOLLOUT registered
```

and

```
updateHandler() uses EPOLL_CTL_MOD
```

---

# 4. One important improvement

Right now you're **hardcoding the mask**:

```cpp
EPOLLIN | EPOLLOUT
```

But your reactor probably supports **multiple handler types**.

Better pattern:

```cpp
uint32_t events = EPOLLIN;

if (handler->isWritable())
    events |= EPOLLOUT;

updateHandler(handler, events);
```

Or more explicitly:

```cpp
void Reactor::enableWrite(IEventHandler *handler)
{
    if (handler->isWritable())
        return;

    handler->setWritable(true);

    uint32_t events = EPOLLIN | EPOLLOUT;
    updateHandler(handler, events);
}
```

---

# 5. Even better reactor design (used by nginx/libevent)

Handlers **do not know epoll flags**.

Instead they expose intent:

```
wantRead()
wantWrite()
```

Reactor computes flags:

```cpp
uint32_t events = 0;

if (handler->wantRead())
    events |= EPOLLIN;

if (handler->wantWrite())
    events |= EPOLLOUT;
```

This prevents subtle bugs.

---

# 6. One more subtle epoll rule (important)

If you're using **edge-triggered mode**:

```
EPOLLET
```

you must **always include it on MOD**:

```cpp
EPOLLIN | EPOLLOUT | EPOLLET
```

because `MOD` replaces the mask.

For example:

```cpp
updateHandler(handler, EPOLLIN | EPOLLOUT | EPOLLET);
```

Otherwise you accidentally switch to **level-triggered mode**.

---

# 7. Final verdict

Your code is **conceptually correct**:

```cpp
updateHandler(handler, EPOLLIN | EPOLLOUT);
```

and **not just `EPOLLOUT`**.

But if you're using **edge-triggered epoll**, the correct mask is likely:

```cpp
EPOLLIN | EPOLLOUT | EPOLLET
```

---

If you'd like, I can also show you a **very clean Reactor/Handler architecture used in production servers** that avoids:

* epoll flag duplication
* handler state bugs
* accidental missed events

It's about **20 lines of design change but makes the reactor much safer.**
