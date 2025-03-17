#pragma once
#include "engine/global/base.h" 
#include "engine/runtime/rhi/buffer/rhi_buffer.h"

namespace rtr {

class RHI_geometry {
protected:
    std::shared_ptr<RHI_element_buffer> m_element_buffer{};
    std::unordered_map<unsigned int, std::shared_ptr<RHI_vertex_buffer>> m_vertex_buffers{};

public:
    RHI_geometry(
        const std::unordered_map<unsigned int, std::shared_ptr<RHI_vertex_buffer>> &vertex_buffers, 
        const std::shared_ptr<RHI_element_buffer>& element_buffer
    ) : m_vertex_buffers(vertex_buffers) , 
        m_element_buffer(element_buffer) { }

    virtual ~RHI_geometry() = default;
    virtual void init() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void destroy() = 0;
    virtual void bind_vertex_buffer(const std::shared_ptr<RHI_vertex_buffer>& vbo, unsigned int location) = 0;
    virtual void draw() = 0;
    virtual void instanced_draw(unsigned int instance_count) = 0;

    void bind_buffers() {
        
        bind();

        for (auto& [location, vbo] : m_vertex_buffers) {
            vbo->bind();
            bind_vertex_buffer(vbo, location);
            vbo->unbind();
        }

        if (m_element_buffer) {
            m_element_buffer->bind();
        } else {
            std::cout << "No element buffer" << std::endl;
            assert(false);
        }

        unbind();
    }


};

}
