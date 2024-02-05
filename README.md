## 概述
___
​	这是一个在Linux环境下使用C++开发的高性能网络服务器项目，采用了Epoll I/O多路复用技术、线程池、数据库连接池以及Redis缓存处理。而且通过引入设计模式提升了代码的结构和维护性，同时采用多线程并发处理机制与Redis缓存策略，优化了客户端请求的处理流程。

​	编写的过程中参考了[TinyWebServer]([qinguoyi/TinyWebServer: :fire: Linux下C++轻量级WebServer服务器 (github.com)](https://github.com/qinguoyi/TinyWebServer))和[WebServer: C++ Linux WebServer服务器]([markparticle/WebServer: C++ Linux WebServer服务器 (github.com)](https://github.com/markparticle/WebServer))这两个项目，并对项目进行了一些改进，包括**代码重构、修复组件的bug、使用GoogleTest框架编写单元测试与引入redis缓存机制**等。

## 特点

---

- **多线程处理**：利用线程池处理并发请求，提高服务器处理能力。通过分配独立线程给每个网络请求，确保了高并发下的响应速度和稳定性。

- **Epoll I/O 多路复用**：使用 Epoll 实现 I/O 事件的高效处理。Epoll 可以同时监控多个描述符，当某个描述符就绪时，能够快速响应，大大提升了网络I/O性能。

- **数据库连接池**：维护数据库连接，提高数据库操作的效率。连接池避免了每次查询数据库时都建立和销毁连接的开销，从而加快了数据库操作速度。

- **定时器支持**：通过定时器处理非活跃连接，保持服务器性能。定时器能够定期检查并关闭闲置的连接，释放资源，保证服务器的高效运行。

- **灵活的配置**：支持多种服务器配置，包括端口、超时时间、日志级别等。易于根据不同的运行环境和需求进行调整，提升服务器的灵活性和可用性。

- **Redis缓存**：使用 Redis 进行数据缓存，降低数据库访问频率，提升数据处理速度。利用 Redis 的高性能和持久化特性，有效缓解了数据库的压力，加速了数据访问。

- **设计模式应用**：在代码编写中引入多种设计模式，如单例模式、RAII模式、单一职责原则等，增强了代码的可读性、可维护性和扩展性。

___

### 文件结构

```
.
├── bin
├── build
├── log
├── Makefile
├── README.assets
├── README.md
├── resources
├── src
├── test
└── webbench-1.5
```

### 说明

- `bin/`: 存放编译后生成的可执行文件。
- `build/`: 存放编译生成的文件。
- `log/`: 存放程序运行时生成的日志文件。
- `Makefile`: 项目的make编译配置文件，用于控制项目的编译过程。
- `README.assets/`: 存放README.md文件中使用的资源，如图片。
- `README.md`: 项目的说明文件，介绍项目的功能、使用方法等信息。
- `resources/`: 存放项目运行时需要的资源文件，如配置文件、静态页面等。
- `src/`: 存放项目的源代码文件和相关的头文件。
    - `db/`: 数据库连接和缓存管理相关的代码。
    - `network/`: 网络通信和HTTP协议处理相关的代码。
    - `server/`: 服务器主要运行逻辑的代码。
    - `utility/`: 提供日志、线程池、定时器等工具的代码。
- `test/`: 使用Gtest框架编写的单元测试和集成测试。
- `webbench-1.5/`: 包含WebBench工具，用于对web服务器进行压力测试。

## 类结构

___
### 源码文件结构

```
src
├── db
│   ├── DBConnectionPool.cpp
│   ├── DBConnectionPool.h
│   ├── DBConnectionRAII.h
│   └── RedisManager.h
├── main.cpp
├── network
│   ├── Buffer.cpp
│   ├── Buffer.h
│   ├── EpollManager.cpp
│   ├── EpollManager.h
│   ├── HttpHandler.cpp
│   ├── HttpHandler.h
│   ├── http_request
│   │   ├── AuthManager.cpp
│   │   ├── AuthManager.h
│   │   ├── HttpRequest.cpp
│   │   ├── HttpRequest.h
│   │   ├── HttpRequestParser.cpp
│   │   ├── HttpRequestParser.h
│   │   └── URLUtility.h
│   └── http_response
│       ├── FileMapper.cpp
│       ├── FileMapper.h
│       ├── HttpResponse.cpp
│       ├── HttpResponse.h
│       ├── MimeTypes.cpp
│       └── MimeTypes.h
├── server
│   ├── WebServer.cpp
│   └── WebServer.h
└── utility
    ├── BlockQueue.h
    ├── Log.cpp
    ├── Log.h
    ├── ThreadPool.h
    ├── TimerHandler.cpp
    └── TimerHandler.h
```

### 说明

#### db 模块
负责数据库连接和缓存管理。
- `DBConnectionPool`: 管理数据库连接池，提供数据库连接和释放接口。
- `DBConnectionRAII`: 利用RAII机制管理数据库连接资源，确保资源的正确释放。
- `RedisManager`: 封装对Redis操作的接口，提供缓存服务。

#### network 模块
负责网络通信和HTTP协议处理。
- `Buffer`: 网络数据缓冲区管理。
- `EpollManager`: 封装epoll相关操作，实现高效的事件处理。
- `HttpHandler`: 处理HTTP请求和响应。
- `HttpRequest`: 表示HTTP请求，解析请求内容。
- `HttpRequestParser`: 解析HTTP请求的具体内容。
- `AuthManager`: 处理用户认证相关逻辑。
- `URLUtility`: 提供URL处理的工具函数。

##### http_request 子目录
HTTP请求的处理。
- `AuthManager.cpp`: 实现认证管理相关功能。
- `AuthManager.h`: 认证管理类的声明。
- `HttpRequest.cpp`: 实现HTTP请求的处理逻辑。
- `HttpRequest.h`: HTTP请求类的声明。
- `HttpRequestParser.cpp`: 实现HTTP请求解析。
- `HttpRequestParser.h`: HTTP请求解析类的声明。
- `URLUtility.h`: 提供URL处理的工具函数。

##### http_response 子目录
HTTP响应的生成和处理。
- `FileMapper`: 映射服务器文件系统，支持文件请求的处理。
- `HttpResponse`: 构建HTTP响应，包括状态码、头部和正文。
- `MimeTypes`: 管理MIME类型，支持不同类型文件的响应。

#### server 模块
负责服务器的主要运行逻辑。
- `WebServer`: 服务器主类，管理服务的启动、运行和停止。

#### utility 模块
提供日志、线程池、定时器等工具类。
- `BlockQueue`: 阻塞队列，用于线程间的同步。
- `Log`: 提供日志记录功能。
- `ThreadPool`: 线程池管理。
- `TimerHandler`: 定时器处理逻辑，用于处理超时事件。

## 运行
___
### 配置数据库用户

```mysql
ALTER USER 'hao'@'hostname' IDENTIFIED BY 'hao';
GRANT ALL PRIVILEGES ON *.* TO 'hao'@'localhost' WITH GRANT OPTION;
```

### 创建测试用户

```sql
create database webserver;
use webserver
create table user(
    username char(50) NULL,
    password char(50) NULL
)ENGINE=InnoDB;
INSERT INTO user(username, password) VALUES('user1', '123456');
```

### 编译

```bash
make
```
### 运行服务器

```bash
./bin/server
```

### 在浏览器输入

```
127.0.0.1/1316
```

## 运行效果

图片

![image-20231215194726861](README.assets/image-20231215194726861.png)

视频

![image-20231215194859287](README.assets/image-20231215194859287.png)

主页

![image-20231215195013293](README.assets/image-20231215195013293.png)

登录

![image-20231215194918573](README.assets/image-20231215194918573.png)

## 配置说明

在 `main` 函数中配置服务器参数：
- `port`: 服务器端口。
- `trigMode`: 触发模式。
- `timeoutMS`: 超时时间（毫秒）。
- `optLinger`: 是否开启 Linger。
- `mysqlPort`: MySQL 端口。
- `user`: 数据库用户名。
- `pwd`: 数据库密码。
- `dbName`: 数据库名。
- `sqlNum`: 数据库连接数。
- `threadNum`: 线程池线程数。
- `isOpenLog`: 是否开启日志。
- `logLevel`: 日志级别。
- `asyncQueueSize`: 异步队列大小。

## 单元测试

___

使用Google Test框架测试四个组件：消息队列、线程池、数据库连接池、epoll管理类

### 步骤

1. 使用前请先配置好Google Test框架，将编译好的googletest复制到test目录下。

   ```shell
   test
   ├── CMakeLists.txt
   ├── googletest
   ├── README.md
   ├── testBlockQueue.cpp
   ├── testDBConnectionPool.cpp
   ├── testEpollManager.cpp
   ├── test.sh
   └── testThreadPool.cpp
   ```

2. 创建build文件夹并进入

   ```shell
   mkdir build
   cd build
   ```

3. 输入指令

   ```shell
   cmake ..
   make
   ../test.sh
   ```

### 测试结果

#### 消息队列

```
正在运行: testBlockQueue
[==========] Running 8 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 8 tests from BlockQueueTest
[ RUN      ] BlockQueueTest.Constructor
[       OK ] BlockQueueTest.Constructor (0 ms)
[ RUN      ] BlockQueueTest.Clear
[       OK ] BlockQueueTest.Clear (0 ms)
[ RUN      ] BlockQueueTest.EmptyAndFull
[       OK ] BlockQueueTest.EmptyAndFull (0 ms)
[ RUN      ] BlockQueueTest.PushBackAndFront
[       OK ] BlockQueueTest.PushBackAndFront (0 ms)
[ RUN      ] BlockQueueTest.PushFrontAndBack
[       OK ] BlockQueueTest.PushFrontAndBack (0 ms)
[ RUN      ] BlockQueueTest.Pop
[       OK ] BlockQueueTest.Pop (0 ms)
[ RUN      ] BlockQueueTest.PopWithTimeout
[       OK ] BlockQueueTest.PopWithTimeout (1000 ms)
[ RUN      ] BlockQueueTest.Close
[       OK ] BlockQueueTest.Close (0 ms)
[----------] 8 tests from BlockQueueTest (1000 ms total)

[----------] Global test environment tear-down
[==========] 8 tests from 1 test suite ran. (1000 ms total)
[  PASSED  ] 8 tests.
```

#### 线程池

```
正在运行: testThreadPool
[==========] Running 3 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 3 tests from ThreadPoolTest
[ RUN      ] ThreadPoolTest.SingleTask
[       OK ] ThreadPoolTest.SingleTask (101 ms)
[ RUN      ] ThreadPoolTest.MultipleTasks
[       OK ] ThreadPoolTest.MultipleTasks (1001 ms)
[ RUN      ] ThreadPoolTest.DestructorClosesPool
[       OK ] ThreadPoolTest.DestructorClosesPool (1 ms)
[----------] 3 tests from ThreadPoolTest (1103 ms total)

[----------] Global test environment tear-down
[==========] 3 tests from 1 test suite ran. (1103 ms total)
[  PASSED  ] 3 tests.
```

#### epoll管理类

```
正在运行: testEpollManager
[==========] Running 7 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 7 tests from EpollManagerTest
[ RUN      ] EpollManagerTest.ConstructorDestructor
[       OK ] EpollManagerTest.ConstructorDestructor (0 ms)
[ RUN      ] EpollManagerTest.AddFdValidFd
[       OK ] EpollManagerTest.AddFdValidFd (0 ms)
[ RUN      ] EpollManagerTest.AddFdInvalidFd
[       OK ] EpollManagerTest.AddFdInvalidFd (0 ms)
[ RUN      ] EpollManagerTest.ModifyFdValidFd
[       OK ] EpollManagerTest.ModifyFdValidFd (0 ms)
[ RUN      ] EpollManagerTest.ModifyFdInvalidFd
[       OK ] EpollManagerTest.ModifyFdInvalidFd (0 ms)
[ RUN      ] EpollManagerTest.DeleteFdValidFd
[       OK ] EpollManagerTest.DeleteFdValidFd (0 ms)
[ RUN      ] EpollManagerTest.DeleteFdInvalidFd
[       OK ] EpollManagerTest.DeleteFdInvalidFd (0 ms)
[----------] 7 tests from EpollManagerTest (0 ms total)

[----------] Global test environment tear-down
[==========] 7 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 7 tests.
```

#### 数据库连接池

```
正在运行: testDBConnectionPool
[==========] Running 3 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 3 tests from DBConnectionPoolTest
[ RUN      ] DBConnectionPoolTest.InitPool
[       OK ] DBConnectionPoolTest.InitPool (13 ms)
[ RUN      ] DBConnectionPoolTest.GetAndReleaseConnection
[       OK ] DBConnectionPoolTest.GetAndReleaseConnection (14 ms)
[ RUN      ] DBConnectionPoolTest.ConcurrentGetAndRelease
[       OK ] DBConnectionPoolTest.ConcurrentGetAndRelease (116 ms)
[----------] 3 tests from DBConnectionPoolTest (144 ms total)

[----------] Global test environment tear-down
[==========] 3 tests from 1 test suite ran. (144 ms total)
[  PASSED  ] 3 tests.
```





## 压力测试
___

### 测试环境：
   - 工具：Webbench - Simple Web Benchmark 1.5
   - 编译：
     - cd ./webbench-1.5
     - make

### 测试
1. 测试 1：
   - **命令**：
     ```
     ./webbench-1.5/webbench -c 1000 -t 10 http://127.0.0.1:1316/
     ```
   - **配置**：1000 客户端, 运行时间 10 秒
   - **结果**：
     - 速度：66510 页面/分钟, 2614378 字节/秒
     - 请求：11085 成功, 0 失败

2. **测试 2**：
   - **命令**：
     ```
     ./webbench-1.5/webbench -c 5000 -t 10 http://127.0.0.1:1316/
     ```
   - **配置**：5000 客户端, 运行时间 10 秒
   - **结果**：
     
     - 错误信息：`problems forking worker no. 4284`, `fork failed.: Resource temporarily unavailable`
   - **原因分析**：
     
     - 由于是在虚拟机进行的测试，系统资源（如进程数、内存等）限制了测试，无法创建更多的进程以满足 5000 个客户端的需求。

### Webbench 编译错误解决
1. **编译 Webbench**：
   在webbench-1.5目录下运行 `make webbench` 时出现错误。

2. **错误 1**：
   - **描述**：`fatal error: rpc/types.h: No such file or directory`
   - **解决方法**：
     1. 安装 `libtirpc-dev` 包：
        ```
        sudo apt-get install -y libtirpc-dev
        ```
     2. 创建软连接：
        ```
        sudo ln -s /usr/include/tirpc/rpc/types.h /usr/include/rpc
        ```

3. **错误 2**：
   - **描述**：`fatal error: netconfig.h: No such file or directory`
   - **解决方法**：创建另一个软连接：
     ```
     sudo ln -s /usr/include/tirpc/netconfig.h /usr/include
     ```

4. **错误 3**：
   - **描述**：`/bin/sh: 1: ctags: not found`
   - **解决方法**：安装 `ctags`：
     ```
     sudo apt-get install universal-ctags
     ```


##  注意事项
___
- 确保在运行服务器前，MySQL 服务已启动并可访问。
- 根据服务器和数据库的实际配置调整 `main` 函数中的参数。

## 参考项目
___
[qinguoyi/TinyWebServer: :fire: Linux下C++轻量级WebServer服务器 (github.com)](https://github.com/qinguoyi/TinyWebServer)

[markparticle/WebServer: C++ Linux WebServer服务器 (github.com)](https://github.com/markparticle/WebServer)

