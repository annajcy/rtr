#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/state/rhi_pipeline_state.h"


namespace rtr {

inline constexpr unsigned int gl_blend_factor(Blend_factor factor) {
    switch (factor) {
        case Blend_factor::ZERO: return GL_ZERO;
        case Blend_factor::ONE: return GL_ONE;
        case Blend_factor::SRC_COLOR: return GL_SRC_COLOR;
        case Blend_factor::ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
        case Blend_factor::DST_COLOR: return GL_DST_COLOR;
        case Blend_factor::ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
        case Blend_factor::SRC_ALPHA: return GL_SRC_ALPHA;
        case Blend_factor::ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
        case Blend_factor::DST_ALPHA: return GL_DST_ALPHA;
        case Blend_factor::ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
        default: return GL_ZERO;
    }
}

inline constexpr unsigned int gl_blend_operation(Blend_operation operation) {
    switch (operation) {
        case Blend_operation::ADD: return GL_FUNC_ADD;
        case Blend_operation::SUBTRACT: return GL_FUNC_SUBTRACT;
        case Blend_operation::REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
        case Blend_operation::MIN: return GL_MIN;
        case Blend_operation::MAX: return GL_MAX;
        default: return GL_FUNC_ADD;
    }
}

inline constexpr unsigned int gl_cull_mode(Cull_mode mode) {
    switch (mode) {
        case Cull_mode::FRONT: return GL_FRONT;
        case Cull_mode::BACK: return GL_BACK;
        case Cull_mode::FRONT_AND_BACK: return GL_FRONT_AND_BACK;
        default: return GL_BACK;
    }
}

inline constexpr unsigned int gl_front_face(Front_face face) {
    switch (face) {
        case Front_face::CLOCKWISE: return GL_CW;
        case Front_face::COUNTER_CLOCKWISE : return GL_CCW;
        default: return GL_CCW;
    }
}

inline constexpr unsigned int gl_depth_function(Depth_function function) {
    switch (function) {
        case Depth_function::NEVER: return GL_NEVER;
        case Depth_function::LESS: return GL_LESS;
        case Depth_function::EQUAL: return GL_EQUAL;
        case Depth_function::LESS_EQUAL : return GL_LEQUAL;
        case Depth_function::GREATER: return GL_GREATER;
        case Depth_function::NOT_EQUAL: return GL_NOTEQUAL;
        case Depth_function::GREATER_EQUAL : return GL_GEQUAL;
        case Depth_function::ALWAYS: return GL_ALWAYS;
        default: return GL_LESS;
    }
}

inline constexpr unsigned int gl_stencil_operation(Stencil_operation operation) {
    switch (operation) {
        case Stencil_operation::KEEP: return GL_KEEP;
        case Stencil_operation::ZERO: return GL_ZERO;
        case Stencil_operation::REPLACE: return GL_REPLACE;
        case Stencil_operation::INCR: return GL_INCR;
        case Stencil_operation::INCR_WRAP: return GL_INCR_WRAP;
        case Stencil_operation::DECR: return GL_DECR;
        case Stencil_operation::DECR_WRAP: return GL_DECR_WRAP;
        case Stencil_operation::INVERT: return GL_INVERT;
        default: return GL_KEEP;
    }
}

inline constexpr unsigned int gl_stencil_function(Stencil_function function) {
    switch (function) {
        case Stencil_function::NEVER: return GL_NEVER;
        case Stencil_function::LESS: return GL_LESS;
        case Stencil_function::EQUAL: return GL_EQUAL;
        case Stencil_function::LESS_EQUAL : return GL_LEQUAL;
        case Stencil_function::GREATER: return GL_GREATER;
        case Stencil_function::NOT_EQUAL: return GL_NOTEQUAL;
        case Stencil_function::GREATER_EQUAL : return GL_GEQUAL;
        case Stencil_function::ALWAYS: return GL_ALWAYS;
        default: return GL_LESS;
    }
}


class RHI_pipeline_state_OpenGL : public RHI_pipeline_state {

public:
    RHI_pipeline_state_OpenGL() : RHI_pipeline_state() { init(); }

    ~RHI_pipeline_state_OpenGL() override = default;

    virtual void apply_blend_state() override {
        if (blend_state.enable) {
            glEnable(GL_BLEND);
            glBlendFuncSeparate(
                gl_blend_factor(blend_state.src_color_factor),
                gl_blend_factor(blend_state.dst_color_factor),
                gl_blend_factor(blend_state.src_alpha_factor),
                gl_blend_factor(blend_state.dst_alpha_factor)
            );
            glBlendEquationSeparate(
                gl_blend_operation(blend_state.color_operation),
                gl_blend_operation(blend_state.alpha_operation)
            );
        } else {
            glDisable(GL_BLEND);
        }
    }

    virtual void apply_cull_state() override {
        if (cull_state.enable) {
            glEnable(GL_CULL_FACE);
            glCullFace(gl_cull_mode(cull_state.mode));
            glFrontFace(gl_front_face(cull_state.front_face));
        } else {
            glDisable(GL_CULL_FACE);
        }
    }

    virtual void apply_depth_state() override {
        if (depth_state.test_enable) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(gl_depth_function(depth_state.function));
            glDepthMask((depth_state.write_enable ? GL_TRUE : GL_FALSE));
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }

    virtual void apply_polygon_offset_state() override {
        if (polygon_offset_state.point_enabled) {
            glEnable(GL_POLYGON_OFFSET_POINT);
        } else {
            glDisable(GL_POLYGON_OFFSET_POINT);
        }

        if (polygon_offset_state.line_enabled) {
            glEnable(GL_POLYGON_OFFSET_LINE);
        } else {
            glDisable(GL_POLYGON_OFFSET_LINE);
        }

        if (polygon_offset_state.fill_enabled) {
            glEnable(GL_POLYGON_OFFSET_FILL);
        } else {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }

        if (polygon_offset_state.point_enabled || polygon_offset_state.line_enabled || polygon_offset_state.fill_enabled) {
            glPolygonOffset(polygon_offset_state.factor, polygon_offset_state.units);
        }
            
    }

    virtual void apply_stencil_state() override {
        if (stencil_state.enable) {
            glEnable(GL_STENCIL_TEST);
            glStencilMask(stencil_state.mask);
            
            // 设置模板函数和操作
            glStencilFunc(
                gl_stencil_function(stencil_state.function),
                stencil_state.reference,
                stencil_state.function_mask
            );
            
            glStencilOp(
                gl_stencil_operation(stencil_state.stencil_fail),
                gl_stencil_operation(stencil_state.depth_fail),
                gl_stencil_operation(stencil_state.depth_pass)
            );
            
        } else {
            glDisable(GL_STENCIL_TEST);
        }
    }

    virtual void apply_clear_state() override {
        m_clear_mask = 0;
        if (clear_state.color) m_clear_mask |= GL_COLOR_BUFFER_BIT;
        if (clear_state.depth) m_clear_mask |= GL_DEPTH_BUFFER_BIT;
        if (clear_state.stencil) m_clear_mask |= GL_STENCIL_BUFFER_BIT;

        glClearColor(
            clear_state.color_clear_value.r,
            clear_state.color_clear_value.g,
            clear_state.color_clear_value.b,
            clear_state.color_clear_value.a
        );

        glClearDepth(clear_state.depth_clear_value);
        glClearStencil(clear_state.stencil_clear_value);

    }

};

};