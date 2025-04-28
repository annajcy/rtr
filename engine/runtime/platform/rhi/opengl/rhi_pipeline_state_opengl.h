#pragma once

#include "engine/runtime/global/base.h" 
#include "../rhi_pipeline_state.h"
#include "rhi_cast_opengl.h"

namespace rtr {

class RHI_pipeline_state_OpenGL : public RHI_pipeline_state {

public:
    RHI_pipeline_state_OpenGL() : RHI_pipeline_state() { apply(); }
    RHI_pipeline_state_OpenGL(const Pipeline_state& pipeline_state) : 
    RHI_pipeline_state(pipeline_state) { apply(); }

    ~RHI_pipeline_state_OpenGL() override = default;

    void apply_blend_state() override {
        if (state.blend_state.enable) {
            glEnable(GL_BLEND);
            glBlendFuncSeparate(
                gl_blend_factor(state.blend_state.src_color_factor),
                gl_blend_factor(state.blend_state.dst_color_factor),
                gl_blend_factor(state.blend_state.src_alpha_factor),
                gl_blend_factor(state.blend_state.dst_alpha_factor)
            );
            glBlendEquationSeparate(
                gl_blend_operation(state.blend_state.color_operation),
                gl_blend_operation(state.blend_state.alpha_operation)
            );
        } else {
            glDisable(GL_BLEND);
        }
    }

    void apply_cull_state() override {
        if (state.cull_state.enable) {
            glEnable(GL_CULL_FACE);
            glCullFace(gl_cull_mode(state.cull_state.mode));
            glFrontFace(gl_front_face(state.cull_state.front_face));
        } else {
            glDisable(GL_CULL_FACE);
        }
    }

    void apply_depth_state() override {
        if (state.depth_state.test_enable) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(gl_depth_function(state.depth_state.function));
            glDepthMask((state.depth_state.write_enable ? GL_TRUE : GL_FALSE));
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }

    void apply_polygon_offset_state() override {
        if (state.polygon_offset_state.point_enabled) {
            glEnable(GL_POLYGON_OFFSET_POINT);
        } else {
            glDisable(GL_POLYGON_OFFSET_POINT);
        }

        if (state.polygon_offset_state.line_enabled) {
            glEnable(GL_POLYGON_OFFSET_LINE);
        } else {
            glDisable(GL_POLYGON_OFFSET_LINE);
        }

        if (state.polygon_offset_state.fill_enabled) {
            glEnable(GL_POLYGON_OFFSET_FILL);
        } else {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }

        if (state.polygon_offset_state.point_enabled || state.polygon_offset_state.line_enabled || state.polygon_offset_state.fill_enabled) {
            glPolygonOffset(state.polygon_offset_state.factor, state.polygon_offset_state.units);
        }
            
    }

    void apply_stencil_state() override {
        if (state.stencil_state.enable) {
            glEnable(GL_STENCIL_TEST);
            glStencilMask(state.stencil_state.mask);
            
            // 设置模板函数和操作
            glStencilFunc(
                gl_stencil_function(state.stencil_state.function),
                state.stencil_state.reference,
                state.stencil_state.function_mask
            );
            
            glStencilOp(
                gl_stencil_operation(state.stencil_state.stencil_fail),
                gl_stencil_operation(state.stencil_state.depth_fail),
                gl_stencil_operation(state.stencil_state.depth_pass)
            );
            
        } else {
            glDisable(GL_STENCIL_TEST);
        }
    }

};

};