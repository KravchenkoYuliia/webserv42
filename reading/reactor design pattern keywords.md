reactor design pattern keywords

- handle factory
- Event handlers implement the same interface
- event handlers register with an Initiation Dispatcher, which uses a Synchronous Event demultiplexer to wait for events to occur
- When events occur, the synchronoius event demultiplexer notyifies the initiation dispatcher, which synchronously calls back to Event Handler associated with the event
- The event handler then disptaches the event to the method that implements the requested service



The key participants in the reactor pattern includ ethe following:
- Handles
- Synchronous Event Demultiplexer : select(), poll(), epoll()
- Initiation Dispatcher:
    - define an interface for registering, removing and dispatching Event Handlers
    - ultimately the Synchronous Event Demultiplexer is responsible for waiting until new events occur


