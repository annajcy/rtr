#pragma once

#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_frame_buffer.h"
#include "engine/runtime/platform/rhi/rhi_renderer.h"
#include "engine/runtime/platform/rhi/rhi_window.h"

namespace rtr {

struct RHI_global_render_resource {
    std::shared_ptr<RHI_device> device{};
    std::shared_ptr<RHI_window> window{};
    std::shared_ptr<RHI_renderer> renderer{};
    std::shared_ptr<RHI_screen_buffer> screen_buffer{};
    std::shared_ptr<RHI_memory_buffer_binder> memory_binder{};
    std::shared_ptr<RHI_pipeline_state> pipeline_state{};
};

enum class Render_resource_type {
    SHADER,
    MATERIAL,
    GEOMETRY,
    TEXTURE,
    MEMORY_BUFFER,
};

class Render_resource {
protected:
    Render_resource_type m_resource_type{};

public:
    Render_resource(Render_resource_type resource_type) : m_resource_type(resource_type) {}
    virtual ~Render_resource() = default;
    Render_resource_type resource_type() const { return m_resource_type; }
};

};