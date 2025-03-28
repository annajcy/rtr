#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_resource.h"

namespace rtr {

class RHI_shader_code : public RHI_resource
{
protected:
    Shader_type m_type{};

public:
    RHI_shader_code(
        Shader_type type, 
        const std::string& code
    ) : RHI_resource(RHI_resource_type::SHADER_CODE),
        m_type(type) {
        RHI_resource_manager::add_resource(this);
    } 

    virtual ~RHI_shader_code() {
        RHI_resource_manager::remove_resource(guid());
    }

    const Shader_type& type() const { return m_type; }
    virtual bool compile() = 0;

};


};