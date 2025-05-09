#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace rtr {

template<typename Key_type, typename T>
class Resource_manager {
protected:
    std::unordered_map<Key_type, std::shared_ptr<T>> m_resources{};

public:
    Resource_manager() = default;
    ~Resource_manager() = default;

    std::shared_ptr<T> get(const Key_type& key) {
        if (m_resources.find(key) == m_resources.end()) {
            std::cerr << "Resource_manager::get: resource not found" << std::endl;
            return nullptr;
        }
        return m_resources[key];
    }

    template<typename S>
    std::shared_ptr<S> get(const Key_type& key) {
        if (m_resources.find(key) == m_resources.end()) {
            std::cerr << "Resource_manager::get: resource not found" << std::endl;
            return nullptr;
        }
        return std::dynamic_pointer_cast<S>(m_resources[key]);
    }

    void add(const Key_type& key, const std::shared_ptr<T>& resource) {
        m_resources[key] = resource;
    }

    void remove(const Key_type& key) {
        if (m_resources.find(key) == m_resources.end()) {
            std::cerr << "Resource_manager::remove: resource not found" << std::endl;
            return;
        }
        m_resources.erase(key);
    }

    bool has(const Key_type& key) const {
        return m_resources.find(key) != m_resources.end();
    }

    void clear() {
        m_resources.clear();
    }
};

template<typename Key_type, typename T>
class Resource_manager_dep {
protected:
    std::unordered_map<Key_type, std::shared_ptr<T>> m_resources{};
    // 依赖图: Key_type (资源) -> 它所依赖的资源集合 (Key_type)
    std::unordered_map<Key_type, std::unordered_set<Key_type>> m_dependencies{};
    // 被依赖图: Key_type (资源) -> 依赖于它的资源集合 (Key_type)
    std::unordered_map<Key_type, std::unordered_set<Key_type>> m_dependents{};

private:
    // 辅助函数：检查从 start_node 是否可以通过 m_dependencies 到达 target_node
    // 用于循环依赖检测：在添加 A -> B 之前，检查是否已存在路径 B -> ... -> A
    bool is_reachable(const Key_type& start_node, const Key_type& target_node) const {
        if (!m_resources.count(start_node) || !m_resources.count(target_node)) {
            return false; // 如果节点不存在，则不可达
        }
        if (start_node == target_node) return true;

        std::unordered_set<Key_type> visited;
        std::vector<Key_type> stack; // DFS 栈

        stack.push_back(start_node);
        visited.insert(start_node);

        while (!stack.empty()) {
            Key_type current_node = stack.back();
            stack.pop_back();

            if (m_dependencies.count(current_node)) {
                for (const auto& neighbor : m_dependencies.at(current_node)) {
                    if (neighbor == target_node) {
                        return true; // 找到目标
                    }
                    if (visited.find(neighbor) == visited.end() && m_resources.count(neighbor)) { // 确保邻居也存在
                        visited.insert(neighbor);
                        stack.push_back(neighbor);
                    }
                }
            }
        }
        return false; // 未找到目标
    }


public:
    Resource_manager_dep() = default;
    ~Resource_manager_dep() = default; // shared_ptr 会自动管理资源生命周期

    std::shared_ptr<T> get(const Key_type& key) {
        auto it = m_resources.find(key);
        if (it == m_resources.end()) {
            // 考虑使用更具体的 Key_type 打印方式，如果它不是原生可打印的
            // std::cerr << "Resource_manager::get: resource with key '" << key << "' not found" << std::endl;
            std::cerr << "Resource_manager::get: resource not found" << std::endl;
            return nullptr;
        }
        return it->second;
    }

    template<typename S>
    std::shared_ptr<S> get(const Key_type& key) {
        auto it = m_resources.find(key);
        if (it == m_resources.end()) {
            // std::cerr << "Resource_manager::get (typed): resource with key '" << key << "' not found" << std::endl;
            std::cerr << "Resource_manager::get (typed): resource not found" << std::endl;
            return nullptr;
        }
        return std::dynamic_pointer_cast<S>(it->second);
    }

    // 添加资源，不带初始依赖
    void add(const Key_type& key, const std::shared_ptr<T>& resource) {
        if (m_resources.count(key)) {
            // std::cerr << "Resource_manager::add: Resource with key '" << key << "' already exists. Overwriting." << std::endl;
             std::cerr << "Resource_manager::add: Resource with key already exists. Overwriting." << std::endl;
        }
        m_resources[key] = resource;
        // 新添加的资源初始没有依赖或被依赖项，需要通过 add_dependency 明确添加
    }
    
    // 添加一个资源，并指定其依赖项
    // 返回 true 如果成功添加，false 如果失败 (例如，依赖项不存在或会导致循环)
    bool add_with_dependencies(const Key_type& key, const std::shared_ptr<T>& resource, const std::unordered_set<Key_type>& dependencies) {
        if (has(key)) {
            std::cerr << "Resource_manager::add_with_dependencies: Resource with key already exists." << std::endl;
            return false;
        }

        // 检查所有依赖项是否存在
        for (const auto& dep_key : dependencies) {
            if (!has(dep_key)) {
                std::cerr << "Resource_manager::add_with_dependencies: Dependency resource not found." << std::endl;
                return false;
            }
        }
        
        // 临时添加资源本身，以便循环检测可以“看到”它
        // 但不在依赖图中添加任何边
        m_resources[key] = resource;

        // 检查添加这些依赖是否会导致循环
        // 对于每个依赖 A -> B (key -> dep_key), 检查是否已存在路径 B -> A
        for (const auto& dep_key : dependencies) {
            if (is_reachable(dep_key, key)) {
                 std::cerr << "Resource_manager::add_with_dependencies: Adding dependency from new resource to an existing one would create a cycle." << std::endl;
                 m_resources.erase(key); // 回滚添加的资源
                 return false;
            }
        }
        
        // 实际添加依赖关系
        for (const auto& dep_key : dependencies) {
            m_dependencies[key].insert(dep_key);
            m_dependents[dep_key].insert(key);
        }
        return true;
    }


    // 添加依赖关系: resource_key 依赖于 dependency_key (resource_key -> dependency_key)
    bool add_dependency(const Key_type& resource_key, const Key_type& dependency_key) {
        if (!has(resource_key) || !has(dependency_key)) {
            std::cerr << "Resource_manager::add_dependency: One or both resources not found." << std::endl;
            return false;
        }

        if (resource_key == dependency_key) {
            std::cerr << "Resource_manager::add_dependency: Resource cannot depend on itself." << std::endl;
            return false;
        }

        // 检查是否会形成循环: 如果添加 resource_key -> dependency_key，
        // 那么需要检查是否已存在从 dependency_key 到 resource_key 的路径
        if (is_reachable(dependency_key, resource_key)) {
            std::cerr << "Resource_manager::add_dependency: Adding this dependency would create a cycle." << std::endl;
            return false;
        }
        
        // 检查是否已存在此依赖
        if (m_dependencies.count(resource_key) && m_dependencies[resource_key].count(dependency_key)) {
            // std::cerr << "Resource_manager::add_dependency: Dependency already exists." << std::endl;
            return true; // 已经是依赖关系，也算成功
        }

        m_dependencies[resource_key].insert(dependency_key);
        m_dependents[dependency_key].insert(resource_key);
        return true;
    }

    // 移除依赖关系
    bool remove_dependency(const Key_type& resource_key, const Key_type& dependency_key) {
        if (!has(resource_key) || !has(dependency_key)) {
            std::cerr << "Resource_manager::remove_dependency: One or both resources not found." << std::endl;
            return false;
        }

        bool changed = false;
        if (m_dependencies.count(resource_key)) {
            if (m_dependencies[resource_key].erase(dependency_key) > 0) {
                changed = true;
            }
            if (m_dependencies[resource_key].empty()) {
                m_dependencies.erase(resource_key);
            }
        }

        if (m_dependents.count(dependency_key)) {
            if (m_dependents[dependency_key].erase(resource_key) > 0) {
                changed = true;
            }
            if (m_dependents[dependency_key].empty()) {
                m_dependents.erase(dependency_key);
            }
        }
        
        if (!changed) {
             std::cerr << "Resource_manager::remove_dependency: Dependency did not exist." << std::endl;
        }
        return changed;
    }


    void remove(const Key_type& key) {
        if (!has(key)) {
            // std::cerr << "Resource_manager::remove: resource with key '" << key << "' not found" << std::endl;
            std::cerr << "Resource_manager::remove: resource not found" << std::endl;
            return;
        }

        // 检查是否有其他资源依赖于此资源
        if (m_dependents.count(key) && !m_dependents[key].empty()) {
            std::cerr << "Resource_manager::remove: Cannot remove resource. Other resources depend on it. Dependents are: ";
            // 实际打印Key_type列表可能需要特定代码
            for(const auto& dep_key : m_dependents[key]) {
                // std::cerr << dep_key << " "; // 假设Key_type可打印
            }
            std::cerr << "(dependents list)" << std::endl;
            return;
        }

        // 移除此资源对其依赖项的 "被依赖" 记录
        if (m_dependencies.count(key)) {
            for (const auto& dependency_it_depends_on : m_dependencies[key]) {
                if (m_dependents.count(dependency_it_depends_on)) {
                    m_dependents[dependency_it_depends_on].erase(key);
                    if (m_dependents[dependency_it_depends_on].empty()) {
                        m_dependents.erase(dependency_it_depends_on);
                    }
                }
            }
            m_dependencies.erase(key); // 移除此资源的所有依赖记录
        }
        
        // 确保它不再是任何依赖项的依赖者 (理论上上面已经处理，这里是双重检查)
        // 这个步骤是多余的，因为如果它被依赖，上面的第一个if会阻止删除

        m_resources.erase(key); // 从主资源映射中移除
    }

    bool has(const Key_type& key) const {
        return m_resources.find(key) != m_resources.end();
    }

    void clear() {
        m_resources.clear();
        m_dependencies.clear();
        m_dependents.clear();
    }

    std::unordered_set<Key_type> get_dependencies_of(const Key_type& key) const {
        if (m_dependencies.count(key)) {
            return m_dependencies.at(key);
        }
        return {};
    }

    std::unordered_set<Key_type> get_dependents_on(const Key_type& key) const {
        if (m_dependents.count(key)) {
            return m_dependents.at(key);
        }
        return {};
    }
};


};