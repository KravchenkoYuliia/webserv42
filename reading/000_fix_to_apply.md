```c++
void Reactor::addHandler(IEventHandler *handler, uint32_t type)
{
    if (!handler)
        throw std::invalid_argument("handler is NULL");

    struct epoll_event event;
    event.events = type;
    event.data.ptr = handler;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handler->getFd(), &event) == -1)
        throw std::runtime_error("epoll_ctl ADD failed");

    try {
        handlers_.push_back(handler);
    }
    catch (...) {
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handler->getFd(), NULL);
        throw;
    }
}

Socket::Socket(const Socket& other)
    : fd_(-1),
      bind_port_(other.bind_port_)
{
    if (other.fd_ != -1)
    {
        fd_ = ::dup(other.fd_);
        if (fd_ == -1)
            throw std::runtime_error("dup failed");
    }
}

Socket& Socket::operator=(const Socket& other)
{
    if (this != &other)
    {
        // Close existing fd
        if (fd_ != -1)
            ::close(fd_);

        bind_port_ = other.bind_port_;

        if (other.fd_ != -1)
        {
            fd_ = ::dup(other.fd_);
            if (fd_ == -1)
                throw std::runtime_error("dup failed");
        }
        else
        {
            fd_ = -1;
        }
    }
    return *this;
}
```

