#pragma once
#include "engine/runtime/global/base.h"
#include "engine/runtime/function/framework/game_object.h"

namespace rtr {

enum class Component_type {
    NODE,
    MESH_RENDERER,
    CAMERA,
    LIGHT
};

class Component_base {
public:
	using Ptr = std::shared_ptr<Component_base>;
protected:
    Component_type m_type{};

public:
    Component_base(Component_type type) : m_type(type) {}
    virtual ~Component_base() = default;
    virtual void tick(float delta_time) = 0;
    Component_type type() const { return m_type; }
};

};