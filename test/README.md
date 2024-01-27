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



