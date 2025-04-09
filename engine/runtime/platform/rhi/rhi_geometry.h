#pragma once

#include "engine/runtime/global/base.h" 
#include "rhi_buffer.h"
#include "rhi_resource.h"

namespace rtr {

class RHI_geometry : public RHI_resource {
protected:
    RHI_buffer::Ptr m_element_buffer{};
    std::unordered_map<unsigned int, RHI_buffer::Ptr> m_vertex_buffers{};

public:
    RHI_geometry(
        const std::unordered_map<unsigned int, RHI_buffer::Ptr> &vertex_buffers, 
        const RHI_buffer::Ptr& element_buffer
    ) : RHI_resource(RHI_resource_type::GEOMETRY),
        m_vertex_buffers(vertex_buffers) , 
        m_element_buffer(element_buffer) 
    {
        for (auto &[location, buffer] : m_vertex_buffers) {
            if (buffer) {
                this->add_dependency(buffer);
            }
        }
    }

    using Ptr = std::shared_ptr<RHI_geometry>;

    virtual ~RHI_geometry() {
        for (auto &[location, buffer] : m_vertex_buffers) {
            this->remove_dependency(buffer);
        }
    }

    virtual void bind_buffers() = 0;
    virtual void bind_vertex_buffer(unsigned int location, const RHI_buffer::Ptr& vbo) = 0;

    virtual void draw(Draw_mode mode = Draw_mode::TRIANGLES) = 0;
    virtual void draw_instanced(unsigned int instance_count, Draw_mode mode = Draw_mode::TRIANGLES) = 0;


};

}
