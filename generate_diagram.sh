# #!/bin/bash

# # 默认配置
# CONFIG_DIR="clang_uml_config"
# OUTPUT_DIR="docs/diagrams"
# SKIP_EXISTING=0 # 0 代表 false, 1 代表 true

# # 显示用法帮助函数
# usage() {
#     echo "用法: $0 [-s|--skip-existing]"
#     echo "  -s, --skip-existing  如果SVG输出文件已存在，则跳过其生成过程。"
#     exit 1
# }

# # 解析命令行选项
# # 如果需要更复杂的选项处理，可以使用 getopts
# if [[ "$1" == "-s" || "$1" == "--skip-existing" ]]; then
#     SKIP_EXISTING=1
#     echo "信息：已启用跳过已存在SVG文件模式。"
#     shift # 移除已解析的选项
# fi

# # 检查是否有多余的未知参数
# if [ "$#" -gt 0 ]; then
#     echo "错误：未知参数: $@"
#     usage
# fi


# # 检查配置目录是否存在
# if [ ! -d "$CONFIG_DIR" ]; then
#     echo "错误：配置目录 '$CONFIG_DIR' 不存在。"
#     exit 1
# fi

# # 检查配置目录中是否有yaml文件
# if ! compgen -G "$CONFIG_DIR/*.yaml" > /dev/null; then
#     echo "信息：在配置目录 '$CONFIG_DIR' 中没有找到 .yaml 文件。"
#     exit 0 # 不是错误，只是没有文件处理
# fi

# # 创建输出目录 (如果不存在)
# mkdir -p "$OUTPUT_DIR"

# echo "--- 参数配置 ---"
# echo "配置目录: $CONFIG_DIR"
# echo "输出目录: $OUTPUT_DIR"
# echo "跳过已存在SVG文件: $(if [ "$SKIP_EXISTING" -eq 1 ]; then echo "是"; else echo "否"; fi)"
# echo "--------------------"
# echo ""

# any_svg_generated_or_updated=0

# # 遍历配置目录中的所有yaml文件
# for config_file in "$CONFIG_DIR"/*.yaml; do
#     if [ -f "$config_file" ]; then
#         config_basename=$(basename "$config_file" .yaml)
        
#         # 假设：
#         # 1. YAML配置文件中的 'output_directory' 指向脚本的 $OUTPUT_DIR.
#         # 2. YAML配置文件中的 'output_name' (或其他决定输出文件名的设置) 使得生成的 .puml 文件名为 $config_basename.puml.
#         expected_puml_file_path="$OUTPUT_DIR/$config_basename.puml"
#         expected_svg_file_path="$OUTPUT_DIR/$config_basename.svg"

#         echo "正在处理配置文件: $config_file"

#         if [ "$SKIP_EXISTING" -eq 1 ] && [ -f "$expected_svg_file_path" ]; then
#             echo "  跳过 (SVG已存在): $expected_svg_file_path"
#             if [ -f "$expected_puml_file_path" ]; then
#                  echo "  对应的PUML文件: $expected_puml_file_path 也存在。"
#             fi
#             echo "--------------------" # 添加分隔符
#             continue # 继续处理下一个配置文件
#         fi
        
#         # --- 执行 clang-uml ---
#         echo "  执行 clang-uml ..."
#         clang-uml --config "$config_file" --query-driver .
        
#         status=$?
#         if [ $status -ne 0 ]; then
#             echo "  错误：clang-uml 处理配置文件 '$config_file' 失败 (退出码: $status)"
#             echo "--------------------" # 添加分隔符
#             continue # 继续处理下一个配置文件
#         fi

#         # --- 检查 .puml 文件并执行 PlantUML ---
#         if [ -f "$expected_puml_file_path" ]; then
#             echo "  clang-uml 成功生成/更新: $expected_puml_file_path"
#             echo "  执行 PlantUML 生成 SVG: $expected_svg_file_path..."
#             plantuml -tsvg "$expected_puml_file_path"
#             status=$?
#             if [ $status -ne 0 ]; then
#                 echo "  错误：PlantUML 未能从 '$expected_puml_file_path' 生成 SVG (退出码: $status)"
#             else
#                 echo "  PlantUML 成功生成: $expected_svg_file_path"
#                 any_svg_generated_or_updated=1
#             fi
#         else
#             echo "  警告: clang-uml 针对 '$config_file' 运行成功, 但预期的PUML文件"
#             echo "        '$expected_puml_file_path' 未在 '$OUTPUT_DIR' 中找到。"
#             echo "        请仔细检查 '$config_file' 中的以下设置:"
#             echo "        1. 'output_directory': 应确保其值为脚本中指定的输出目录 '$OUTPUT_DIR'。"
#             echo "        2. 'output_name' (或类似决定输出文件名的键): 应确保其值为 '$config_basename' (不含.puml后缀)。"
#             echo "        如果这些名称或路径不匹配，跳过逻辑将无法正确工作，并且可能无法生成SVG。"
#         fi
#     else
#         # 这通常不会发生，因为 for 循环的 globbing (*.yaml) 应该只返回文件
#         echo "跳过: '$config_file' 不是一个有效的文件。"
#     fi
#     echo "--------------------" # 添加分隔符
# done

# echo ""
# echo "--- 执行摘要 ---"
# if [ "$any_svg_generated_or_updated" -eq 1 ]; then
#     echo "部分或全部SVG图表已成功生成或更新于 '$OUTPUT_DIR'。"
# elif [ "$SKIP_EXISTING" -eq 1 ]; then
#     # 检查是否真实跳过了文件，或者只是没有文件需要处理
#     processed_any_config=0
#     if compgen -G "$CONFIG_DIR/*.yaml" > /dev/null; then processed_any_config=1; fi

#     if [ $processed_any_config -eq 1 ]; then
#         echo "没有新的SVG图表生成。可能所有SVG文件都已存在并被跳过，"
#         echo "或者 clang-uml 未能按预期名称/位置生成PUML文件 (请检查上述警告)。"
#         echo "如果希望重新生成，请删除对应的 .svg 文件或不使用 -s 选项运行脚本。"
#     else
#         # This case is covered by the initial check for yaml files.
#         echo "没有找到配置文件进行处理。"
#     fi
# else
#     # SKIP_EXISTING is false, and any_svg_generated_or_updated is 0
#     echo "警告：没有成功生成任何新的SVG图表。请检查上述日志中的错误和警告信息。"
#     echo "        确认 clang-uml 配置是否正确，以及PUML文件是否按预期生成在 '$OUTPUT_DIR' 目录下。"
# fi

# echo "脚本执行完毕。"

#!/bin/bash

# 默认配置
CONFIG_DIR="clang_uml_config"
OUTPUT_DIR="docs/diagrams"
SKIP_EXISTING=0 # 0 代表 false, 1 代表 true

# 显示用法帮助函数
usage() {
    echo "用法: $0 [-s|--skip-existing]"
    echo "  -s, --skip-existing  如果SVG输出文件已存在，则跳过其生成过程。"
    exit 1
}

# 解析命令行选项
if [[ "$1" == "-s" || "$1" == "--skip-existing" ]]; then
    SKIP_EXISTING=1
    echo "信息：已启用跳过已存在SVG文件模式。"
    shift # 移除已解析的选项
fi

# 检查是否有多余的未知参数
if [ "$#" -gt 0 ]; then
    echo "错误：未知参数: $@"
    usage
fi

# 检查配置目录是否存在
if [ ! -d "$CONFIG_DIR" ]; then
    echo "错误：配置目录 '$CONFIG_DIR' 不存在。"
    exit 1
fi

# 检查配置目录中是否有yaml文件 (递归)
# -type f ensures we only find files, not directories ending in .yaml
# -print -quit makes find exit after the first match, efficient for just checking existence
if ! find "$CONFIG_DIR" -name "*.yaml" -type f -print -quit 2>/dev/null | read; then
    echo "信息：在配置目录 '$CONFIG_DIR' 及其子目录中没有找到 .yaml 文件。"
    exit 0 # 不是错误，只是没有文件处理
fi

# 创建根输出目录 (如果不存在)
mkdir -p "$OUTPUT_DIR"

echo "--- 参数配置 ---"
echo "配置目录: $CONFIG_DIR"
echo "输出目录: $OUTPUT_DIR"
echo "跳过已存在SVG文件: $(if [ "$SKIP_EXISTING" -eq 1 ]; then echo "是"; else echo "否"; fi)"
echo "--------------------"
echo ""

any_svg_generated_or_updated=0
processed_any_config_file=0 # Flag to check if we processed any config file

# 递归遍历配置目录中的所有yaml文件
# -print0 and read -r -d $'\0' handle filenames with spaces or special characters
find "$CONFIG_DIR" -name "*.yaml" -type f -print0 | while IFS= read -r -d $'\0' config_file; do
    processed_any_config_file=1 # Mark that we found at least one config file to process

    # --- Determine relative path and output paths ---
    # Relative path of the config file with respect to CONFIG_DIR
    # e.g., if CONFIG_DIR="config" and config_file="config/subdir/my.yaml",
    # then relative_config_path="subdir/my.yaml"
    relative_config_path="${config_file#$CONFIG_DIR/}"

    # Relative path without the .yaml extension
    # e.g., "subdir/my"
    relative_path_no_ext="${relative_config_path%.yaml}"

    # Basename of the config file (without directory, without .yaml)
    # e.g., "my"
    config_basename=$(basename "$config_file" .yaml)

    # The directory where the .puml and .svg should be placed, mirroring the structure
    # e.g., docs/diagrams/subdir
    target_output_subdir="$OUTPUT_DIR/$relative_path_no_ext"
    # Remove the filename part to get the directory for mkdir
    target_output_dir_for_mkdir=$(dirname "$target_output_subdir")

    # Expected .puml and .svg file paths
    # e.g., docs/diagrams/subdir/my.puml
    expected_puml_file_path="$OUTPUT_DIR/$relative_path_no_ext.puml"
    expected_svg_file_path="$OUTPUT_DIR/$relative_path_no_ext.svg"

    echo "正在处理配置文件: $config_file"
    echo "  预期PUML输出: $expected_puml_file_path"
    echo "  预期SVG输出: $expected_svg_file_path"

    # Create the specific output subdirectory if it doesn't exist
    if ! mkdir -p "$target_output_dir_for_mkdir"; then
        echo "  错误：无法创建输出子目录 '$target_output_dir_for_mkdir'"
        echo "--------------------" # 添加分隔符
        continue # 继续处理下一个配置文件
    fi


    if [ "$SKIP_EXISTING" -eq 1 ] && [ -f "$expected_svg_file_path" ]; then
        echo "  跳过 (SVG已存在): $expected_svg_file_path"
        if [ -f "$expected_puml_file_path" ]; then
             echo "  对应的PUML文件: $expected_puml_file_path 也存在。"
        fi
        echo "--------------------" # 添加分隔符
        continue # 继续处理下一个配置文件
    fi
    
    # --- 执行 clang-uml ---
    echo "  执行 clang-uml ..."
    # IMPORTANT: For clang-uml to output to the correct subdirectory,
    # its YAML configuration needs 'output_directory' set correctly.
    # This script *assumes* the YAML config will handle placing the .puml file
    # in a path that, when prefixed with $OUTPUT_DIR, matches $expected_puml_file_path.
    # Specifically, the YAML should effectively have:
    # output_directory: $OUTPUT_DIR (e.g., docs/diagrams)
    # output_name: $relative_path_no_ext (e.g., subdir/myconfig)
    # OR
    # output_directory: $OUTPUT_DIR/$(dirname $relative_path_no_ext) (e.g., docs/diagrams/subdir)
    # output_name: $(basename $relative_path_no_ext) (e.g., myconfig)
    clang-uml --config "$config_file" --query-driver .
    
    status=$?
    if [ $status -ne 0 ]; then
        echo "  错误：clang-uml 处理配置文件 '$config_file' 失败 (退出码: $status)"
        echo "--------------------" # 添加分隔符
        continue # 继续处理下一个配置文件
    fi

    # --- 检查 .puml 文件并执行 PlantUML ---
    if [ -f "$expected_puml_file_path" ]; then
        echo "  clang-uml 成功生成/更新: $expected_puml_file_path"
        echo "  执行 PlantUML 生成 SVG: $expected_svg_file_path..."
        # PlantUML will output the SVG in the same directory as the PUML file
        plantuml -tsvg "$expected_puml_file_path"
        status=$?
        if [ $status -ne 0 ]; then
            echo "  错误：PlantUML 未能从 '$expected_puml_file_path' 生成 SVG (退出码: $status)"
        else
            echo "  PlantUML 成功生成: $expected_svg_file_path"
            any_svg_generated_or_updated=1
        fi
    else
        echo "  警告: clang-uml 针对 '$config_file' 运行成功, 但预期的PUML文件"
        echo "        '$expected_puml_file_path' 未在指定位置找到。"
        echo "        请仔细检查 '$config_file' 中的以下设置:"
        echo "        1. 'output_directory': 应该设置为能让PUML文件输出到 '$target_output_dir_for_mkdir' (或 '$OUTPUT_DIR' 如果 'output_name' 包含子路径)。"
        echo "        2. 'output_name' (或类似键): 应该设置为能形成 '$config_basename' (或 '$relative_path_no_ext' 如果 'output_directory' 是根输出目录)。"
        echo "        例如, 对于配置文件 '$config_file',"
        echo "        一种可能的有效组合是:"
        echo "          output_directory: \"$target_output_dir_for_mkdir\""
        echo "          output_name: \"$config_basename\""
        echo "        或者:"
        echo "          output_directory: \"$OUTPUT_DIR\""
        echo "          output_name: \"$relative_path_no_ext\""
        echo "        如果这些名称或路径不匹配，跳过逻辑将无法正确工作，并且可能无法生成SVG。"
    fi
    echo "--------------------" # 添加分隔符
done

echo ""
echo "--- 执行摘要 ---"
if [ "$any_svg_generated_or_updated" -eq 1 ]; then
    echo "部分或全部SVG图表已成功生成或更新于 '$OUTPUT_DIR' (包括其子目录)。"
elif [ "$SKIP_EXISTING" -eq 1 ]; then
    if [ $processed_any_config_file -eq 1 ]; then
        echo "没有新的SVG图表生成。可能所有SVG文件都已存在并被跳过，"
        echo "或者 clang-uml 未能按预期名称/位置生成PUML文件 (请检查上述警告)。"
        echo "如果希望重新生成，请删除对应的 .svg 文件或不使用 -s 选项运行脚本。"
    else
        # This case is covered by the initial check for yaml files using find.
        # If we are here, it means the find command in the loop didn't iterate even once.
        # However, the initial check should have exited if no files were found.
        # This message is a fallback or for clarity.
        echo "没有找到配置文件进行处理 (已在脚本开始时检查)。"
    fi
else
    # SKIP_EXISTING is false, and any_svg_generated_or_updated is 0
    if [ $processed_any_config_file -eq 1 ]; then
      echo "警告：没有成功生成任何新的SVG图表。请检查上述日志中的错误和警告信息。"
      echo "        确认 clang-uml 配置是否正确，以及PUML文件是否按预期生成在 '$OUTPUT_DIR' 的对应子目录下。"
    else
      # This case should also be covered by the initial find check.
      echo "没有找到配置文件进行处理 (已在脚本开始时检查)。"
    fi
fi

echo "脚本执行完毕。"