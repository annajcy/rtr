#pragma once
#include "engine/global/base.h" 
#include "engine/runtime/rhi/buffer/rhi_buffer.h"

namespace rtr {

class RHI_geometry {
protected:
    std::vector<std::shared_ptr<RHI_vertex_buffer>> m_vertex_buffers{};
    std::shared_ptr<RHI_element_buffer> m_index_buffer{};

public:
    RHI_geometry() = default;
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

        int location = 0;
        for (auto& vbo : m_vertex_buffers) {
            vbo->bind();
            bind_vertex_buffer(vbo, location ++);
            vbo->unbind();
        }

        if (m_index_buffer) {
            m_index_buffer->bind();
        }

        unbind();
    }

    void add_vertex_buffer(const std::shared_ptr<RHI_vertex_buffer>& buffer) {
        m_vertex_buffers.push_back(buffer);
    }

    void add_vertex_buffers(const std::vector<std::shared_ptr<RHI_vertex_buffer>>& buffers) {
        m_vertex_buffers.insert(m_vertex_buffers.end(), buffers.begin(), buffers.end());
    }

    void set_vertex_buffers(const std::vector<std::shared_ptr<RHI_vertex_buffer>>& buffers) {
        m_vertex_buffers = buffers;
    }

    void set_index_buffer(const std::shared_ptr<RHI_element_buffer>& buffer) {
        m_index_buffer = buffer;
    }

};

}
