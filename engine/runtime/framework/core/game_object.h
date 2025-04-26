#pragma once

#include "engine/runtime/framework/component/component_base.h"
#include <memory>

namespace rtr {

class Game_object : public std::enable_shared_from_this<Game_object>{

protected:
    std::string m_name{};
    std::shared_ptr<Component_list> m_component_list{};
    
public:

    Game_object(const std::string& name) : m_name(name), m_component_list(std::make_shared<Component_list>()) {}

    static std::shared_ptr<Game_object> create(const std::string& name) {
        return std::make_shared<Game_object>(name);
    }

    virtual ~Game_object() = default;
    const std::string& name() const { return m_name; }

    const std::shared_ptr<Component_list>& component_list() const { return m_component_list; }

	template<typename T> 
    std::shared_ptr<T> get_component() {
        return m_component_list->get_component<T>();
	}

    template<typename T> 
    const std::shared_ptr<T> get_component() const {
        return m_component_list->get_component<T>();
	}

    template<typename T> 
    std::shared_ptr<T> add_component(const std::shared_ptr<T>& component) {
        m_component_list->add_component(component);
        component->set_component_list(m_component_list);
        component->on_add_to_game_object();
        return component;
    }

    template<typename T>
    std::shared_ptr<T> add_component() {
        auto component = std::make_shared<T>();
        m_component_list->add_component(component);
        component->set_component_list(m_component_list);
        component->on_add_to_game_object();
        return component;
    }
    
    template<typename T>
    void remove_component(const std::shared_ptr<T>& component) {
        m_component_list->remove_component<T>();
    }

    void sort_components() {
        m_component_list->sort_components([](const std::shared_ptr<Component_base>& a, const std::shared_ptr<Component_base>& b) {
            return a->priority() < b->priority();
        });
    }

	void tick(const Logic_tick_context& tick_context) {
		sort_components();
		for (auto& component : m_component_list->components()) {
			if (component->is_enabled()) {
				component->tick(tick_context);
			}
		}
	}
    
};

};