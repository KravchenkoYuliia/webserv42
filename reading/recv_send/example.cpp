void Server::handleClientRead(int fd) {
  char buf[4096];
  ssize_t retval = recv(fd, buf, sizeof(buf), 0); // J'ai remove le -1 du sizeof
  if (retval == -1) {
    LOG_ERR("recv(): " + std::string(strerror(errno)));
    closeClient(fd);
    return;
  } else if (retval == 0) {
    closeClient(fd);
    return;
  }
  LOG_RECV("FD = " << fd << " recv() received " << retval << " bytes");
  Client &client = _clientMap.find(fd)->second;
  client.swallow(buf, retval);
  if (client.isRequestComplete() == true) {
    epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLOUT;
    ev.data.fd = fd;
    epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev);
  }
}

// TODO: ne pas envoyer tout d'un coup en mode bourrin
void Server::handleClientWrite(int fd) {
  Client &client = _clientMap.find(fd)->second;
  if (client.isResponseReady() == false)
    client.buildResponse();

  ssize_t retval;

  if (client.isResponseReady() == true) {
    retval = send(fd, client.getResponse(), client.getResponseLength(), 0);
    if (retval == -1)
      LOG_ERR("send(): " + std::string(strerror(errno)));
    else
      LOG_SEND("FD = " << fd << " send() sent " << retval << " bytes");
    closeClient(fd);
  }
}

-----------------


enum ConnectionState {
    STATE_READING   = 1 << 0,
    STATE_WRITING   = 1 << 1,
    STATE_PROCESSING = 1 << 2,
    STATE_CLOSING   = 1 << 3
};

struct Connection {
    int fd;
    int state;                // combination of flags
    std::string read_buffer;
    std::string write_buffer;
};

void handle_event(Connection* conn, epoll_event& event, int epoll_fd) {
    epoll_event ev;
    ev.data.ptr = conn;

    // ----- Reading -----
    if ((conn->state & STATE_READING) && (event.events & EPOLLIN)) {
        char buf[4096];
        int n = recv(conn->fd, buf, sizeof(buf), 0);
        if (n <= 0) {
            // client closed or error
            conn->state = STATE_CLOSING;
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn->fd, NULL);
            close(conn->fd);
            return;
        }
        conn->read_buffer.append(buf, n);

        // If the request is complete (simplified detection)
        if (conn->read_buffer.find("\r\n\r\n") != std::string::npos) {
            conn->state &= ~STATE_READING;
            conn->state |= STATE_PROCESSING;

            // ---- Process the request ----
            conn->write_buffer = build_response(conn->read_buffer); // your function
            conn->state &= ~STATE_PROCESSING;
            conn->state |= STATE_WRITING;

            // Switch epoll to watch for writing
            ev.events = EPOLLOUT;
            epoll_ctl(epoll_fd, EPOLL_CTL_MOD, conn->fd, &ev);
        }
    }

    // ----- Writing -----
    if ((conn->state & STATE_WRITING) && (event.events & EPOLLOUT)) {
        ssize_t n = send(conn->fd, conn->write_buffer.c_str(), conn->write_buffer.size(), 0);
        if (n < 0) {
            // send error
            conn->state = STATE_CLOSING;
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn->fd, NULL);
            close(conn->fd);
            return;
        }

        conn->write_buffer.erase(0, n);
        if (conn->write_buffer.empty()) {
            // Finished sending
            conn->state &= ~STATE_WRITING;
            conn->state |= STATE_READING; // for keep-alive, or close for HTTP/1.0

            // Switch epoll to watch for reading again
            ev.events = EPOLLIN;
            epoll_ctl(epoll_fd, EPOLL_CTL_MOD, conn->fd, &ev);
        }
    }
}
