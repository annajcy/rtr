TARGET_DIR="engine"

# 然后删除这些.cpp文件
find "$TARGET_DIR" -type f -name "*.cpp" -delete
echo "所有.cpp文件已删除"