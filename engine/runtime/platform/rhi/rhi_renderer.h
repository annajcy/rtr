#pragma once

#include "engine/runtime/global/base.h" 
#include "rhi_resource.h"
#include "rhi_shader_program.h"
#include "rhi_geometry.h"
#include "rhi_frame_buffer.h"
#include "rhi_window.h"


namespace rtr {
class RHI_renderer : public RHI_resource {
protected:
    RHI_shader_program::Ptr m_shader_program{};
    RHI_geometry::Ptr m_geometry{};
    RHI_frame_buffer::Ptr m_frame_buffer{};
    Clear_state m_clear_state{};

public:
    RHI_renderer(const Clear_state& clear_state) : 
    RHI_resource(RHI_resource_type::RENDERER),
    m_clear_state(clear_state) {}
        
    using Ptr = std::shared_ptr<RHI_renderer>;
    virtual ~RHI_renderer() {}
    virtual void draw(
        const RHI_shader_program::Ptr& shader_program,
        const RHI_geometry::Ptr& geometry,
        const RHI_frame_buffer::Ptr& frame_buffer
    ) = 0;
    virtual void draw_instanced(
        const RHI_shader_program::Ptr& shader_program,
        const RHI_geometry::Ptr& geometry,
        const RHI_frame_buffer::Ptr& frame_buffer,
        unsigned int instance_count
    ) = 0;
    virtual void clear(
        const RHI_frame_buffer::Ptr& frame_buffer
    ) = 0;
    virtual void apply_clear_state() = 0;

    void change_clear_state(std::function<void(Clear_state&)> changer) {
        changer(m_clear_state);
        apply_clear_state();
    }

    const RHI_shader_program::Ptr& shader_program() const { return m_shader_program; }
    const RHI_geometry::Ptr& geometry() const { return m_geometry; }
    const RHI_frame_buffer::Ptr& frame_buffer() const { return m_frame_buffer; }

    void set_shader_program(const RHI_shader_program::Ptr& shader_program) {
        this->remove_dependency(m_shader_program);
        this->add_dependency(shader_program);
        m_shader_program = shader_program;
    }

    void set_geometry(const RHI_geometry::Ptr& geometry) {
        this->remove_dependency(m_geometry);
        this->add_dependency(geometry);
        m_geometry = geometry;
        
    }

    void set_frame_buffer(const RHI_frame_buffer::Ptr& frame_buffer) {
        this->remove_dependency(m_frame_buffer);
        this->add_dependency(frame_buffer);
        m_frame_buffer = frame_buffer;
    }

    void init() {
        set_frame_buffer(nullptr);
        set_geometry(nullptr);
        set_shader_program(nullptr);
    }

    virtual glm::ivec4 get_viewport() const = 0;
    virtual void set_viewport(const glm::ivec4& viewport) = 0;

};

};