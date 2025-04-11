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
    LIGHT,
    
};

class Component_base : public GUID {

protected:
    Component_type m_component_type{};
    bool m_is_enabled{true};

    std::vector<Component_type> m_required_component_types{};
    std::unordered_map<Component_type, std::shared_ptr<Component_base>> m_required_components{};

public:
    Component_base(Component_type type) : m_component_type(type) {}
    virtual ~Component_base() = default;
    virtual void tick(float delta_time) = 0;
    Component_type component_type() const { return m_component_type; }
    bool is_enabled() const { return m_is_enabled; }
    void set_enabled(bool enabled) { m_is_enabled = enabled; }

    template<typename T>
    std::shared_ptr<T> get_component() const requires std::is_base_of_v<Component_base, T> {
        auto it = m_required_components.find(T::c_type());
        if (it == m_required_components.end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<T>(it->second);
    }

    template<typename T>
    void add_required_component(const std::shared_ptr<T>& component) requires std::is_base_of_v<Component_base, T> {
        m_required_components[T::c_type()] = component;
        m_required_component_types.push_back(T::c_type());
    }

};

};