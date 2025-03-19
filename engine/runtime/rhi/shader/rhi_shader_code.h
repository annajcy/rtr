#pragma once
#include "engine/global/base.h"

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
    Shader_type m_type{};
    std::string m_code{};

public:
    RHI_shader_code(Shader_type type, const std::string& code) : m_type(type), m_code(code) {} 
    virtual ~RHI_shader_code() = default;
    const Shader_type& type() const { return m_type; }
    const std::string& code() const { return m_code; }

    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual bool compile() = 0;

};


};