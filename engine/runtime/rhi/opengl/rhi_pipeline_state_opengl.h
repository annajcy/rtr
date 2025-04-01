#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/opengl/rhi_cast_opengl.h"
#include "engine/runtime/rhi/rhi_pipeline_state.h"

namespace rtr {

class RHI_pipeline_state_OpenGL : public RHI_pipeline_state {

public:
    RHI_pipeline_state_OpenGL() : RHI_pipeline_state() { apply(); }
    RHI_pipeline_state_OpenGL(const Pipeline_state& pipeline_state) : 
    RHI_pipeline_state(pipeline_state) { apply(); }

    ~RHI_pipeline_state_OpenGL() override = default;

    void apply_blend_state() override {
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

    void apply_cull_state() override {
        if (cull_state.enable) {
            glEnable(GL_CULL_FACE);
            glCullFace(gl_cull_mode(cull_state.mode));
            glFrontFace(gl_front_face(cull_state.front_face));
        } else {
            glDisable(GL_CULL_FACE);
        }
    }

    void apply_depth_state() override {
        if (depth_state.test_enable) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(gl_depth_function(depth_state.function));
            glDepthMask((depth_state.write_enable ? GL_TRUE : GL_FALSE));
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }

    void apply_polygon_offset_state() override {
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

    void apply_stencil_state() override {
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

};

};