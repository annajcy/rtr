#pragma once

#include "engine/global/base.h"
#include <memory>
#include <unordered_map>
#include <unordered_set> // 引入 unordered_set

namespace rtr {

enum class RHI_resource_type {
    BUFFER,
    TEXTURE,
    SHADER_CODE,
    SHADER_PROGRAM,
    GEOMETRY,
    FRAME_BUFFER
};

class RHI_resource : public std::enable_shared_from_this<RHI_resource> {
protected:
    // 修改为 std::vector 存储依赖关系
    std::vector<std::weak_ptr<RHI_resource>> m_dependencies;
    std::vector<std::weak_ptr<RHI_resource>> m_reverse_dependencies;
    RHI_resource_type m_resource_type{};
    
public:
    using Ptr = std::shared_ptr<RHI_resource>;
    
    RHI_resource(RHI_resource_type type) : m_resource_type(type) {}
    
    virtual ~RHI_resource() {
        std::vector<Ptr> dependencies;
        for (const auto& weak_dep : m_dependencies) {
            if (auto dep = weak_dep.lock()) {
                dependencies.push_back(dep);
            }
        }

        for (auto& dep : dependencies) {
            remove_dependency(dep);
        }

        std::vector<Ptr> reverse_dependencies;
        for (const auto& weak_dep : m_reverse_dependencies) {
            if (auto dep = weak_dep.lock()) {
                reverse_dependencies.push_back(dep);
            }
        }

        for (auto& dep : reverse_dependencies) {
            dep->remove_dependency(shared_from_this());
        }
    }

    void add_dependency(const Ptr& resource) {
        auto self = shared_from_this();
        // 使用find_if和自定义比较器
        auto pred = [&](const auto& weak) {
            return !weak.expired() && weak.lock() == resource;
        };
        
        if (std::find_if(m_dependencies.begin(), m_dependencies.end(), pred) == m_dependencies.end()) {
            m_dependencies.push_back(resource);
        }
        
        auto& reverse_deps = resource->m_reverse_dependencies;
        auto self_pred = [&](const auto& weak) {
            return !weak.expired() && weak.lock() == self;
        };
        
        if (std::find_if(reverse_deps.begin(), reverse_deps.end(), self_pred) == reverse_deps.end()) {
            reverse_deps.push_back(self);
        }
    }

    void remove_dependency(const Ptr& resource) {
        // 使用find_if定位元素
        auto pred = [&](const auto& weak) {
            return !weak.expired() && weak.lock() == resource;
        };
        
        auto it = std::find_if(m_dependencies.begin(), m_dependencies.end(), pred);
        if (it != m_dependencies.end()) {
            m_dependencies.erase(it);
        }
    
        auto self = shared_from_this();
        auto& reverse_deps = resource->m_reverse_dependencies;
        auto self_pred = [&](const auto& weak) {
            return !weak.expired() && weak.lock() == self;
        };
        
        auto reverse_it = std::find_if(reverse_deps.begin(), reverse_deps.end(), self_pred);
        if (reverse_it != reverse_deps.end()) {
            reverse_deps.erase(reverse_it);
        }
    }

    virtual RHI_resource_type resource_type() const { return m_resource_type; }
};

}