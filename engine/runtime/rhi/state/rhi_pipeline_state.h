#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/struct.h"

namespace rtr {

class RHI_pipeline_state : public Pipeline_state {

protected:
    
    unsigned int m_clear_mask{};

public:

    RHI_pipeline_state() : Pipeline_state() {}
    virtual ~RHI_pipeline_state() = default;
    
    void init() {
        apply();
    }
    
    void apply() {
        apply_blend_state();
        apply_cull_state();
        apply_depth_state();
        apply_polygon_offset_state();
        apply_stencil_state();
        apply_clear_state();
    }

    virtual void apply_blend_state() = 0;
    virtual void apply_cull_state() = 0;
    virtual void apply_depth_state() = 0;
    virtual void apply_polygon_offset_state() = 0;
    virtual void apply_stencil_state() = 0;
    virtual void apply_clear_state() = 0;


    unsigned int clear_mask() { return m_clear_mask; }

};

};