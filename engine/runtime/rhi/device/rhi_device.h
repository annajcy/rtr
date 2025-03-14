#pragma once

#include "engine/global/base.h" 
#include "engine/runtime/rhi/buffer/rhi_buffer.h"
#include "engine/runtime/rhi/geometry/rhi_geometry.h"

namespace rtr {
    
enum API_type {
    OPENGL,
    DIRECTX,
    VULKAN
};

class RHI_device {

protected:
    API_type m_api_type{};
public:
    
    RHI_device(API_type api_type) : m_api_type(api_type) {}
    virtual ~RHI_device() = default;

    const API_type& api_type() { return m_api_type; }

    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void check_error() = 0;
    
    virtual std::shared_ptr<RHI_vertex_buffer> create_vertex_buffer(
        Buffer_usage usage,
        Buffer_attribute_type attribute_type,
        unsigned int unit_count,
        unsigned int data_count,
        void* data
    ) = 0;

    virtual std::shared_ptr<RHI_element_buffer> create_element_buffer(
        Buffer_usage usage,
        unsigned int data_count,
        void* data
    ) = 0;

    virtual std::shared_ptr<RHI_geometry> create_geometry() = 0;
    
    
};


};



