#pragma once

#include "engine/global/base.h" 
#include "engine/runtime/rhi/buffer/rhi_buffer.h"
#include "engine/runtime/rhi/geometry/rhi_geometry.h"
#include "engine/runtime/rhi/window/rhi_window.h"
#include "engine/runtime/rhi/shader/rhi_shader_code.h"
#include "engine/runtime/rhi/shader/rhi_shader_program.h"
#include "engine/runtime/rhi/texture/rhi_texture.h"
#include "engine/runtime/rhi/state/rhi_pipeline_state.h"
#include "engine/runtime/rhi/state/rhi_binding_state.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace rtr {
    
enum class API_type {
    OPENGL,
    DIRECTX,
    VULKAN
};

struct RHI_device_descriptor {
    unsigned int width{800};
    unsigned int height{600};
    std::string title = "RTR Engine";
};


class RHI_device_cache {

public:
    std::unordered_map<unsigned int, std::shared_ptr<RHI_vertex_buffer>> vertex_buffer_cache{};
    std::unordered_map<unsigned int, std::shared_ptr<RHI_element_buffer>> element_buffer_cache{};
    std::unordered_map<unsigned int, std::shared_ptr<RHI_geometry>> geometry_cache{};

    std::unordered_map<unsigned int, std::shared_ptr<RHI_texture_2D>> texture_2D_cache{};
    std::unordered_map<unsigned int, std::shared_ptr<RHI_texture_cube_map>> texture_cube_map_cache{};

    std::unordered_map<unsigned int, std::shared_ptr<RHI_frame_buffer>> frame_buffer_cache{};

    std::unordered_map<unsigned int, std::shared_ptr<RHI_shader_code>> shader_code_cache{};
    std::unordered_map<unsigned int, std::shared_ptr<RHI_shader_program>> shader_program_cache{};

public:
    RHI_device_cache() = default;
    ~RHI_device_cache() = default;
    
};

class RHI_device {

protected:
    API_type m_api_type{};
    RHI_device_descriptor m_device_descriptor{};

    std::shared_ptr<RHI_window> m_window{};
    std::shared_ptr<RHI_pipeline_state> m_pipeline_state{};
    std::shared_ptr<RHI_binding_state> m_binding_state{};
    std::shared_ptr<RHI_device_cache> m_cache;


public:
    
    RHI_device(
        API_type api_type, 
        const RHI_device_descriptor& device_descriptor
    ) : m_api_type(api_type), 
        m_device_descriptor(device_descriptor) {}

    virtual ~RHI_device() = default;

    const API_type& api_type() { return m_api_type; }
    std::shared_ptr<RHI_window>& window() { return m_window; }
    std::shared_ptr<RHI_pipeline_state>& pipeline_state() { return m_pipeline_state; }
    std::shared_ptr<RHI_binding_state>& binding_state() { return m_binding_state; }
    std::shared_ptr<RHI_device_cache>& cache() { return m_cache; }
    const RHI_device_descriptor& device_descriptor() { return m_device_descriptor; }

    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void check_error() = 0;

    std::shared_ptr<RHI_vertex_buffer> create_vertex_buffer(
        unsigned int id,
        Buffer_usage usage,
        Buffer_attribute_type attribute_type,
        Buffer_iterate_type iterate_type,
        unsigned int unit_data_count,
        unsigned int data_count,
        const void* data
    ) {
        if (m_cache->vertex_buffer_cache.contains(id)) {
            std::cout << "Vertex buffer " << id << " already exists" << std::endl;
            return m_cache->vertex_buffer_cache[id];
        }
        else {
            auto buffer = create_vertex_buffer(usage, attribute_type, iterate_type, unit_data_count, data_count, data);
            m_cache->vertex_buffer_cache[id] = buffer;
            return buffer;
        }
    }
    
    virtual std::shared_ptr<RHI_vertex_buffer> create_vertex_buffer(
        Buffer_usage usage,
        Buffer_attribute_type attribute_type,
        Buffer_iterate_type iterate_type,
        unsigned int unit_count,
        unsigned int data_count,
        const void* data
    ) = 0;

    std::shared_ptr<RHI_element_buffer> create_element_buffer(
        unsigned int id,
        Buffer_usage usage,
        unsigned int data_count,
        const void* data
    ) {
        if (m_cache->element_buffer_cache.contains(id)) {
            std::cout << "Element buffer " << id << " already exists" << std::endl;
            return m_cache->element_buffer_cache[id];
        }
        else {
            auto buffer = create_element_buffer(usage, data_count, data);
            m_cache->element_buffer_cache[id] = buffer;
            return buffer;
        }
    }

    virtual std::shared_ptr<RHI_element_buffer> create_element_buffer(
        Buffer_usage usage,
        unsigned int data_count,
        const void* data
    ) = 0;

    std::shared_ptr<RHI_geometry> create_geometry(
        unsigned int id,
        const std::unordered_map<unsigned int, std::shared_ptr<RHI_vertex_buffer>>& vertex_buffers,
        const std::shared_ptr<RHI_element_buffer>& element_buffer
    ) {
        if (m_cache->geometry_cache.contains(id)) {
            std::cout << "Geometry " << id << " already exists" << std::endl;
            return m_cache->geometry_cache[id];
        }
        else {
            auto geometry = create_geometry(vertex_buffers, element_buffer);
            m_cache->geometry_cache[id] = geometry;
            return geometry;
        }
    }

    virtual std::shared_ptr<RHI_geometry> create_geometry(
        const std::unordered_map<unsigned int, std::shared_ptr<RHI_vertex_buffer>>& vertex_buffers,
        const std::shared_ptr<RHI_element_buffer>& element_buffer
    ) = 0;

    std::shared_ptr<RHI_shader_code> create_shader_code(
        unsigned int id,
        Shader_type type,
        const std::string& code
    ) {
        if (m_cache->shader_code_cache.contains(id)) {
            std::cout << "Shader code " << id << " already exists" << std::endl;
            return m_cache->shader_code_cache[id];
        }
        else {
            auto shader_code = create_shader_code(type, code);
            m_cache->shader_code_cache[id] = shader_code;
            return shader_code;
        }
    }

    virtual std::shared_ptr<RHI_shader_code> create_shader_code(
        Shader_type type,
        const std::string& code
    ) = 0;

    std::shared_ptr<RHI_shader_program> create_shader_program(
        unsigned int id,
        const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shaders
    ) {
        if (m_cache->shader_program_cache.contains(id)) {
            std::cout << "Shader program " << id << " already exists" << std::endl;
            return m_cache->shader_program_cache[id];
        }
        else {
            auto shader_program = create_shader_program(shaders);
            m_cache->shader_program_cache[id] = shader_program;
            return shader_program;
        }
    }

    virtual std::shared_ptr<RHI_shader_program> create_shader_program(
        const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shaders
    ) = 0;

    std::shared_ptr<RHI_shader_program> create_shader_program(
        unsigned int id,
        const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shaders,
        const std::unordered_map<std::string, RHI_uniform_entry>& uniforms,
        const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays
    ) {
        if (m_cache->shader_program_cache.contains(id)) {
            std::cout << "Shader program " << id << " already exists" << std::endl;
            return m_cache->shader_program_cache[id];
        }
        else {
            auto shader_program = create_shader_program(shaders, uniforms, uniform_arrays);
            m_cache->shader_program_cache[id] = shader_program;
            return shader_program;
        }
    }

    virtual std::shared_ptr<RHI_shader_program> create_shader_program(
        const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shaders,
        const std::unordered_map<std::string, RHI_uniform_entry>& uniforms,
        const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays
    ) = 0;

    std::shared_ptr<RHI_texture_2D> create_texture_2D(
        unsigned int id,
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        int width,
        int height,
        const unsigned char* data
    ) {
        if (m_cache->texture_2D_cache.contains(id)) {
            std::cout << "Texture 2D " << id << " already exists" << std::endl;
            return m_cache->texture_2D_cache[id];
        } else {
            auto texture = create_texture_2D(internal_format, external_format, buffer_type, width, height, data);
            m_cache->texture_2D_cache[id] = texture;
            return texture;
        }
    }

    virtual std::shared_ptr<RHI_texture_2D> create_texture_2D(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        int width,
        int height,
        const unsigned char* data
    ) = 0;

    std::shared_ptr<RHI_texture_2D> create_texture_2D(
        unsigned int id,
        int width,
        int height,
        const unsigned char* data
    ) {
        if (m_cache->texture_cube_map_cache.contains(id)) {
            std::cout << "Texture 2D " << id << " already exists" << std::endl;
            return m_cache->texture_2D_cache[id];
        } else {
            auto texture = create_texture_2D(width, height, data);
            m_cache->texture_2D_cache[id] = texture;
            return texture;
        }
    }

    virtual std::shared_ptr<RHI_texture_2D> create_texture_2D(
        int width,
        int height,
        const unsigned char* data
    ) = 0;


    std::shared_ptr<RHI_texture_cube_map> create_texture_cube_map(
        unsigned int id,
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        const std::unordered_map<Texture_cube_map_face, RHI_texture_cube_map::Face_data> &face_textures
    ) {
        if (m_cache->texture_cube_map_cache.contains(id)) {
            std::cout << "Texture cube map " << id << " already exists" << std::endl;
            return m_cache->texture_cube_map_cache[id];
        } else {
            auto texture = create_texture_cube_map(
                internal_format, 
                external_format, 
                buffer_type, 
                face_textures
            );

            m_cache->texture_cube_map_cache[id] = texture;
            return texture;
        }
    }


    virtual std::shared_ptr<RHI_texture_cube_map> create_texture_cube_map(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        const std::unordered_map<Texture_cube_map_face, RHI_texture_cube_map::Face_data> &face_textures
    ) = 0;

    std::shared_ptr<RHI_texture_cube_map> create_texture_cube_map(
        unsigned int id,
        std::unordered_map<Texture_cube_map_face, RHI_texture_cube_map::Face_data> face_textures
    ) {
        if (m_cache->texture_cube_map_cache.contains(id)) {
            std::cout << "Texture cube map " << id << " already exists" << std::endl;
            return m_cache->texture_cube_map_cache[id];
        } else {
            auto texture = create_texture_cube_map(face_textures);
            m_cache->texture_cube_map_cache[id] = texture;
            return texture;
        }
    }

    virtual std::shared_ptr<RHI_texture_cube_map> create_texture_cube_map(
        const std::unordered_map<Texture_cube_map_face, RHI_texture_cube_map::Face_data> &face_textures
    ) = 0;

  
    std::shared_ptr<RHI_frame_buffer> create_frame_buffer(
        unsigned int id,
        int width,
        int height,
        const std::vector<std::shared_ptr<RHI_texture_2D>>& color_attachments,
        const std::shared_ptr<RHI_texture_2D>& depth_attachment
    ) {
        if (m_cache->frame_buffer_cache.contains(id)) {
            std::cout << "Frame buffer " << id << " already exists" << std::endl;
            return m_cache->frame_buffer_cache[id];
        } else {
            auto frame_buffer = create_frame_buffer(width, height, color_attachments, depth_attachment);
            m_cache->frame_buffer_cache[id] = frame_buffer;
            return frame_buffer;
        }
    }

    virtual std::shared_ptr<RHI_frame_buffer> create_frame_buffer(
        int width,
        int height,
        const std::vector<std::shared_ptr<RHI_texture_2D>>& color_attachments,
        const std::shared_ptr<RHI_texture_2D>& depth_attachment
    ) = 0;

    void draw() {
        m_pipeline_state->apply();

        m_binding_state->bind();
        m_binding_state->geometry()->draw();
        m_binding_state->unbind();
    }

    void instanced_draw(int instance_count) {
        m_pipeline_state->apply();

        m_binding_state->bind();
        m_binding_state->geometry()->instanced_draw(instance_count);
        m_binding_state->unbind();
    }

    virtual void clear() = 0;

    
};


};



