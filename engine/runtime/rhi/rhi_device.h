#pragma once

#include "engine/global/base.h" 
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_binding_state.h"
#include "engine/runtime/rhi/rhi_pipeline_state.h"
#include "engine/runtime/rhi/rhi_window.h"


namespace rtr {
    
struct RHI_device_descriptor {
    unsigned int width{800};
    unsigned int height{600};
    std::string title = "RTR Engine";
};

class RHI_device {

protected:
    API_type m_api_type{};
    RHI_device_descriptor m_device_descriptor{};

    std::shared_ptr<RHI_window> m_window{};
    std::shared_ptr<RHI_pipeline_state> m_pipeline_state{};
    std::shared_ptr<RHI_binding_state> m_binding_state{};

public:
    
    RHI_device(
        API_type api_type, 
        const RHI_device_descriptor& device_descriptor
    ) : m_api_type(api_type), 
        m_device_descriptor(device_descriptor) {}

    virtual ~RHI_device() = default;

    const API_type& api_type() { return m_api_type; }
    std::shared_ptr<RHI_window>& window() { return m_window; }
    std::shared_ptr<RHI_pipeline_state>& pipeline_state() { return m_pipeline_state; }
    std::shared_ptr<RHI_binding_state>& binding_state() { return m_binding_state; }
    const RHI_device_descriptor& device_descriptor() { return m_device_descriptor; }

    virtual void check_error() = 0;


};

};



