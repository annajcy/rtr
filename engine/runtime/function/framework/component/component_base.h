#pragma once

#include "engine/runtime/global/base.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace rtr {

enum class Component_type {
    NODE,
    MESH_RENDERER,
    CAMERA,
    CAMERA_CONTROL,
    LIGHT
};


class Component_base : public std::enable_shared_from_this<Component_base>, public GUID {

protected:
    Component_type m_component_type{};
    bool m_is_enabled{true};
    int m_priority{0};

    std::vector<std::weak_ptr<Component_base>> m_dependencies{};
    std::vector<std::weak_ptr<Component_base>> m_reverse_dependencies{};

public:
    Component_base(Component_type type) : m_component_type(type) {}
    virtual ~Component_base() = default;
    virtual void tick(float delta_time) = 0;
    Component_type component_type() const { return m_component_type; }
    bool is_enabled() const { return m_is_enabled; }
    void set_enabled(bool enabled) { m_is_enabled = enabled; }
    int priority() const { return m_priority; }
    void set_priority(int priority) { m_priority = priority; }

    void add_dependency(const std::shared_ptr<Component_base>& dependency) {
        m_dependencies.push_back(dependency);
        dependency->m_reverse_dependencies.push_back(shared_from_this());
    }

    void remove_dependency(const std::shared_ptr<Component_base>& dependency) {
        for (auto it = m_dependencies.begin(); it!= m_dependencies.end(); ++it) {
            if ((*it).lock() == dependency) {
                m_dependencies.erase(it);
                break;
            }
        }
        for (auto it = dependency->m_reverse_dependencies.begin(); it!= dependency->m_reverse_dependencies.end(); ++it) {
            if ((*it).lock() == shared_from_this()) {
                dependency->m_reverse_dependencies.erase(it);
                break;
            }
        }
    }

    const std::vector<std::weak_ptr<Component_base>>& dependencies() const { return m_dependencies; }
    const std::vector<std::weak_ptr<Component_base>>& reverse_dependencies() const { return m_reverse_dependencies; }



};

};