#pragma once

#include "engine/runtime/context/tick_context/render_tick_context.h"
#include "engine/runtime/function/render/render_resource.h"
#include "engine/runtime/resource/resource_manager.h"

namespace rtr {

class Base_pipeline {
protected:
    RHI_global_resource& m_rhi_global_resource;
    Resource_manager<std::string, Render_resource> m_render_resource_manager{};

public:
    Base_pipeline(RHI_global_resource& rhi_global_resource) : 
    m_rhi_global_resource(rhi_global_resource) {}

    virtual ~Base_pipeline() {}

    virtual void execute(const Render_tick_context& tick_context) = 0;
   
    virtual void init_ubo() = 0;
    virtual void update_ubo(const Render_tick_context& tick_context) = 0;

    virtual void init_render_passes() = 0;
    virtual void update_render_pass(const Render_tick_context& tick_context) = 0;

    virtual void update_render_resource(const Render_tick_context& tick_context) = 0;
};

}