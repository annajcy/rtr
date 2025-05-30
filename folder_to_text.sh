#!/bin/bash

# 检查是否提供了输入文件夹参数
if [ -z "$1" ]; then
  echo "用法: $0 <输入文件夹>"
  exit 1
fi

# 检查输入文件夹是否存在
if [ ! -d "$1" ]; then
  echo "错误: 文件夹 '$1' 不存在。"
  exit 1
fi

INPUT_DIR="$1"
# 将输出文件名设置为输入文件夹的名称加上 "_code.txt"
OUTPUT_FILE="${INPUT_DIR%/}_code.txt" # %/ 删除末尾的斜杠 (如果存在)

# 如果输出文件已存在，则先删除
if [ -f "$OUTPUT_FILE" ]; then
  rm "$OUTPUT_FILE"
fi

# 查找并处理文件
# -type f: 只查找普通文件
# -name '*.cpp' -o -name '*.h' -o -name '*.hpp': 查找指定后缀的文件
# -print0: 使用 null 字符分隔文件名，以便安全处理包含空格或特殊字符的文件名
# xargs -0 -I {}: 读取以 null 分隔的输入，并为每个文件执行命令，{} 会被文件名替换
# sh -c '...': 执行一个 shell 命令字符串
# echo "--- 文件名: {} ---" >> "$OUTPUT_FILE": 将文件名作为分隔符写入输出文件
# cat "{}" >> "$OUTPUT_FILE": 将文件内容追加到输出文件
# echo -e "\n" >> "$OUTPUT_FILE": 在每个文件内容后添加一个空行，以增加可读性
find "$INPUT_DIR" -type f \( -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \) -print0 | while IFS= read -r -d $'\0' file; do
  echo "--- 文件名: $file ---" >> "$OUTPUT_FILE"
  cat "$file" >> "$OUTPUT_FILE"
  echo -e "\n" >> "$OUTPUT_FILE"
done

echo "所有代码已合并到: $OUTPUT_FILE"