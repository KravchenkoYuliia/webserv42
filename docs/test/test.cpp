#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define PORT 8080
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

// ------------------------------------------------------------
// Utility: set a file descriptor to non-blocking mode
// ------------------------------------------------------------
void setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }
}

// ------------------------------------------------------------
// Create, bind, and listen on a TCP socket
// ------------------------------------------------------------
int createServerSocket()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Allow fast restart of server
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, SOMAXCONN) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

int main()
{
    // ============================================================
    // 1️⃣ Create TCP server socket
    // ============================================================
    int server_fd = createServerSocket();

    // Set listening socket to non-blocking
    setNonBlocking(server_fd);

    std::cout << "Server listening on port " << PORT << std::endl;

    // ============================================================
    // 2️⃣ Create epoll instance
    // ============================================================
    int epoll_fd = epoll_create(1024);
    if (epoll_fd == -1)
    {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    // ============================================================
    // 3️⃣ Add server socket to epoll interest list
    //    EPOLLIN means: "Tell me when this fd is readable"
    //    For a listening socket, readable = new connection
    // ============================================================
    struct epoll_event event;
    event.events = EPOLLIN;      // We only care about incoming connections
    event.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
    {
        perror("epoll_ctl ADD server_fd");
        exit(EXIT_FAILURE);
    }

    // Buffer to store events returned by epoll_wait
    struct epoll_event events[MAX_EVENTS];

    // ============================================================
    // 4️⃣ Main Event Loop
    // ============================================================
    while (true)
    {
        // Wait indefinitely (-1) until something happens
        int ready = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        if (ready == -1)
        {
            if (errno == EINTR)
                continue; // interrupted by signal, retry
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        // Loop through all ready file descriptors
        for (int i = 0; i < ready; ++i)
        {
            int current_fd = events[i].data.fd;

            // ====================================================
            // 5️⃣ If event is on server_fd -> new client(s)
            // ====================================================
            if (current_fd == server_fd)
            {
                while (true)
                {
                    int client_fd = accept(server_fd, NULL, NULL);

                    if (client_fd == -1)
                    {
                        // No more clients waiting
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            break;
                        else
                        {
                            perror("accept");
                            break;
                        }
                    }

                    std::cout << "New client connected: "
                              << client_fd << std::endl;

                    // Make client socket non-blocking
                    setNonBlocking(client_fd);

                    // Add client socket to epoll
                    struct epoll_event client_event;
                    client_event.events = EPOLLIN; // monitor for incoming data
                    client_event.data.fd = client_fd;

                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD,
                                  client_fd, &client_event) == -1)
                    {
                        perror("epoll_ctl ADD client_fd");
                        close(client_fd);
                    }
                }
            }
            // ====================================================
            // 6️⃣ Otherwise, event is from a client socket
            // ====================================================
            else
            {
                char buffer[BUFFER_SIZE];

                int bytes = recv(current_fd, buffer,
                                 BUFFER_SIZE, 0);

                if (bytes <= 0)
                {
                    // Client closed connection or error
                    std::cout << "Client disconnected: "
                              << current_fd << std::endl;

                    close(current_fd);
                }
                else
                {
                    // Print received data
                    std::cout << "Received from "
                              << current_fd << ": "
                              << std::string(buffer, bytes)
                              << std::endl;

                    // Echo back to client
                    send(current_fd, buffer, bytes, 0);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
