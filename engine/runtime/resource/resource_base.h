#pragma once

#include "engine/runtime/global/base.h" 
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace rtr {

struct Hash {
private:
    unsigned long long m_value{};

public:
    Hash() = default;
    Hash(const Hash&) = default;
    Hash(Hash&&) = default;
    Hash(unsigned long long value) : m_value(value) {}
    Hash& operator=(const Hash&) = default;
    Hash& operator=(Hash&&) = default;
    bool operator==(const Hash& other) const { return m_value == other.m_value; }
    bool operator!=(const Hash& other) const { return m_value != other.m_value; }

    Hash& operator+=(const Hash& other) {
        // 黄金比例素数 0x9e3779b97f4a7c15
        constexpr unsigned long long prime = 0x9e3779b97f4a7c15ULL;
        m_value ^= other.m_value + prime + (m_value << 6) + (m_value >> 2);
        return *this;
    }

    Hash operator+(const Hash& other) const {
        Hash combined = *this;
        combined += other;
        return combined;
    }

    unsigned long long value() const { return m_value; }

    static Hash from_string(const std::string& str) {
        constexpr unsigned long long fnv_prime = 0x100000001b3ULL;
        unsigned long long hash = 0xcbf29ce484222325ULL; // FNV偏移基础值
        
        for (const char& ch : str) {
            hash ^= static_cast<uint8_t>(ch);
            hash *= fnv_prime;
        }
        return Hash(hash);
    }

    static Hash from_raw_data(const unsigned char* data, unsigned int size) {
        constexpr unsigned long long prime = 0x9E3779B185EECB87ULL;
        unsigned long long hash = 0x2D358005CCAE049FULL;

        // 每次处理8字节
        const auto* blocks = reinterpret_cast<const unsigned long long*>(data);
        for (unsigned i = 0; i < size / 8; ++i) {
            hash ^= blocks[i];
            hash = (hash << 13) | (hash >> 51);
            hash *= prime;
        }

        // 处理剩余字节（1-7字节）
        const unsigned char* tail = data + (size / 8) * 8;
        unsigned long long last = 0;
        for (unsigned i = 0; i < size % 8; ++i) {
            last |= static_cast<unsigned long long>(tail[i]) << (i * 8);
        }
        hash ^= last;
        hash *= prime;

        return Hash(hash ^ (hash >> 33));
    }

};

template<typename Key_type, typename T>
class Resource_manager {
protected:
    std::unordered_map<Key_type, std::shared_ptr<T>> m_resources{};
    std::unordered_map<Key_type, std::vector<Key_type>> m_dependencies{};

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

    std::vector<Key_type> get_dependencies(const Key_type& key) {
        if (m_dependencies.find(key) == m_dependencies.end()) {
            std::cerr << "Resource_manager::get_dependencies: resource not found" << std::endl;
            return {};
        }
        return m_dependencies[key];
    }

    std::vector<std::shared_ptr<T>> get_dependencies(const Key_type& key, bool recursive) {
        if (m_dependencies.find(key) == m_dependencies.end()) {
            std::cerr << "Resource_manager::get_dependencies: resource not found" << std::endl;
            return {};
        }
        std::vector<std::shared_ptr<T>> dependencies{};
        for (const auto& dependency : m_dependencies[key]) {
            dependencies.push_back(get(dependency));
            if (recursive) {
                auto recursive_dependencies = get_dependencies(dependency, recursive);
                dependencies.insert(dependencies.end(), recursive_dependencies.begin(), recursive_dependencies.end());
            }
        }
    }

    template<typename S>
    std::shared_ptr<S> get(const Key_type& key) {
        if (m_resources.find(key) == m_resources.end()) {
            std::cerr << "Resource_manager::get: resource not found" << std::endl;
            return nullptr;
        }
        return std::dynamic_pointer_cast<S>(m_resources[key]);
    }

    void add_dependency(const Key_type& key, const Key_type& dependency) {
        m_dependencies[key].push_back(dependency);
    }

    void remove_dependency(const Key_type& key, const Key_type& dependency) {
        if (m_dependencies.find(key) == m_dependencies.end()) {
            std::cerr << "Resource_manager::remove_dependency: resource not found" << std::endl;
            return;
        }
        auto& dependencies = m_dependencies[key];
        dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), dependency), dependencies.end());
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
        m_dependencies.erase(key);
        for (auto &[_, dependencies] : m_dependencies) {
            dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), key), dependencies.end());
        }
    }

    bool has(const Key_type& key) const {
        return m_resources.find(key) != m_resources.end();
    }

    void clear() {
        m_resources.clear();
    }
};


};