## 项目概述
​	这是一个基于Linux下C++ 的网络服务器项目。通过使用 Epoll I/O 多路复用技术、线程池、数据库连接池等现代网络编程技术，提供了服务器环境。服务器支持基本的 HTTP 功能，并通过多线程处理客户端请求，实现高效的连接处理。

​	编写该项目的是为了熟悉linux下的网络编程，还可以顺便熟悉线程池、连接池、Epoll等技术。编写的过程中参考了《TinyWebServer》和《WebServer: C++》这两个项目。

## 功能特点
- **多线程处理**：利用线程池处理并发请求，提高服务器处理能力。
- **Epoll I/O 多路复用**：使用 Epoll 实现 I/O 事件的高效处理。
- **数据库连接池**：维护数据库连接，提高数据库操作的效率。
- **定时器支持**：通过定时器处理非活跃连接，保持服务器性能。
- **灵活的配置**：支持多种服务器配置，包括端口、超时时间、日志级别等。

## 类结构
- **WebServer**：主服务器类，负责服务器的初始化、运行和资源管理。
- **EpollManager**：Epoll 事件管理。
- **ThreadPool**：线程池管理。
- **DBConnectionPool**：数据库连接池管理。
- **TimerHandler**：定时器处理。
- **HttpHandler**：处理 HTTP 请求和响应。

## 项目文件结构

- `BlockQueue.h`: 阻塞队列的实现。
- `Buffer.cpp` & `Buffer.h`: 缓冲区的实现。
- `DBConnection*`: 数据库连接相关类。
- `EpollManager*`: Epoll 事件处理相关类。
- `Http*`: HTTP 请求和响应处理相关类。
- `Log*`: 日志系统的实现。
- `main.cpp`: 程序入口。
- `Makefile`: 用于编译项目的 Makefile 文件。
- `resources/`: 包含静态资源如 HTML、CSS、JS 文件和图片等。
- `test/`: 测试相关文件。
- `ThreadPool.h`: 线程池的实现。
- `TimerHandler*`: 定时器处理类。
- `WebServer*`: Web 服务器主类。

## 运行
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

##  注意事项
- 确保在运行服务器前，MySQL 服务已启动并可访问。
- 根据服务器和数据库的实际配置调整 `main` 函数中的参数。

## 参考项目

[qinguoyi/TinyWebServer: :fire: Linux下C++轻量级WebServer服务器 (github.com)](https://github.com/qinguoyi/TinyWebServer)

[markparticle/WebServer: C++ Linux WebServer服务器 (github.com)](https://github.com/markparticle/WebServer)

