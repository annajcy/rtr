#pragma once

#include "engine/runtime/global/base.h"
#include <filesystem>
#include <vector>

namespace rtr {

// 在类定义前添加文件树节点结构体
struct File_tree_node {
    std::string name;
    bool is_directory;
    std::vector<File_tree_node> children;
};

class File_service {
    std::filesystem::path m_root_path;  // 根目录路径

public:
    // 设置文件系统根目录
    void set_root(const std::string& path) { 
        m_root_path = std::filesystem::absolute(path);
    }

    // 获取目录下的文件列表（相对根目录的路径）
    std::vector<std::string> list_files(const std::string& relative_dir = "") const {
        std::vector<std::string> result;
        auto full_path = m_root_path / relative_dir;
        
        for (const auto& entry : std::filesystem::directory_iterator(full_path)) {
            result.push_back(entry.path().filename().string());
        }
        return result;
    }

    // 创建空文件
    bool add_file(const std::string& relative_path) {
        auto full_path = m_root_path / relative_path;
        if (exists(full_path)) return false;
        
        std::ofstream file(full_path);
        return file.good();
    }

    // 删除文件/目录
    bool delete_file(const std::string& relative_path) {
        auto full_path = m_root_path / relative_path;
        return std::filesystem::remove_all(full_path) > 0;
    }

    // 复制文件/目录
    bool copy_file(const std::string& src, const std::string& dest) {
        auto src_path = m_root_path / src;
        auto dest_path = m_root_path / dest;
        
        try {
            std::filesystem::copy(src_path, dest_path, 
                std::filesystem::copy_options::recursive | 
                std::filesystem::copy_options::overwrite_existing);
            return true;
        } catch (...) {
            return false;
        }
    }

    // 移动/重命名文件
    bool move_file(const std::string& src, const std::string& dest) {
        auto src_path = m_root_path / src;
        auto dest_path = m_root_path / dest;
        
        try {
            std::filesystem::rename(src_path, dest_path);
            return true;
        } catch (...) {
            return false;
        }
    }

    // 新增文件树构建方法
    File_tree_node build_file_tree(const std::string& relative_dir = "") const {
        return build_tree_recursive(m_root_path / relative_dir);
    }

private:
    // 新增递归构建方法
    File_tree_node build_tree_recursive(const std::filesystem::path& current_path) const {
        File_tree_node node;
        node.name = current_path.filename().string();
        node.is_directory = is_directory(current_path);

        if (node.is_directory) {
            for (const auto& entry : std::filesystem::directory_iterator(current_path)) {
                node.children.push_back(build_tree_recursive(entry.path()));
            }
        }
        
        return node;
    }
};

}