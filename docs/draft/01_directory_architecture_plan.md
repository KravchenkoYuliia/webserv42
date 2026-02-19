webserv/
│
├── Makefile
├── config/
│   └── default.conf
│
├── include/
│   ├── core/
│   │   ├── ServerManager.hpp
│   │   ├── EventLoop.hpp
│   │   ├── ConnectionManager.hpp
│   │   └── Socket.hpp
│   │
│   ├── config/
│   │   ├── ConfigParser.hpp
│   │   ├── ServerConfig.hpp
│   │   ├── LocationConfig.hpp
│   │   └── Tokenizer.hpp
│   │
│   ├── http/
│   │   ├── HttpRequest.hpp
│   │   ├── HttpResponse.hpp
│   │   ├── RequestParser.hpp
│   │   └── ResponseBuilder.hpp
│   │
│   ├── routing/
│   │   ├── Router.hpp
│   │   └── LocationMatcher.hpp
│   │
│   └── utils/
│       ├── Logger.hpp
│       ├── FileUtils.hpp
│       └── StringUtils.hpp
│
├── src/
│   ├── main.cpp
│   │
│   ├── core/
│   ├── config/
│   ├── http/
│   ├── routing/
│   └── utils/
│
└── www/
    ├── default/
    └── test_site/
