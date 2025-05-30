#pragma once

#include "engine/runtime/function/render/pipeline/base_pipeline.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include <memory>

namespace rtr {

class Render_system {

protected:
    RHI_global_resource m_global_resource{};
    std::shared_ptr<Base_pipeline> m_render_pipeline{};

public:
    Render_system(
        const RHI_global_resource &global_resource
    ) : m_global_resource(global_resource) {}

    static std::shared_ptr<Render_system> create(
        const RHI_global_resource &global_resource
        
    ) {
        return std::make_shared<Render_system>(
            global_resource
        );
    }

    const RHI_global_resource& rhi_global_resource() const {
        return m_global_resource;
    }

    RHI_global_resource& rhi_global_resource() {
        return m_global_resource;
    }

    void set_render_pipeline(const std::shared_ptr<Base_pipeline>& pipeline) {
        m_render_pipeline = pipeline;
    }

    void tick(const Render_tick_context& tick_context) {
        m_render_pipeline->update_render_resource(tick_context);
        m_render_pipeline->update_ubo(tick_context);
        m_render_pipeline->update_render_pass(tick_context);
        m_render_pipeline->execute(tick_context);
    }

};

}