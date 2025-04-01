#pragma once

#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_resource.h"
#include "engine/runtime/rhi/rhi_shader_program.h"
#include "engine/runtime/rhi/rhi_geometry.h"
#include "engine/runtime/rhi/rhi_frame_buffer.h"
#include <memory>

namespace rtr {
class RHI_renderer : public RHI_resource{
protected:
    RHI_shader_program::Ptr m_shader_program{};
    RHI_geometry::Ptr m_geometry{};
    RHI_frame_buffer::Ptr m_frame_buffer{};

public:
    RHI_renderer(
        const RHI_shader_program::Ptr& shader_program,
        const RHI_geometry::Ptr& geometry,
        const RHI_frame_buffer::Ptr& frame_buffer
    ) : RHI_resource(RHI_resource_type::RENDERER) {}
    using Ptr = std::shared_ptr<RHI_renderer>;
    virtual ~RHI_renderer() {}
    virtual void draw() = 0;
    virtual void draw_instanced(unsigned int instance_count) = 0;
    RHI_shader_program::Ptr& shader_program() { return m_shader_program; }
    RHI_geometry::Ptr& geometry() { return m_geometry; }
    RHI_frame_buffer::Ptr& frame_buffer() { return m_frame_buffer; }

};

};