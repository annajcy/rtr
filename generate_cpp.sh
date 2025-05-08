#!/bin/bash

TARGET_DIR="engine"

# 将这些文件重命名为.cpp
find "$TARGET_DIR" -type f -name "*.h" -exec sh -c 'cp "$0" "${0%.h}.diagram.cpp"' {} \;

echo "所有.h文件已重命名为.diagram.cpp"



