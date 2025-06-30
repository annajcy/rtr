#!/bin/bash

# 确保我们在仓库的根目录
if [ ! -d ".git" ]; then
    echo "错误：请在 Git 仓库的根目录下运行此脚本。"
    exit 1
fi

# 1. 取消所有子模块的初始化，并强制执行
echo "正在取消初始化所有子模块..."
git submodule deinit --all -f

# 2. 获取所有子模块的路径，并从 .gitmodules 和工作区中移除它们
echo "正在从 Git 索引和工作区中移除所有子模块..."
while read -r path; do
    if [ -n "$path" ]; then
        echo "正在移除: $path"
        # 从 .git/config 和索引中移除
        git rm -f "$path"
        # 清理 .git/modules 中的残留数据
        rm -rf ".git/modules/$path"
    fi
done <<< "$(git submodule -q foreach 'echo $path')"

# 3. 移除 .gitmodules 文件（如果还存在）
if [ -f ".gitmodules" ]; then
    echo "正在移除 .gitmodules 文件..."
    git rm -f .gitmodules
fi

# 4. 提交你的更改
echo "所有子模块已成功移除。请运行 'git commit' 来提交更改。"
echo "建议的提交信息: 'Chore: Remove all submodules'"

# 提示用户进行提交
git status