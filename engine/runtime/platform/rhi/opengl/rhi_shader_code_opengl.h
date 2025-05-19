#pragma once

#include "engine/runtime/global/base.h" 
#include "../rhi_shader_code.h"

#include <cassert>

#define LOG_STR_LEN 1024

namespace rtr {


inline constexpr unsigned int gl_shader_type(Shader_type type) {
    switch (type) {
        case Shader_type::VERTEX:
            return GL_VERTEX_SHADER;
        case Shader_type::FRAGMENT:
            return GL_FRAGMENT_SHADER;
        case Shader_type::GEOMETRY:
            return GL_GEOMETRY_SHADER;
        case Shader_type::COMPUTE:
            return GL_COMPUTE_SHADER;
        case Shader_type::TESSELATION_CONTROL:
            return GL_TESS_CONTROL_SHADER;
        case Shader_type::TESSELATION_EVALUATION:
            return GL_TESS_EVALUATION_SHADER;
        default:
            return 0;
    }
}

inline constexpr std::string gl_shader_type_str(Shader_type type) {
    switch (type) {
        case Shader_type::VERTEX:
            return "GL_VERTEX_SHADER";
        case Shader_type::FRAGMENT:
            return "GL_FRAGMENT_SHADER";
        case Shader_type::GEOMETRY:
            return "GL_GEOMETRY_SHADER";
        case Shader_type::COMPUTE:
            return "GL_COMPUTE_SHADER";
        case Shader_type::TESSELATION_CONTROL:
            return "GL_TESS_CONTROL_SHADER";
        case Shader_type::TESSELATION_EVALUATION:
            return "GL_TESS_EVALUATION_SHADER";
        default:
            return "UNKNOWN_SHADER_TYPE";
    }
}

class RHI_shader_code_OpenGL : public RHI_shader_code {
protected:
    unsigned int m_code_id{};

public:
    RHI_shader_code_OpenGL(Shader_type type, const std::string & code) : RHI_shader_code(type, code) {
        m_code_id = glCreateShader(gl_shader_type(m_shader_type));
        const char* code_ptr = code.c_str();
        glShaderSource(m_code_id, 1, &code_ptr, nullptr);

        if (!compile()) {
            std::cout << "ERROR::SHADER::COMPILE_FAILED" << std::endl;
            std::cout << code << std::endl;
            glDeleteShader(m_code_id);
            assert(false);
        }
    }

    virtual ~RHI_shader_code_OpenGL() override {
        if (m_code_id) {
            glDeleteShader(m_code_id);
        }
    }

    virtual bool compile() override {
        glCompileShader(m_code_id);
        return check_complie_error();
    }

    bool check_complie_error() {
        char info_log[LOG_STR_LEN];
        int success{};
        glGetShaderiv(m_code_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(m_code_id, LOG_STR_LEN, nullptr, info_log);
            std::cerr << info_log << std::endl;
        }
        return success;
    }

    unsigned int code_id() const { return m_code_id; }
};


};