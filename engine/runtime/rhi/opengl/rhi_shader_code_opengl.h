#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/opengl/gl_cast.h"
#include "engine/runtime/rhi/rhi_shader_code.h"

#define LOG_STR_LEN 1024

namespace rtr {

class RHI_shader_code_OpenGL : public RHI_shader_code {
protected:
    unsigned int m_code_id{};

public:
    RHI_shader_code_OpenGL(Shader_type type, const std::string & code) : RHI_shader_code(type, code) {
        m_code_id = glCreateShader(gl_shader_type(m_type));
        const char* code_ptr = code.c_str();
        glShaderSource(m_code_id, 1, &code_ptr, nullptr);

        if (!compile()) {
            std::cout << "ERROR::SHADER::COMPILE_FAILED" << std::endl;
            glDeleteShader(m_code_id);
        } else {
            std::cout << "INFO::SHADER::COMPILE_SUCCESS" << std::endl;
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
            std::cerr << "Shader compiled failed" << " --" << gl_shader_type_str(m_type) << " " <<  m_code_id << std::endl;
            std::cerr << info_log << std::endl;
        } else {
            std::cout << "Shader compiled successfully" << " --" << gl_shader_type_str(m_type) << " " << m_code_id << std::endl;
        }
        return success;
    }

    virtual unsigned int id() override {
        return m_code_id;
    }

};


};