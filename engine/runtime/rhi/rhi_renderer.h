#pragma once

#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_resource.h"
#include "engine/runtime/rhi/rhi_shader_program.h"
#include "engine/runtime/rhi/rhi_geometry.h"
#include "engine/runtime/rhi/rhi_frame_buffer.h"
#include "engine/runtime/rhi/rhi_window.h"
#include <memory>

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
    virtual void draw() = 0;
    virtual void draw_instanced(unsigned int instance_count) = 0;
    virtual void clear() = 0;
    virtual void apply_clear_state() = 0;

    void change_clear_state(std::function<void(Clear_state&)> changer) {
        changer(m_clear_state);
        apply_clear_state();
    }

    RHI_shader_program::Ptr& shader_program() { return m_shader_program; }
    RHI_geometry::Ptr& geometry() { return m_geometry; }
    RHI_frame_buffer::Ptr& frame_buffer() { return m_frame_buffer; }

    virtual glm::ivec4 get_viewport() const = 0;
    virtual void set_viewport(const glm::ivec4& viewport) = 0;

};

};