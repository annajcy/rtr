#pragma once

#include "engine/runtime/global/base.h" 

#include "../rhi_shader_code.h"
#include "../rhi_shader_program.h"

#include "rhi_error_opengl.h"
#include "rhi_shader_code_opengl.h"
#include <memory>

#define LOG_STR_LEN 1024

namespace rtr {


class RHI_shader_program_OpenGL : public RHI_shader_program { 
protected:
    unsigned int m_program_id{};

public:

    RHI_shader_program_OpenGL(
        const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shader_codes,
        const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>& uniforms
    ) : RHI_shader_program(shader_codes, uniforms) {

        m_program_id = glCreateProgram();

        for (auto& [type, shader] : shader_codes) {
            attach_code(shader);
        }

        if (!link()) {
            for (auto& [type, code] : m_codes) {
                detach_code(code);
            }
    
            if (m_program_id) {
                glDeleteProgram(m_program_id);
            }
        }

        update_uniforms();
    }

    virtual ~RHI_shader_program_OpenGL() {
        
        for (auto& [type, shader] : m_codes) {
            detach_code(shader);
        }

        if (m_program_id) {
            glDeleteProgram(m_program_id);
        }
    }

    void bind() {
        glUseProgram(m_program_id);
    }

    void unbind() {
        glUseProgram(0);
    }

    unsigned int program_id() const {
        return m_program_id;
    }

    void attach_code(const std::shared_ptr<RHI_shader_code>& code) override {
        auto gl_code = std::dynamic_pointer_cast<RHI_shader_code_OpenGL>(code);
        glAttachShader(m_program_id, gl_code->code_id());
    }

    void detach_code(const std::shared_ptr<RHI_shader_code>& code) override {
        auto gl_code = std::dynamic_pointer_cast<RHI_shader_code_OpenGL>(code);
        glDetachShader(m_program_id, gl_code->code_id());
    }

    bool link() override {
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
        }
        return success;
    }

    int get_uniform_locations(const std::string& name) {
        auto location = glGetUniformLocation(m_program_id, name.c_str());
        if (location == -1) {
            std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_NOT_FOUND: " << name << std::endl;
        } 
        return location;
    }

    void set_uniform(
        const std::string& name, 
        Uniform_data_type type, 
        const void* data
    ) override {
        
        int location = get_uniform_locations(name);
        if (location == -1) return;

        switch (type) {
            case Uniform_data_type::FLOAT:
                glUniform1f(location, *static_cast<const float*>(data));
                break;
            case Uniform_data_type::INT:
                glUniform1i(location, *static_cast<const int*>(data));
                break;
            case Uniform_data_type::BOOL:
                glUniform1i(location, *static_cast<const bool*>(data) ? 1 : 0);
                break;
            case Uniform_data_type::VEC2:
                glUniform2fv(location, 1, static_cast<const float*>(data));
                break;
            case Uniform_data_type::VEC3:
                glUniform3fv(location, 1, static_cast<const float*>(data));
                break;
            case Uniform_data_type::VEC4:
                glUniform4fv(location, 1, static_cast<const float*>(data));
                break;
            case Uniform_data_type::IVEC2:
                glUniform2iv(location, 1, static_cast<const int*>(data));
                break;
            case Uniform_data_type::IVEC3:
                glUniform3iv(location, 1, static_cast<const int*>(data));
                break;
            case Uniform_data_type::IVEC4:
                glUniform4iv(location, 1, static_cast<const int*>(data));
                break;
            case Uniform_data_type::MAT2:
                glUniformMatrix2fv(location, 1, GL_FALSE, static_cast<const float*>(data));
                break;
            case Uniform_data_type::MAT3:
                glUniformMatrix3fv(location, 1, GL_FALSE, static_cast<const float*>(data));
                break;
            case Uniform_data_type::MAT4:
                glUniformMatrix4fv(location, 1, GL_FALSE, static_cast<const float*>(data));
                break;
            case Uniform_data_type::SAMPLER:
                glUniform1i(location, *static_cast<const int*>(data));
                break;
            default:
                std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_TYPE_NOT_SUPPORTED" << std::endl;
                break;
        }
    }

    void set_uniform_array(
        const std::string& name, 
        Uniform_data_type type, 
        const void* data, 
        unsigned int count
    ) override {

        int location = get_uniform_locations(name);
        if (location == -1) return;
    
        switch (type) {
            case Uniform_data_type::FLOAT:
                glUniform1fv(location, count, static_cast<const float*>(data));
                break;
            case Uniform_data_type::INT:
            case Uniform_data_type::BOOL:
                glUniform1iv(location, count, static_cast<const int*>(data));
                break;
            case Uniform_data_type::VEC2:
                glUniform2fv(location, count, static_cast<const float*>(data));
                break;
            case Uniform_data_type::VEC3:
                glUniform3fv(location, count, static_cast<const float*>(data));
                break;
            case Uniform_data_type::VEC4:
                glUniform4fv(location, count, static_cast<const float*>(data));
                break;
            case Uniform_data_type::IVEC2:
                glUniform2iv(location, count, static_cast<const int*>(data));
                break;
            case Uniform_data_type::IVEC3:
                glUniform3iv(location, count, static_cast<const int*>(data));
                break;
            case Uniform_data_type::IVEC4:
                glUniform4iv(location, count, static_cast<const int*>(data));
                break;
            case Uniform_data_type::MAT2:
                glUniformMatrix2fv(location, count, GL_FALSE, static_cast<const float*>(data));
                break;
            case Uniform_data_type::MAT3:
                glUniformMatrix3fv(location, count, GL_FALSE, static_cast<const float*>(data));
                break;
            case Uniform_data_type::MAT4:
                glUniformMatrix4fv(location, count, GL_FALSE, static_cast<const float*>(data));
                break;
            default:
                std::cout << "ERROR::SHADER::PROGRAM::UNIFORM_TYPE_NOT_SUPPORTED" << std::endl;
                break;
        }

    }

    void update_uniforms() override {
        glUseProgram(m_program_id);
        for (auto& [name, entry] : m_uniforms) {
            if (entry->is_need_update()) {
                if (entry->entry_type() == Uniform_entry_type::ARRAY) {
                    set_uniform_array(name, entry->data_type(), entry->data_ptr(), entry->data_count());                    
                } else if (entry->entry_type() == Uniform_entry_type::SINGLE) {
                    set_uniform(name, entry->data_type(), entry->data_ptr());
                }
                entry->is_need_update() = false;
            }
        }
    }
    
};

};