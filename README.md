## 项目概述
___
​	这是一个基于Linux下C++ 的网络服务器项目。通过使用 Epoll I/O 多路复用技术、线程池、数据库连接池等现代网络编程技术，提供了服务器环境。服务器支持基本的 HTTP 功能，并通过多线程处理客户端请求，实现高效的连接处理。

​	编写该项目的是为了熟悉linux下的网络编程，还可以顺便熟悉线程池、连接池、Epoll等技术。编写的过程中参考了《TinyWebServer》和《WebServer: C++》这两个项目。

## 功能特点
___
- **多线程处理**：利用线程池处理并发请求，提高服务器处理能力。
- **Epoll I/O 多路复用**：使用 Epoll 实现 I/O 事件的高效处理。
- **数据库连接池**：维护数据库连接，提高数据库操作的效率。
- **定时器支持**：通过定时器处理非活跃连接，保持服务器性能。
- **灵活的配置**：支持多种服务器配置，包括端口、超时时间、日志级别等。

## 类结构
___
- **WebServer**：主服务器类，负责服务器的初始化、运行和资源管理。
- **EpollManager**：Epoll 事件管理。
- **ThreadPool**：线程池管理。
- **DBConnectionPool**：数据库连接池管理。
- **TimerHandler**：定时器处理。
- **HttpHandler**：处理 HTTP 请求和响应。

## 项目文件结构
___

- `BlockQueue.h`: 阻塞队列的实现。
- `Buffer.cpp` & `Buffer.h`: 缓冲区的实现。
- `DBConnection*`: 数据库连接相关类。
- `EpollManager*`: Epoll 事件处理相关类。
- `Http*`: HTTP 请求和响应处理相关类。
- `Log*`: 日志系统的实现。
- `main.cpp`: 程序入口。
- `Makefile`: 用于编译项目的 Makefile 文件。
- `resources/`: 包含静态资源如 HTML、CSS、JS 文件和图片等。
- `ThreadPool.h`: 线程池的实现。
- `TimerHandler*`: 定时器处理类。
- `WebServer*`: Web 服务器主类。

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

