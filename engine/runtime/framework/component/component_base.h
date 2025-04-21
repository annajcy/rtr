#pragma once

#include "engine/runtime/context/engine_tick_context.h"
#include "engine/runtime/global/guid.h"
#include <memory>
#include <vector>

namespace rtr {

enum class Component_type {
    NODE,
    MESH_RENDERER,
    CAMERA,
    CAMERA_CONTROL,
    LIGHT,
    CUSTOM
};

class Component_base;

class Component_list : public std::enable_shared_from_this<Component_list> {

protected:
    std::vector<std::shared_ptr<Component_base>> m_components{};

public:

    void add_component(const std::shared_ptr<Component_base>& component) {
        m_components.push_back(component);
    }

    template <typename T>
    const std::shared_ptr<T> get_component() const {
        for (auto& component : m_components) {
            if (auto ptr = std::dynamic_pointer_cast<T>(component)) {
                return ptr;
            }
        }
        return nullptr;
    }

    template <typename T>
    std::shared_ptr<T> get_component() {
        for (auto& component : m_components) {
            if (auto ptr = std::dynamic_pointer_cast<T>(component)) {
                return ptr;
            }
        }
        return nullptr;
    }

    template <typename T>
    void remove_component() {
        for (auto it = m_components.begin(); it != m_components.end(); ++it) {
            if (auto ptr = std::dynamic_pointer_cast<T>(*it)) {
                m_components.erase(it);
                return;
            }
        }
    }

    template <typename T>
    bool has_component() const {
        for (auto& component : m_components) {
            if (auto ptr = std::dynamic_pointer_cast<T>(component)) {
                return true;
            }
        }
        return false;
    }

    const std::vector<std::shared_ptr<Component_base>>& components() const { return m_components; }

    void sort_components(
        std::function<bool(const std::shared_ptr<Component_base>& a, const std::shared_ptr<Component_base>& b)> compare
    ) {
        std::sort(m_components.begin(), m_components.end(), compare);
    }

};

class Component_base : public GUID {

protected:
    Component_type m_component_type{};
    bool m_is_enabled{true};
    int m_priority{0};
    std::weak_ptr<Component_list> m_component_list{};

public:
    Component_base() : m_component_type(Component_type::CUSTOM) {}
    Component_base(Component_type type) : m_component_type(type) {}
    virtual ~Component_base() = default;
    virtual void tick(const Logic_tick_context& tick_context) = 0;
    virtual void on_add_to_game_object() {}
    Component_type component_type() const { return m_component_type; }
    bool is_enabled() const { return m_is_enabled; }
    void set_enabled(bool enabled) { m_is_enabled = enabled; }
    int priority() const { return m_priority; }
    void set_priority(int priority) { m_priority = priority; }
    
    bool is_attached_to_gameobject() const { return component_list() != nullptr; }

    template<typename T> 
    std::shared_ptr<T> get_component() {
        return component_list()->get_component<T>();
	}

    template<typename T> 
    const std::shared_ptr<T> get_component() const {
        return component_list()->get_component<T>();
	}

    template<typename T> 
    std::shared_ptr<T> add_component(const std::shared_ptr<T>& component) {
        component_list()->add_component(component);
        component->set_component_list(component_list());
        component->on_add_to_game_object();
        return component;
    }

    template<typename T>
    std::shared_ptr<T> add_component() {
        auto component = std::make_shared<T>();
        component_list()->add_component(component);
        component->set_component_list(component_list());
        component->on_add_to_game_object();
        return component;
    }
    
    template<typename T>
    void remove_component(const std::shared_ptr<T>& component) {
        component_list()->remove_component<T>();
    }

    std::shared_ptr<Component_list> component_list() const { 
        if (m_component_list.expired()) {
            return nullptr;
        }
        return m_component_list.lock();
    }

    void set_component_list(const std::shared_ptr<Component_list>& component_list) { m_component_list = component_list; }

};


};