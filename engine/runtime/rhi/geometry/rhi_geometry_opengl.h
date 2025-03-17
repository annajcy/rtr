#pragma once
#include "engine/global/base.h" 
#include "engine/runtime/rhi/geometry/rhi_geometry.h"
#include "engine/runtime/rhi/buffer/rhi_buffer_opengl.h"

namespace rtr {

inline constexpr unsigned int gl_atribute_type(Buffer_attribute_type type) {
    switch (type) {
    case Buffer_attribute_type::FLOAT: return GL_FLOAT;
    case Buffer_attribute_type::INT: return GL_INT;
    case Buffer_attribute_type::UINT: return GL_UNSIGNED_INT;
    case Buffer_attribute_type::BOOL: return GL_BOOL;
    default: return 0;
    }
}

class RHI_geometry_OpenGL : public RHI_geometry {
protected:
    unsigned int m_vao{};

public:
    RHI_geometry_OpenGL(
        const std::unordered_map<unsigned int, std::shared_ptr<RHI_vertex_buffer>> &vertex_buffers, 
        const std::shared_ptr<RHI_element_buffer>& element_buffer
    ) : RHI_geometry(vertex_buffers, element_buffer) { 
        init(); 
        bind_buffers();
    }

    virtual ~RHI_geometry_OpenGL() override { 
        destroy(); 
    }

    virtual void init() override {
        glGenVertexArrays(1, &m_vao);
    }

    virtual void bind() override {
        glBindVertexArray(m_vao);
    }

    virtual void unbind() override {
        glBindVertexArray(0);
    }

    virtual void destroy() override {
        if (m_vao) {
            glDeleteVertexArrays(1, &m_vao);
            m_vao = 0;
        }
    }

    virtual void bind_vertex_buffer(const std::shared_ptr<RHI_vertex_buffer>& vbo, unsigned int location) override {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, vbo->unit_count(), gl_atribute_type(vbo->attribute_type()), GL_FALSE, vbo->unit_size(), (void*)0);
    }

    virtual void draw() override {
        auto gl_element_buffer = std::dynamic_pointer_cast<RHI_element_buffer_OpenGL>(m_element_buffer);

        if (gl_element_buffer) {
            bind();
            glDrawElements(GL_TRIANGLES, gl_element_buffer->data_count(), GL_UNSIGNED_INT, 0);
            unbind();
        }
    }

    virtual void instanced_draw(unsigned int instance_count) override {
        auto gl_element_buffer = std::dynamic_pointer_cast<RHI_element_buffer_OpenGL>(m_element_buffer);

        if (gl_element_buffer) {
            bind();
            glDrawElementsInstanced(GL_TRIANGLES, gl_element_buffer->data_count(), GL_UNSIGNED_INT, 0, instance_count);
            unbind();
        }
    }

};

}
