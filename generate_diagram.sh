#!/bin/bash

# 配置文件目录
CONFIG_DIR="clang_uml_config"
# 输出目录
OUTPUT_DIR="docs/diagrams"
# 编译数据库路径 
COMPILATION_DB="build/compile_commands.json"

# 检查编译数据库是否存在
if [ ! -f "$COMPILATION_DB" ]; then
    echo "错误：编译数据库不存在，请先执行："
    echo "cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=1"
    exit 1
fi

# 创建输出目录
mkdir -p "$OUTPUT_DIR"

# 遍历配置目录中的所有yaml文件
for config_file in "$CONFIG_DIR"/*.yaml; do
    if [ -f "$config_file" ]; then
        echo "正在处理配置文件: $config_file"
        
        # 执行clang-uml命令
        clang-uml --config "$config_file" --query-driver .
        
        # 检查命令执行状态
        if [ $? -ne 0 ]; then
            echo "错误：处理配置文件 $config_file 失败"
            exit 2
        fi
    fi
done

echo "UML图生成完成，输出目录：$OUTPUT_DIR"

plantuml -tsvg "$OUTPUT_DIR"/*.puml

echo "PlantUML图生成完成，输出目录：$OUTPUT_DIR"