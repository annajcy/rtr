#pragma once

#include "engine/runtime/global/base.h" 
#include "../rhi_buffer.h"
#include "../rhi_cast.h"
#include "../rhi_geometry.h"

#include "rhi_buffer_opengl.h"
#include "rhi_cast_opengl.h"
#include "rhi_error_opengl.h"

namespace rtr {

class RHI_geometry_OpenGL : public RHI_geometry {
protected:
    unsigned int m_vao{};

public:
    RHI_geometry_OpenGL(
        const std::unordered_map<unsigned int, RHI_buffer::Ptr> &vertex_buffers, 
        const RHI_buffer::Ptr& element_buffer
    ) : RHI_geometry(vertex_buffers, element_buffer) { 
        glGenVertexArrays(1, &m_vao);
        bind_buffers();
    }

    virtual ~RHI_geometry_OpenGL() override { 
        if (m_vao) {
            glDeleteVertexArrays(1, &m_vao);
        }
    }

    void bind() {
        glBindVertexArray(m_vao);
    }

    void unbind() {
        glBindVertexArray(0);
    }

    virtual void bind_vertex_buffer(unsigned int location, const RHI_buffer::Ptr &vbo) override {
        
        auto gl_vbo = std::dynamic_pointer_cast<RHI_vertex_buffer_OpenGL>(vbo);
        
        if (!gl_vbo) {
            return;
        }

        glBindBuffer(GL_ARRAY_BUFFER, gl_vbo->buffer_id()); 

        glEnableVertexAttribArray(location);
        
        glVertexAttribPointer(
            location, 
            gl_vbo->unit_data_count(), 
            gl_buffer_data_type(gl_vbo->buffer_data_type()), 
            GL_FALSE, 
            gl_vbo->unit_data_size(), 
            (void*)0
        );

        gl_check_error();
        glVertexAttribDivisor(location, gl_vbo->iterate_type() == Buffer_iterate_type::PER_INSTANCE ? 1 : 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);  
    }

    virtual void bind_buffers() override {
        glBindVertexArray(m_vao);

        for (auto& [location, vbo] : m_vertex_buffers) {
            bind_vertex_buffer(location, vbo);
        }

        auto gl_element_buffer = std::dynamic_pointer_cast<RHI_element_buffer_OpenGL>(m_element_buffer);
        if (gl_element_buffer) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_element_buffer->buffer_id());
        }

        glBindVertexArray(0);
    }
 
    virtual void draw(Draw_mode mode = Draw_mode::TRIANGLES) override {
        if (auto gl_element_buffer = std::dynamic_pointer_cast<RHI_element_buffer_OpenGL>(m_element_buffer)) {
            glBindVertexArray(m_vao);
            glDrawElements(gl_draw_mode(mode), gl_element_buffer->data_count(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

    virtual void draw_instanced(unsigned int instance_count, Draw_mode mode = Draw_mode::TRIANGLES) override {
        if (auto gl_element_buffer = std::dynamic_pointer_cast<RHI_element_buffer_OpenGL>(m_element_buffer)) {
            glBindVertexArray(m_vao);
            glDrawElementsInstanced(gl_draw_mode(mode), gl_element_buffer->data_count(), GL_UNSIGNED_INT, 0, instance_count);
            glBindVertexArray(0);
        }
    }

    unsigned int geometry_id() const {
        return m_vao;
    }
};

}
