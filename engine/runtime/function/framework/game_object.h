#pragma once

#include "engine/runtime/global/base.h"
#include "engine/runtime/function/framework/component/component_base.h"
#include "engine/runtime/global/guid.h"
#include <memory>
#include <unordered_map>

namespace rtr {

class Game_object : public std::enable_shared_from_this<Game_object>, public GUID {
public:
	inline static std::unordered_map<unsigned int, std::weak_ptr<Game_object>> s_game_object_map{};

protected:
    std::string m_name{};
	std::unordered_map<Component_type, std::shared_ptr<Component_base>> m_component_map{};

public:

    Game_object(const std::string& name) : m_name(name) {}

    static std::shared_ptr<Game_object> create(const std::string& name) {
        return std::make_shared<Game_object>(name);
    }

    virtual ~Game_object() = default;
    const std::string& name() const { return m_name; }

	template<typename T>
	std::shared_ptr<T> get_component() requires std::is_base_of<Component_base, T>::value {
		auto type = T::type();
		if (m_component_map.find(type) == m_component_map.end()) {
			return nullptr;
		}
		return std::dynamic_pointer_cast<T>(m_component_map[type]);
	}

	template<typename T>
	void add_component(const std::shared_ptr<T>& component) requires std::is_base_of<Component_base, T>::value {
		for (auto& type: component->required_component_types()) {
			component->add_required_component(get_component<T>());
		}
		m_component_map[component->type()] = component;
	}

	template<typename T>
	void remove_component() requires std::is_base_of<Component_base, T>::value {
		if (m_component_map.find(T::type()) == m_component_map.end()) {
			return;
		}
		auto component_ptr = m_component_map[T::type()];
		if (component_ptr != nullptr) {
			s_game_object_map.erase(component_ptr->guid());
		}
		m_component_map.erase(T::type());
	}

	void tick(float delta_time) {
		for (auto& [type, component] : m_component_map) {
			if (component->is_enabled()) {
				component->tick(delta_time);
			}
		}
	}

	static std::shared_ptr<Game_object> get_onwer_game_object(const std::shared_ptr<Component_base>& component) {
		if (component == nullptr) {
			return nullptr;
		}
		return s_game_object_map[component->guid()].lock();
	}

};

};