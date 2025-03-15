#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/shader/rhi_shader_code.h"

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
    }
}

class RHI_shader_code_OpenGL : public RHI_shader_code {
protected:
    unsigned int m_code_id{};

public:
    RHI_shader_code_OpenGL(Shader_type type, const std::string & code) : RHI_shader_code(type, code) {
        init();

        if (!compile()) {
            std::cout << "ERROR::SHADER::COMPILE_FAILED" << std::endl;
            destroy();
        } else {
            std::cout << "INFO::SHADER::COMPILE_SUCCESS" << std::endl;
        }
    }

    virtual ~RHI_shader_code_OpenGL() override {
        destroy();
    }

    virtual void init() override {
        m_code_id = glCreateShader(gl_shader_type(m_type));
        const char* code_ptr = m_code.c_str();
        glShaderSource(m_code_id, 1, &code_ptr, nullptr);
    }

    virtual void destroy() override {
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
            std::cerr << "Shader compiled failed" << " --" << gl_shader_type(m_type) << " " <<  m_code_id << std::endl;
            std::cerr << info_log << std::endl;
        } else {
            std::cout << "Shader compiled successfully" << " --" << gl_shader_type(m_type) << " " << m_code_id << std::endl;
        }
    }

    unsigned int code_id() const {
        return m_code_id;
    }


};


};