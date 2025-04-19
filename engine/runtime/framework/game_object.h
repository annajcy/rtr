#pragma once

#include "engine/runtime/framework/component/camera/camera_component.h"
#include "engine/runtime/framework/component/camera/camera_control_component.h"
#include "engine/runtime/framework/component/light/light_component.h"
#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer_component.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include "engine/runtime/framework/component/component_base.h"
#include "engine/runtime/global/guid.h"
#include <memory>
#include <vector>

namespace rtr {

template<typename BASE, typename T>
concept Derived_from = std::is_base_of_v<BASE, T>;

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

    const std::vector<std::shared_ptr<Component_base>>& components() const { return m_components; }

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
    
    template<typename T> requires Derived_from<Camera_component, T>
    std::shared_ptr<T> add_component(const std::shared_ptr<T>& component) {
        auto node = get_component<Node_component>();
        component->set_node(node);
        m_components.push_back(component);
        return component;
    }

    template<typename T> requires Derived_from<Camera_control_component, T>
    std::shared_ptr<T> add_component(const std::shared_ptr<T>& component) {
        auto camera = get_component<Camera_component>();
        component->set_camera(camera);
        m_components.push_back(component);
        return component;
    }

	template<typename T> requires Derived_from<Light_component, T>
    std::shared_ptr<T> add_component(const std::shared_ptr<T>& component) {
        auto node = get_component<Node_component>();
        component->set_node(node);
        m_components.push_back(component);
        return component;
    }

    template<typename T> requires Derived_from<Mesh_renderer_component, T>
    std::shared_ptr<T> add_component(const std::shared_ptr<T>& component) {
        auto node = get_component<Node_component>();
        component->set_node(node);
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

    void sort_components() {
        std::sort(m_components.begin(), m_components.end(), [](const std::shared_ptr<Component_base>& a, const std::shared_ptr<Component_base>& b) {
            return a->priority() < b->priority();
        });
    }

	void tick(const Logic_tick_context& tick_context) {
		sort_components();
		for (auto& component : m_components) {
			if (component->is_enabled()) {
				component->tick(tick_context);
			}
		}
	}
};

};