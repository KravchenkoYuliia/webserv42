```c++
void ConnectionHandler::handleRead()
{
    char buffer[8192];
    std::cout << "EPOLLIN case" << std::endl;
    // read the request until recv == 0
    while (true)
    {
        ssize_t bytes_received = recv(fd_, buffer, sizeof(buffer), 0);
        if (bytes_received == 0)
        {
            reactor_.deleteHandler(fd_);
            return ;
        }
        else if (bytes_received < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break ;
            reactor_.deleteHandler(fd_);
            return ;
        }
        else if (bytes_received > 0)
        {
            // save bytes into a read_buffer
            request_.appendData(buffer, bytes_received);
            RequestParser::ResultType result = request_.parseNext();
            if (result == RequestParser::ParserResult::ERROR)
            {
                reactor_.deleteHandler(fd_);
                return ;
            }
            else if (result == RequestParser::ParserResult::AGAIN)
                continue;
            else if (result == RequestParser::ParserResult::OK)
            {
                std::cout << buffer << std::endl;
                reactor_.updateHandler(this, EPOLLOUT);
            }
        }
    }
}
```

```c++
if (request_.isComplete())
{
    ResponseBuilder builder;
    response_ = builder.build(request_.getRequest());

    write_buffer_ = response_.serialize();
    write_offset_ = 0;

    reactor_.updateHandler(this, EPOLLOUT | EPOLLET);
    return;
}

void ConnectionHandler::handleWrite()
{
    while (write_offset_ < write_buffer_.size())
    {
        ssize_t bytes_sent = send(
            fd_,
            write_buffer_.data() + write_offset_,
            write_buffer_.size() - write_offset_,
            0
        );

        if (bytes_sent > 0)
        {
            write_offset_ += bytes_sent;
        }
        else if (bytes_sent == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // socket buffer full, wait for next EPOLLOUT
                return;
            }

            reactor_.deleteHandler(fd_);
            return;
        }
    }

    // response fully sent
    handleResponseComplete();
}

void ConnectionHandler::handleResponseComplete()
{
    request_.reset();
    write_buffer_.clear();
    write_offset_ = 0;

    reactor_.updateHandler(this, EPOLLIN | EPOLLET);
}
```
