#pragma once
#include "engine/global/base.h"

#include "engine/runtime/rhi/geometry/rhi_geometry.h"
#include "engine/runtime/rhi/shader/rhi_shader_program.h"
#include "engine/runtime/rhi/texture/rhi_texture.h"
#include "engine/runtime/rhi/frame_buffer/rhi_frame_buffer.h"

namespace rtr {

class RHI_binding_state {
protected:
    std::shared_ptr<RHI_geometry> m_geometry{};
    std::shared_ptr<RHI_shader_program> m_shader_program{};
    std::unordered_map<unsigned int, std::shared_ptr<RHI_texture_2D>> m_textures_2D{};
    std::unordered_map<unsigned int, std::shared_ptr<RHI_texture_cube_map>> m_textures_cube_map{};
    std::shared_ptr<RHI_frame_buffer> m_frame_buffer{};

public:

    RHI_binding_state() = default;
    virtual ~RHI_binding_state() = default;

    virtual void init() {
        m_geometry = nullptr;
        m_shader_program = nullptr;
        m_textures_2D.clear();
        m_textures_cube_map.clear();
        m_frame_buffer = nullptr;
    }

    virtual void bind() {
        if (m_frame_buffer) {
            m_frame_buffer->bind();
        }

        m_geometry->bind();
        
        for (auto& [location, texture] : m_textures_2D) {
            texture->bind(location);
        }
        for (auto& [location, texture] : m_textures_cube_map) {
            texture->bind(location);
        }

        m_shader_program->bind();
        m_shader_program->update_uniforms();

    }

    virtual void unbind() {

        m_geometry->unbind();
        m_shader_program->unbind();

        for (auto& [location, texture] : m_textures_2D) {
            texture->unbind();
        }

        for (auto& [location, texture] : m_textures_cube_map) {
            texture->unbind();
        }

        if (m_frame_buffer) {
            m_frame_buffer->unbind();
        }
    }

    virtual bool is_valid() {
        return m_geometry && m_shader_program;
    }

    std::shared_ptr<RHI_geometry>& geometry() { return m_geometry; }
    std::shared_ptr<RHI_shader_program>& shader_program() { return m_shader_program; }
    std::shared_ptr<RHI_frame_buffer>& frame_buffer() { return m_frame_buffer; }
    std::unordered_map<unsigned int, std::shared_ptr<RHI_texture_2D>>& textures_2D() { return m_textures_2D; }
    std::unordered_map<unsigned int, std::shared_ptr<RHI_texture_cube_map>>& textures_cube_map() { return m_textures_cube_map; }
    
};

};