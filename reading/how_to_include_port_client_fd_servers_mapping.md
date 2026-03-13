Perfect — here’s a **clean, consistent, step-by-step process** for your web server flow, showing **which class handles what, which variables are involved, and the logical order of operations**. I’ll avoid code and focus on responsibilities and data flow.

---

# **Step-by-Step Flow**

### **1️⃣ Parse the configuration**

* **Class:** `ConfigParser`
* **Action:** Read the config file and create a list of server definitions.
* **Data produced:**

  * `std::vector<ServerConfig>` containing for each server:

    * listening port
    * server name(s)
    * locations (URI paths, root directories, etc.)
* **Next step:** pass this list to `ServerManager`.

---

### **2️⃣ Initialize servers and listening sockets**

* **Class:** `ServerManager`
* **Action:**

  * Group `ServerConfig`s by their listening port.
  * Create one listening socket per unique port.
  * Attach each listening socket to the `Reactor` via a `ConnectionAcceptor`.
* **Data stored in ServerManager:**

  * Map: `port → vector<ServerConfig*>` (server candidates per port)
  * Reactor instance
* **Next step:** Reactor is now ready to handle incoming events.

---

### **3️⃣ Reactor waits for events**

* **Class:** `Reactor`
* **Action:**

  * Wait for I/O events using `epoll_wait`.
  * When an event occurs, dispatch it to the appropriate `IEventHandler`.
* **Handlers involved:**

  * `ConnectionAcceptor` for new connections
  * `ConnectionHandler` for existing client sockets
* **Next step:** New connections trigger `ConnectionAcceptor::handleRead()`.

---

### **4️⃣ Accept new client connections**

* **Class:** `ConnectionAcceptor`
* **Action:**

  * Accept new `client_fd`s from the listening socket.
  * Assign the connection’s port and server candidates.
  * Create a `ConnectionHandler` for each client and register it with the Reactor.
* **Data attached to ConnectionHandler:**

  * `client_fd` (socket)
  * `port` (from listening socket)
  * `servers_` (vector of ServerConfig candidates for that port)
* **Next step:** Client socket is ready for reading HTTP requests.

---

### **5️⃣ Read and parse HTTP request**

* **Class:** `ConnectionHandler`
* **Action:**

  * Read incoming bytes from the client socket in edge-triggered mode until EAGAIN.
  * Parse bytes into an `HttpRequest` using `RequestParser`.
* **Data stored in ConnectionHandler:**

  * `request_parser_` → parsed HTTP request data
* **Next step:** Determine which server configuration should handle the request.

---

### **6️⃣ Select server and location**

* **Class:** `ConnectionHandler`
* **Action:**

  * From the `servers_` vector, match the `Host` header to select the correct `ServerConfig`.
  * Within the selected server, choose the best matching `LocationConfig` based on the request URI.
* **Data stored:**

  * `selected_server_` → ServerConfig handling this request
  * `selected_location_` → LocationConfig for URI
* **Next step:** Build the HTTP response.

---

### **7️⃣ Build HTTP response**

* **Class:** `ResponseBuilder` (used by ConnectionHandler)
* **Action:**

  * Use the parsed request, `selected_server_`, and `selected_location_` to construct a `HttpResponse`.
  * Serialize the response into a buffer for sending.
* **Data stored in ConnectionHandler:**

  * `serialized_response_` → ready to send
  * `state_` → set to WRITING
* **Next step:** Reactor signals writable event on client socket.

---

### **8️⃣ Send HTTP response**

* **Class:** `ConnectionHandler`
* **Action:**

  * Write bytes from `serialized_response_` to the client socket in edge-triggered mode until all bytes are sent or EAGAIN.
  * Track how many bytes were sent (`bytes_sent_`).
  * If Keep-Alive is off, close the connection; otherwise, reset for next request.
* **Next step:** Either close connection or continue reading next request.

---

### **9️⃣ Cleanup**

* **Classes:** `ConnectionHandler`, `Reactor`
* **Action:**

  * On errors or connection close:

    * Remove the handler from the Reactor.
    * Close the socket.
    * Free `ConnectionHandler` memory.

---

# ✅ **Summary Table**

| Step | Class                                | Main Data                            | Responsibility                                          |
| ---- | ------------------------------------ | ------------------------------------ | ------------------------------------------------------- |
| 1    | ConfigParser                         | servers_                             | Parse config file into ServerConfig objects             |
| 2    | ServerManager                        | servers_by_port_, Reactor            | Initialize listening sockets per port, attach acceptors |
| 3    | Reactor                              | handlers_                            | Wait for events, dispatch to handlers                   |
| 4    | ConnectionAcceptor                   | port_, servers_                      | Accept connections, create ConnectionHandler            |
| 5    | ConnectionHandler (read)             | request_parser_                      | Read bytes, parse HTTP request                          |
| 6    | ConnectionHandler (server selection) | selected_server_, selected_location_ | Match Host header and URI to pick server/location       |
| 7    | ResponseBuilder                      | serialized_response_                 | Build HTTP response                                     |
| 8    | ConnectionHandler (write)            | bytes_sent_                          | Send response to client                                 |
| 9    | ConnectionHandler / Reactor          | N/A                                  | Cleanup after close/error                               |

---

This step-by-step listing shows **exactly which variables and methods are relevant at each stage**, and clearly separates responsibilities between classes.

If you want, I can also produce a **linear “event timeline” diagram** showing the flow from a client connection all the way to response sent, which makes this extremely easy to visualize.

Do you want me to do that?
