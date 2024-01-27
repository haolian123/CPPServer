#!/bin/bash

# 要执行的文件
files=("testBlockQueue" "testThreadPool" "testEpollManager" "testDBConnectionPool")

for file in "${files[@]}"; do
    if [ -x "$file" ]; then
        echo "正在运行: $file"
        ./"$file"
    else
        echo "警告：文件 '$file' 不存在或不可执行。"
    fi
done
