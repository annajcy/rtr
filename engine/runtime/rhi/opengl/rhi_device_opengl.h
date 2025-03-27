#pragma once

#include "engine/global/base.h" 

#include "engine/runtime/rhi/rhi_device.h"

#include "engine/runtime/rhi/opengl/rhi_buffer_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_geometry_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_window_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_shader_code_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_shader_program_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_pipeline_state_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_texture_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_frame_buffer_opengl.h"


namespace rtr {
    

class RHI_device_OpenGL : public RHI_device {
public:

    RHI_device_OpenGL(const RHI_device_descriptor& device_descriptor) : RHI_device(API_type::OPENGL, device_descriptor) { 

    }
    
    virtual ~RHI_device_OpenGL() override {  }
    
};


};



