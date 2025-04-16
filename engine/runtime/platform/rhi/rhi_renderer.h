#pragma once

#include "engine/runtime/global/base.h" 

#include "rhi_shader_program.h"
#include "rhi_geometry.h"
#include "rhi_frame_buffer.h"
#include "rhi_window.h"
#include <memory>


namespace rtr {

struct Clear_state {
    bool color{true};
    bool depth{true};
    bool stencil{true};

    float depth_clear_value{1.0f};
    unsigned int stencil_clear_value{0x00};
    glm::vec4 color_clear_value{1.0f, 0.75f, 0.8f, 1.0f};

    static Clear_state enabled() {
        return {
            true,
            true,
            true,
            1.0f,
            0x00,
            glm::vec4(1.0f, 0.75f, 0.8f, 1.0f)
        };
    }
    
};

class RHI_renderer {
protected:
    std::shared_ptr<RHI_shader_program> m_shader_program{};
    std::shared_ptr<RHI_geometry> m_geometry{};
    std::shared_ptr<RHI_frame_buffer> m_frame_buffer{};
    Clear_state m_clear_state{};

public:
    RHI_renderer(const Clear_state& clear_state) : m_clear_state(clear_state) {}
   
    virtual ~RHI_renderer() {}

    virtual void draw(
        const std::shared_ptr<RHI_shader_program>& shader_program,
        const std::shared_ptr<RHI_geometry>& geometry,
        const std::shared_ptr<RHI_frame_buffer>& frame_buffer
    ) = 0;

    virtual void draw_instanced(
        const std::shared_ptr<RHI_shader_program>& shader_program,
        const std::shared_ptr<RHI_geometry>& geometry,
        const std::shared_ptr<RHI_frame_buffer>& frame_buffer,
        unsigned int instance_count
    ) = 0;

    virtual void clear(
        const std::shared_ptr<RHI_frame_buffer>& frame_buffer
    ) = 0;

    virtual void apply_clear_state() = 0;

    void change_clear_state(std::function<void(Clear_state&)> changer) {
        changer(m_clear_state);
        apply_clear_state();
    }

    const std::shared_ptr<RHI_shader_program>& shader_program() const { return m_shader_program; }
    const std::shared_ptr<RHI_geometry>& geometry() const { return m_geometry; }
    const std::shared_ptr<RHI_frame_buffer>& frame_buffer() const { return m_frame_buffer; }

    void set_shader_program(const std::shared_ptr<RHI_shader_program>& shader_program) {
        m_shader_program = shader_program;
    }

    void set_geometry(const std::shared_ptr<RHI_geometry>& geometry) {
        m_geometry = geometry;
    }

    void set_frame_buffer(const std::shared_ptr<RHI_frame_buffer>& frame_buffer) {
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