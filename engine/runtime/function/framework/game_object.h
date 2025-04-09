#pragma once

#include "engine/runtime/global/base.h"
#include "engine/runtime/function/framework/component/component_base.h"

namespace rtr {


class Game_object : public std::enable_shared_from_this<Game_object> {
public:
    using Ptr = std::shared_ptr<Game_object>;
	using Ptr_weak = std::weak_ptr<Game_object>;
protected:
    std::string m_name{};
	std::unordered_map<Component_type, Component_base::Ptr> m_component_map{};
    

public:

	template<typename T>
	std::shared_ptr<T> get_component()
	requires std::is_base_of<Component_base, T>::value {
		
	}

	Component_base::Ptr get_component(Component_type type) {
		return m_component_map[type];
	}

	void add_component(Component_type type, const Component_base::Ptr& component) {
		m_component_map[type] = component;
	}


};

};