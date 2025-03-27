#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/rhi_shader_program.h"

#define LOG_STR_LEN 1024

namespace rtr {


class RHI_shader_program_OpenGL : public RHI_shader_program { 

protected:
    unsigned int m_program_id{};

public:

    RHI_shader_program_OpenGL(
        const std::unordered_map<Shader_type, unsigned int>& shaders,
        const std::unordered_map<std::string, RHI_uniform_entry>& uniforms,
        const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays) :
        RHI_shader_program(shaders, uniforms, uniform_arrays) {
        m_program_id = glCreateProgram();

        for (auto& [type, shader] : shaders) {
            attach_shader_code(shader);
        }

        if (!link()) {
            for (auto& [type, code] : m_codes) {
                detach_shader_code(code);
            }
    
            if (m_program_id) {
                glDeleteProgram(m_program_id);
            }
        }

        for (auto& [name, entry] : uniforms) {
            set_uniform(name, entry.type, entry.data);
        }

        for (auto& [name, entry] : uniform_arrays) {
            set_uniform_array(name, entry.type, entry.data, entry.count);
        }
    }

    virtual ~RHI_shader_program_OpenGL() {
        for (auto& [type, shader] : m_codes) {
            detach_shader_code(shader);
        }

        if (m_program_id) {
            glDeleteProgram(m_program_id);
        }
    }

    virtual void bind() override {
        glUseProgram(m_program_id);
    }

    virtual void unbind() override {
        glUseProgram(0);
    }

    unsigned int program_id() const {
        return m_program_id;
    }

    virtual void attach_shader_code(unsigned int code) override {
        glAttachShader(m_program_id, code);
    }

    virtual void detach_shader_code(unsigned int code) override {
        glDetachShader(m_program_id, code);
    }

    virtual bool link() override {
        glLinkProgram(m_program_id);
        return check_link_error();
    }

    bool check_link_error() {  
        char info_log[LOG_STR_LEN];
        int success{};
        glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(m_program_id, LOG_STR_LEN, nullptr, info_log);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
        } else {   
            std::cout << "INFO::SHADER::PROGRAM::LINKING_SUCCESS" << std::endl;
        }
        return success;
    }

    virtual void set_uniform(
        const std::string& name, 
        Uniform_type type, 
        const void* data
    ) override {
        int location = glGetUniformLocation(m_program_id, name.c_str());
        if (location == -1) {
            //std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_NOT_FOUND: " << name << std::endl;
            return;
        }

        switch (type) {
            case Uniform_type::FLOAT:
                glUniform1f(location, *static_cast<const float*>(data));
                break;
            case Uniform_type::INT:
                glUniform1i(location, *static_cast<const int*>(data));
                break;
            case Uniform_type::BOOL:
                glUniform1i(location, *static_cast<const bool*>(data) ? 1 : 0);
                break;
            case Uniform_type::VEC2:
                glUniform2fv(location, 1, static_cast<const float*>(data));
                break;
            case Uniform_type::VEC3:
                glUniform3fv(location, 1, static_cast<const float*>(data));
                break;
            case Uniform_type::VEC4:
                glUniform4fv(location, 1, static_cast<const float*>(data));
                break;
            case Uniform_type::IVEC2:
                glUniform2iv(location, 1, static_cast<const int*>(data));
                break;
            case Uniform_type::IVEC3:
                glUniform3iv(location, 1, static_cast<const int*>(data));
                break;
            case Uniform_type::IVEC4:
                glUniform4iv(location, 1, static_cast<const int*>(data));
                break;
            case Uniform_type::MAT2:
                glUniformMatrix2fv(location, 1, GL_FALSE, static_cast<const float*>(data));
                break;
            case Uniform_type::MAT3:
                glUniformMatrix3fv(location, 1, GL_FALSE, static_cast<const float*>(data));
                break;
            case Uniform_type::MAT4:
                glUniformMatrix4fv(location, 1, GL_FALSE, static_cast<const float*>(data));
                break;
            case Uniform_type::SAMPLER:
                glUniform1i(location, *static_cast<const int*>(data));
                break;
            default:
                std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_TYPE_NOT_SUPPORTED" << std::endl;
                break;
        }

    }

    virtual void set_uniform_array(
        const std::string& name, 
        Uniform_type type, 
        const void* data, 
        unsigned int count
    ) override {

        int location = glGetUniformLocation(m_program_id, name.c_str());
        if (location == -1) {
            //std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_NOT_FOUND: " << name << std::endl;
            return;
        } else {
            //std::cout << "INFO::SHADER::PROGRAM::UNIFORM_FOUND: " << name << " LOCATION: " << location << std::endl;
        }
       
        switch (type) {
            case Uniform_type::FLOAT:
                glUniform1fv(location, count, static_cast<const float*>(data));
                break;
            case Uniform_type::INT:
            case Uniform_type::BOOL:
                glUniform1iv(location, count, static_cast<const int*>(data));
                break;
            case Uniform_type::VEC2:
                glUniform2fv(location, count, static_cast<const float*>(data));
                break;
            case Uniform_type::VEC3:
                glUniform3fv(location, count, static_cast<const float*>(data));
                break;
            case Uniform_type::VEC4:
                glUniform4fv(location, count, static_cast<const float*>(data));
                break;
            case Uniform_type::IVEC2:
                glUniform2iv(location, count, static_cast<const int*>(data));
                break;
            case Uniform_type::IVEC3:
                glUniform3iv(location, count, static_cast<const int*>(data));
                break;
            case Uniform_type::IVEC4:
                glUniform4iv(location, count, static_cast<const int*>(data));
                break;
            case Uniform_type::MAT2:
                glUniformMatrix2fv(location, count, GL_FALSE, static_cast<const float*>(data));
                break;
            case Uniform_type::MAT3:
                glUniformMatrix3fv(location, count, GL_FALSE, static_cast<const float*>(data));
                break;
            case Uniform_type::MAT4:
                glUniformMatrix4fv(location, count, GL_FALSE, static_cast<const float*>(data));
                break;
            default:
                std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_TYPE_NOT_SUPPORTED" << std::endl;
                break;
        }

    }

    virtual unsigned int id() override {
        return m_program_id;
    }
    
};

};