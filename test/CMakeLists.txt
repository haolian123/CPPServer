cmake_minimum_required(VERSION 3.12) 

project(my_cpp_project) # 项目名称

# 启用测试
enable_testing()

# 添加 Google Test 子目录
add_subdirectory(googletest)

# 引入头文件
include_directories(googletest/include)
include_directories(Webserver) # 项目头文件目录
include_directories(/usr/include/mysql) #  MySQL库的头文件目录

# 创建 testBlockQueue 可执行文件
add_executable(testBlockQueue testBlockQueue.cpp)
# 链接 Google Test 库到 testBlockQueue 可执行文件
target_link_libraries(testBlockQueue PRIVATE gtest)

# 创建 testThreadPool 可执行文件
add_executable(testThreadPool testThreadPool.cpp)
# 链接 Google Test 库到 testThreadPool 可执行文件
target_link_libraries(testThreadPool PRIVATE gtest)

# 创建 testDBConnectionPool 可执行文件，包含 DBConnectionPool.cpp
add_executable(testDBConnectionPool testDBConnectionPool.cpp ../DBConnectionPool.cpp)
# 链接 Google Test 库和 MySQL 库到 testDBConnectionPool 可执行文件
target_link_libraries(testDBConnectionPool PRIVATE gtest /usr/lib/x86_64-linux-gnu/libmysqlclient.so) # 这里为实际的 MySQL 库文件路径

# 创建 testEpollManager 可执行文件，包含 EpollManager.cpp
add_executable(testEpollManager testEpollManager.cpp ../EpollManager.cpp)
# 链接 Google Test 库和 MySQL 库到 testEpollManager 可执行文件
target_link_libraries(testEpollManager PRIVATE gtest) 

# 包含 Google Test 的 CMake 模块
include(GoogleTest)

# 自动发现测试
gtest_discover_tests(testBlockQueue)

gtest_discover_tests(testThreadPool)

gtest_discover_tests(testDBConnectionPool)

gtest_discover_tests(testEpollManager)


