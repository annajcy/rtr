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

class Component_base : public GUID {

protected:
    Component_type m_component_type{};
    bool m_is_enabled{true};

public:
    Component_base(Component_type type) : m_component_type(type) {}
    virtual ~Component_base() = default;
    virtual void tick(float delta_time) = 0;
    Component_type component_type() const { return m_component_type; }
    bool is_enabled() const { return m_is_enabled; }
    void set_enabled(bool enabled) { m_is_enabled = enabled; }
};

};