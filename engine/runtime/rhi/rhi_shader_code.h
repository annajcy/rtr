#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"

namespace rtr {

class RHI_shader_code 
{
protected:
    Shader_type m_type{};

public:
    RHI_shader_code(Shader_type type, const std::string& code) :
    m_type(type) {} 
    virtual ~RHI_shader_code() = default;
    const Shader_type& type() const { return m_type; }

    virtual bool compile() = 0;
    virtual unsigned int id() = 0;

};


};