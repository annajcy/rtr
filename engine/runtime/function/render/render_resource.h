#pragma once

namespace rtr {

enum class Render_resource_type {
    MATERIAL,
    GEOMETRY,
    TEXTURE
};

class Render_resource {
protected:
    Render_resource_type m_resource_type{};

public:
    Render_resource(Render_resource_type resource_type) : m_resource_type(resource_type) {}
    virtual ~Render_resource() = default;
    Render_resource_type resource_type() const { return m_resource_type; }
};

};