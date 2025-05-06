#pragma once

namespace rtr {

enum class Render_object_type {
    MATERIAL,
    GEOMETRY,
    TEXTURE,
    MEMORY_BUFFER,
};

class Render_object {
protected:
    Render_object_type m_resource_type{};

public:
    Render_object(Render_object_type resource_type) : m_resource_type(resource_type) {}
    virtual ~Render_object() = default;
    Render_object_type resource_type() const { return m_resource_type; }
};

};