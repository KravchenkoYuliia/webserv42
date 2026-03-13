```c++
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <cstdlib>

#define MAX_EVENTS 10

int main()
{
    int epoll_fd = epoll_create(10);

    // assume server_fd already exists and is listening + non-blocking
    int server_fd; // <-- assume initialized

    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    std::map<int, std::string> read_buffer;
    std::map<int, std::string> write_buffer;

    epoll_event events[MAX_EVENTS];

    while (true)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; i++)
        {
            int fd = events[i].data.fd;

            // 🔵 NEW CONNECTION
            if (fd == server_fd)
            {
                int client_fd = accept(server_fd, NULL, NULL);

                fcntl(client_fd, F_SETFL, O_NONBLOCK);

                epoll_event client_ev;
                client_ev.events = EPOLLIN;
                client_ev.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);

                read_buffer[client_fd] = "";
                write_buffer[client_fd] = "";
            }

            // 🔵 READ EVENT
            else if (events[i].events & EPOLLIN)
            {
                char buf[4096];

                while (true)
                {
                    ssize_t bytes = recv(fd, buf, sizeof(buf), 0);

                    if (bytes > 0)
                    {
                        read_buffer[fd].append(buf, bytes);
                    }
                    else if (bytes == 0)
                    {
                        close(fd);
                        read_buffer.erase(fd);
                        write_buffer.erase(fd);
                        break;
                    }
                    else
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            break;
                        close(fd);
                        read_buffer.erase(fd);
                        write_buffer.erase(fd);
                        break;
                    }
                }

                // 🔵 CHECK IF REQUEST COMPLETE
                size_t header_end = read_buffer[fd].find("\r\n\r\n");

                if (header_end != std::string::npos)
                {
                    std::string header_part =
                        read_buffer[fd].substr(0, header_end);

                    std::istringstream stream(header_part);
                    std::string line;
                    std::string method;
                    std::string path;
                    size_t content_length = 0;

                    // First line
                    std::getline(stream, line);
                    std::istringstream firstLine(line);
                    firstLine >> method >> path;

                    // Headers
                    while (std::getline(stream, line))
                    {
                        if (line.find("Content-Length:") != std::string::npos)
                        {
                            std::string len =
                                line.substr(line.find(":") + 1);
                            content_length =
                                std::atoi(len.c_str());
                        }
                    }

                    bool request_complete = false;

                    if (method == "GET")
                        request_complete = true;

                    if (method == "POST")
                    {
                        size_t total_expected =
                            header_end + 4 + content_length;

                        if (read_buffer[fd].size() >= total_expected)
                            request_complete = true;
                    }

                    // 🔵 BUILD RESPONSE
                    if (request_complete)
                    {
                        std::string body;

                        if (method == "GET")
                        {
                            body =
                                "<html><body><h1>Hello GET</h1></body></html>";
                        }
                        else if (method == "POST")
                        {
                            std::string post_body =
                                read_buffer[fd].substr(header_end + 4);

                            body =
                                "Received POST body:\n" + post_body;
                        }

                        std::ostringstream response;
                        response << "HTTP/1.1 200 OK\r\n";
                        response << "Content-Length: "
                                 << body.size() << "\r\n";
                        response << "Content-Type: text/plain\r\n";
                        response << "Connection: close\r\n";
                        response << "\r\n";
                        response << body;

                        write_buffer[fd] = response.str();

                        // Switch to EPOLLOUT
                        epoll_event new_ev;
                        new_ev.events = EPOLLOUT;
                        new_ev.data.fd = fd;
                        epoll_ctl(epoll_fd,
                                   EPOLL_CTL_MOD,
                                   fd,
                                   &new_ev);
                    }
                }
            }

            // 🔵 WRITE EVENT
            else if (events[i].events & EPOLLOUT)
            {
                while (!write_buffer[fd].empty())
                {
                    ssize_t bytes = send(
                        fd,
                        write_buffer[fd].c_str(),
                        write_buffer[fd].size(),
                        0);

                    if (bytes > 0)
                    {
                        write_buffer[fd].erase(0, bytes);
                    }
                    else
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            break;
                        close(fd);
                        read_buffer.erase(fd);
                        write_buffer.erase(fd);
                        break;
                    }
                }

                if (write_buffer[fd].empty())
                {
                    close(fd);
                    read_buffer.erase(fd);
                    write_buffer.erase(fd);
                }
            }
        }
    }
}
```
