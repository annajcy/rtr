#pragma once

#include "engine/runtime/global/base.h" 

namespace rtr {

enum class Resource_type {
    MODEL,
    IMAGE,
    TEXT,
    SHADER,
    MATERIAL,
    MESH,
    TEXTURE,
    UNKNOWN
};

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

    operator unsigned long long() const { return m_value; }

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

struct Resource_base {
    Resource_type type{};
    Hash hash{};

    Resource_base(Resource_type type, const Hash& hash) : type(type), hash(hash) {}

    virtual ~Resource_base() = default;
};

#pragma once

// #include "engine/runtime/global/base.h" 

// namespace rtr {

// enum class RHI_resource_type {
//     BUFFER,
//     TEXTURE,
//     SHADER_CODE,
//     SHADER_PROGRAM,
//     GEOMETRY,
//     FRAME_BUFFER,
//     RENDERER,
//     COMPUTE_TASK
// };

// class RHI_resource : public std::enable_shared_from_this<RHI_resource> {
// protected:
//     // 修改为 std::vector 存储依赖关系
//     std::vector<std::weak_ptr<RHI_resource>> m_dependencies;
//     std::vector<std::weak_ptr<RHI_resource>> m_reverse_dependencies;
//     RHI_resource_type m_resource_type{};
    
// public:
//     using Ptr_resource = std::shared_ptr<RHI_resource>;
    
//     RHI_resource(RHI_resource_type type) : m_resource_type(type) {}
    
//     virtual ~RHI_resource() {
//         std::vector<Ptr_resource> dependencies;
//         for (const auto& weak_dep : m_dependencies) {
//             if (auto dep = weak_dep.lock()) {
//                 dependencies.push_back(dep);
//             }
//         }

//         for (auto& dep : dependencies) {
//             remove_dependency(dep);
//         }

//         std::vector<Ptr_resource> reverse_dependencies;
//         for (const auto& weak_dep : m_reverse_dependencies) {
//             if (auto dep = weak_dep.lock()) {
//                 reverse_dependencies.push_back(dep);
//             }
//         }

//         for (auto& dep : reverse_dependencies) {
//             dep->remove_dependency(shared_from_this());
//         }
//     }

//     void add_dependency(const Ptr_resource& resource) {
//         if (!resource) return;
//         // 使用find_if和自定义比较器
//         auto pred = [&](const auto& weak) {
//             return !weak.expired() && weak.lock() == resource;
//         };
        
//         if (std::find_if(m_dependencies.begin(), m_dependencies.end(), pred) == m_dependencies.end()) {
//             m_dependencies.push_back(resource);
//         }
        
//         auto& reverse_deps = resource->m_reverse_dependencies;
//         auto self_pred = [&](const auto& weak) {
//             return !weak.expired() && weak.lock() == shared_from_this();
//         };
        
//         if (std::find_if(reverse_deps.begin(), reverse_deps.end(), self_pred) == reverse_deps.end()) {
//             reverse_deps.push_back( weak_from_this());
//         }
//     }

//     void remove_dependency(const Ptr_resource& resource) {
//         if (!resource) return;
//         // 使用find_if定位元素
//         auto pred = [&](const auto& weak) {
//             return !weak.expired() && weak.lock() == resource;
//         };
        
//         auto it = std::find_if(m_dependencies.begin(), m_dependencies.end(), pred);
//         if (it != m_dependencies.end()) {
//             m_dependencies.erase(it);
//         }
    
//         auto& reverse_deps = resource->m_reverse_dependencies;
//         auto self_pred = [&](const auto& weak) {
//             return !weak.expired() && weak.lock() == shared_from_this();
//         };
        
//         auto reverse_it = std::find_if(reverse_deps.begin(), reverse_deps.end(), self_pred);
//         if (reverse_it != reverse_deps.end()) {
//             reverse_deps.erase(reverse_it);
//         }
//     }

//     virtual RHI_resource_type resource_type() const { return m_resource_type; }
// };

// }

};