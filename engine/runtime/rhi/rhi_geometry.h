#pragma once
#include "engine/global/base.h" 
#include "engine/runtime/enum.h"

namespace rtr {

struct Vertex_buffer_descriptor {
    unsigned int vbo{};
    Buffer_attribute_type attribute_type{};
    Buffer_iterate_type iterate_type{};
    unsigned int unit_data_count{};
};

struct Element_buffer_descriptor {
    unsigned int ebo{};
    unsigned int data_count{};
};

class RHI_geometry {
protected:
    Element_buffer_descriptor m_element_buffer{};
    std::unordered_map<unsigned int, Vertex_buffer_descriptor> m_vertex_buffers{};

public:
    RHI_geometry(
        const std::unordered_map<unsigned int, Vertex_buffer_descriptor> &vertex_buffers, 
        const Element_buffer_descriptor& element_buffer
    ) : m_vertex_buffers(vertex_buffers) , 
        m_element_buffer(element_buffer) { }

    virtual ~RHI_geometry() = default;
    virtual void bind() = 0;
    virtual void unbind() = 0;

    virtual void bind_buffers() = 0;
    virtual void bind_vertex_buffer(Vertex_buffer_descriptor, unsigned int location) = 0;

    virtual void draw(Draw_mode mode = Draw_mode::TRIANGLES) = 0;
    virtual void instanced_draw(unsigned int instance_count, Draw_mode mode = Draw_mode::TRIANGLES) = 0;
    virtual unsigned int id() = 0;


};

}
