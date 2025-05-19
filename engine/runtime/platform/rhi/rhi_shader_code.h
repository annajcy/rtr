#pragma once

#include <string>

namespace rtr {

enum class Shader_type {
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    COMPUTE,
    TESSELATION_CONTROL,
    TESSELATION_EVALUATION,
    UNKNOWN,
};

class RHI_shader_code 
{
protected:
    Shader_type m_shader_type{};

public:
    RHI_shader_code(
        Shader_type type, 
        const std::string& code
    ) :  m_shader_type(type) {} 

    virtual ~RHI_shader_code() {}

    const Shader_type& shader_type() const { return m_shader_type; }
    virtual bool compile() = 0;

};


};