#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>

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


};