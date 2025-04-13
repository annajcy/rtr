#pragma once

#include "engine/runtime/function/framework/component/camera/camera_component.h"
#include "engine/runtime/function/framework/component/camera/camera_control_component.h"
#include "engine/runtime/function/framework/component/light/light_component.h"
#include "engine/runtime/function/framework/component/node/node_component.h"
#include "engine/runtime/global/base.h"
#include "engine/runtime/function/framework/component/component_base.h"
#include "engine/runtime/global/enum.h"
#include "engine/runtime/global/guid.h"

namespace rtr {

template<typename S, typename T>
concept Component_derived_from = std::is_base_of_v<S, T>;

class Game_object : public std::enable_shared_from_this<Game_object>, public GUID {

protected:
    std::string m_name{};
	std::vector<std::shared_ptr<Component_base>> m_components{};
public:

    Game_object(const std::string& name) : m_name(name) {}

    static std::shared_ptr<Game_object> create(const std::string& name) {
        return std::make_shared<Game_object>(name);
    }

    virtual ~Game_object() = default;
    const std::string& name() const { return m_name; }

	template<typename T> 
	std::shared_ptr<T> get_component() {
		for (auto& component : m_components) {
			if (std::dynamic_pointer_cast<T>(component)) {
				return std::dynamic_pointer_cast<T>(component);
			}
		}
		return nullptr;
	}

    template<typename T>
    std::shared_ptr<T> add_component(const std::shared_ptr<T>& component) {
        m_components.push_back(component);
        return component;
    }
    
    template<typename T> requires Component_derived_from<Camera_component, T>
    std::shared_ptr<T> add_component(const std::shared_ptr<T>& component) {
        component->set_node(get_component<Node_component>());
        m_components.push_back(component);
        return component;
    }

    template<typename T> requires Component_derived_from<Camera_control_component, T>
    std::shared_ptr<T> add_component(const std::shared_ptr<T>& component) {
        component->set_camera(get_component<Camera_component>());
        m_components.push_back(component);
        return component;
    }

	template<typename T> requires Component_derived_from<Light_component, T>
    std::shared_ptr<T> add_component(const std::shared_ptr<T>& component) {
        component->set_node(get_component<Node_component>());
        m_components.push_back(component);
        return component;
    }

    template<typename T>
    void remove_component(const std::shared_ptr<T>& component) {
        for (auto it = m_components.begin(); it!= m_components.end(); ++it) {
            if (*it == component) {
                m_components.erase(it);
                return;
            }
        }
    }

	void tick(float delta_time) {
		for (auto& component : m_components) {
			if (component->is_enabled()) {
				component->tick(delta_time);
			}
		}
	}
};

};