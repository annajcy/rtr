#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/opengl/rhi_frame_buffer_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_geometry_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_shader_program_opengl.h"
#include "engine/runtime/rhi/rhi_renderer.h"
#include "engine/runtime/rhi/rhi_resource.h"
#include "engine/runtime/rhi/rhi_window.h"

namespace rtr {
class RHI_renderer_OpenGL : public RHI_renderer {

public:

    RHI_renderer_OpenGL(const Clear_state& clear_state) : RHI_renderer(clear_state) {
        apply_clear_state();
    }

    ~RHI_renderer_OpenGL() override {}
    void draw(
        const RHI_shader_program::Ptr& shader_program,
        const RHI_geometry::Ptr& geometry,
        const RHI_frame_buffer::Ptr& frame_buffer
    ) override {

        if (m_frame_buffer != frame_buffer) {
            set_frame_buffer(frame_buffer);
            if (auto gl_frame_buffer = std::dynamic_pointer_cast<RHI_frame_buffer_OpenGL>(m_frame_buffer)) {
                gl_frame_buffer->bind();
                if (!gl_frame_buffer->is_screen())
                    set_viewport({0, 0, gl_frame_buffer->width(), gl_frame_buffer->height()});
                else
                    set_viewport({0, 0, gl_frame_buffer->window()->width(), gl_frame_buffer->window()->height()});
            }
        }

        if (m_shader_program != shader_program) {
            set_shader_program(shader_program);
            if (auto gl_shader_program = std::dynamic_pointer_cast<RHI_shader_program_OpenGL>(m_shader_program)) {
                gl_shader_program->bind();
            }
        }

        if (m_geometry != geometry) {
            set_geometry(geometry);
            if (auto gl_geometry = std::dynamic_pointer_cast<RHI_geometry_OpenGL>(m_geometry)) {
                gl_geometry->bind();
            }
        }

        if (m_geometry) {
            if (auto gl_geometry = std::dynamic_pointer_cast<RHI_geometry_OpenGL>(m_geometry)) {
                gl_geometry->draw();
            }
        }
    }

    void draw_instanced(
        const RHI_shader_program::Ptr& shader_program,
        const RHI_geometry::Ptr& geometry,
        const RHI_frame_buffer::Ptr& frame_buffer,
        unsigned int instance_count
    ) override {

        if (m_frame_buffer != frame_buffer) {
            set_frame_buffer(frame_buffer);
            if (auto gl_frame_buffer = std::dynamic_pointer_cast<RHI_frame_buffer_OpenGL>(m_frame_buffer)) {
                gl_frame_buffer->bind();
                if (!gl_frame_buffer->is_screen())
                    set_viewport({0, 0, gl_frame_buffer->width(), gl_frame_buffer->height()});
                else
                    set_viewport({0, 0, gl_frame_buffer->window()->width(), gl_frame_buffer->window()->height()});
            }
        }

        if (m_shader_program != shader_program) {
            set_shader_program(shader_program);
            if (auto gl_shader_program = std::dynamic_pointer_cast<RHI_shader_program_OpenGL>(m_shader_program)) {
                gl_shader_program->bind();
            }
        }

        if (m_geometry != geometry) {
            set_geometry(geometry);
            if (auto gl_geometry = std::dynamic_pointer_cast<RHI_geometry_OpenGL>(m_geometry)) {
                gl_geometry->bind();
            }
        }

        if (m_geometry) {
            if (auto gl_geometry = std::dynamic_pointer_cast<RHI_geometry_OpenGL>(m_geometry)) {
                gl_geometry->draw_instanced(instance_count);
            }
        }
        
    }

    void apply_clear_state() override {
        glClearColor(
            m_clear_state.color_clear_value.r,
            m_clear_state.color_clear_value.g,
            m_clear_state.color_clear_value.b,
            m_clear_state.color_clear_value.a
        );

        glClearDepth(m_clear_state.depth_clear_value);
        glClearStencil(m_clear_state.stencil_clear_value);
    }

    void clear(
        const RHI_frame_buffer::Ptr& frame_buffer
    ) override {

        if (m_frame_buffer != frame_buffer) {
            set_frame_buffer(frame_buffer);
            if (auto gl_frame_buffer = std::dynamic_pointer_cast<RHI_frame_buffer_OpenGL>(m_frame_buffer)) {
                gl_frame_buffer->bind();
                if (!gl_frame_buffer->is_screen())
                    set_viewport({0, 0, gl_frame_buffer->width(), gl_frame_buffer->height()});
                else
                    set_viewport({0, 0, gl_frame_buffer->window()->width(), gl_frame_buffer->window()->height()});
            }
        }

        unsigned int clear_mask = 0;
        if (m_clear_state.color) {
            clear_mask |= GL_COLOR_BUFFER_BIT;
        }

        if (m_clear_state.depth) {
            clear_mask |= GL_DEPTH_BUFFER_BIT;
        }

        if (m_clear_state.stencil) {
            clear_mask |= GL_STENCIL_BUFFER_BIT;
        }

        glClear(clear_mask);
        
    }

     // 新增视口参数获取方法
    glm::ivec4 get_viewport() const override {
        GLint params[4];
        glGetIntegerv(GL_VIEWPORT, params);
        return {params[0], params[1], params[2], params[3]};
    }

    // 新增视口设置方法
    void set_viewport(const glm::ivec4& viewport) override {
        glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    }


};

};