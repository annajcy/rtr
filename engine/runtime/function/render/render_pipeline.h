#pragma once

#include "engine/runtime/function/render/render_pass.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include <memory>

namespace rtr {

class Render_pipeline {
public:
    Render_pipeline() {}
    virtual ~Render_pipeline() {}
    virtual void execute(const Render_tick_context& tick_context) = 0;
    virtual void update_ubo(const Render_tick_context& tick_context) = 0;
};

class Test_render_pipeline : public Render_pipeline {

public:
    Test_render_pipeline() {}
    ~Test_render_pipeline() {}

    void execute(const Render_tick_context& tick_context) override {
        tick_context.renderer->clear(tick_context.screen_frame_buffer);
    }

    void update_ubo(const Render_tick_context& tick_context) override {
        
    }

};

class Forward_render_pipeline : public Render_pipeline {
public:
    Forward_render_pipeline() {}
    ~Forward_render_pipeline() {}

    void execute(const Render_tick_context& tick_context) override {

    }

    void update_ubo(const Render_tick_context& tick_context) override {
        
    }

};

class Deferred_render_pipeline : public Render_pipeline {
public:
    Deferred_render_pipeline() {}
    ~Deferred_render_pipeline() {}


    void execute(const Render_tick_context& tick_context) override {
        
    }

    void update_ubo(const Render_tick_context& tick_context) override {
        
    }

};

}