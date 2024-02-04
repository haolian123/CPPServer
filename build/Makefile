CXX = g++
CFLAGS = -std=c++14 -O2 -Wall -g 

# 指定源代码目录
SRCDIR = ../src

# 生成可执行文件的目标名称
TARGET = server

# 源代码文件列表
SRCS = $(wildcard $(SRCDIR)/network/*.cpp) \
       $(wildcard $(SRCDIR)/db/*.cpp) \
       $(wildcard $(SRCDIR)/server/*.cpp) \
       $(wildcard $(SRCDIR)/utility/*.cpp) \
       $(SRCDIR)/main.cpp

# 默认目标
all: $(TARGET)

# 链接目标
$(TARGET): 
	$(CXX) $(CFLAGS) -o ../bin/$@ $(SRCS) -pthread -lmysqlclient


clean:
	rm -f ../bin/$(TARGET)
.PHONY: clean
