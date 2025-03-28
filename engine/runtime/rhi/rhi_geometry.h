#pragma once
#include "engine/global/base.h" 
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_resource.h"

namespace rtr {

struct Vertex_buffer_descriptor {
    unsigned int vbo_guid{};
    Buffer_attribute_type attribute_type{};
    Buffer_iterate_type iterate_type{};
    unsigned int unit_data_count{};
};

struct Element_buffer_descriptor {
    unsigned int ebo_guid{};
    unsigned int data_count{};
};

class RHI_geometry : public RHI_resource {
protected:
    Element_buffer_descriptor m_element_buffer{};
    std::unordered_map<unsigned int, Vertex_buffer_descriptor> m_vertex_buffers{};

public:
    RHI_geometry(
        const std::unordered_map<unsigned int, Vertex_buffer_descriptor> &vertex_buffers, 
        const Element_buffer_descriptor& element_buffer
    ) : RHI_resource(RHI_resource_type::GEOMETRY),
        m_vertex_buffers(vertex_buffers) , 
        m_element_buffer(element_buffer) { 
            RHI_resource_manager::add_resource(this);
            RHI_resource_manager::add_dependency(guid(), element_buffer.ebo_guid);
            for (auto &[_, vbo_desc] : vertex_buffers) {
                RHI_resource_manager::add_dependency(guid(), vbo_desc.vbo_guid);
            }
        }

    virtual ~RHI_geometry() {
        RHI_resource_manager::remove_resource(guid());
    }

    virtual void bind() = 0;
    virtual void unbind() = 0;

    virtual void bind_buffers() = 0;
    virtual void bind_vertex_buffer(Vertex_buffer_descriptor, unsigned int location) = 0;

    virtual void draw(Draw_mode mode = Draw_mode::TRIANGLES) = 0;
    virtual void instanced_draw(unsigned int instance_count, Draw_mode mode = Draw_mode::TRIANGLES) = 0;


};

}
