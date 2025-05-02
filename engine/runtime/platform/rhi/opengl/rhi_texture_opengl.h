#pragma once

#include "engine/runtime/global/base.h" 

#include "../rhi_texture.h"
#include "engine/runtime/global/enum.h"
#include "glm/ext/vector_float4.hpp"
#include "rhi_cast_opengl.h"
#include <exception>
#include <memory>
#include <stdexcept>

namespace rtr {

class RHI_texture_OpenGL : public RHI_texture {
protected:
    unsigned int m_texture_id{};

public:
    RHI_texture_OpenGL(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_type type,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters
    ) : RHI_texture(
        width,
        height,
        mipmap_levels,
        type,
        internal_format,
        wraps,
        filters
    ) {}
        
    virtual ~RHI_texture_OpenGL() {
        if (m_texture_id) {
            glDeleteTextures(1, &m_texture_id);
        }
    }

    void bind_to_unit(unsigned int location) override {
        glBindTextureUnit(location, m_texture_id);
    }

    unsigned int texture_id() const {
        return m_texture_id;
    }

    void generate_mipmap() override {
        glGenerateTextureMipmap(m_texture_id);
    }

    void set_filter(Texture_filter_target target, Texture_filter filter) override {
        glTextureParameteri(
            m_texture_id, 
            gl_texture_filter_target(target), 
            gl_texture_filter(filter)
        );
    }

    void set_wrap(Texture_wrap_target target, Texture_wrap wrap) override {
        glTextureParameteri(
            m_texture_id, 
            gl_texture_warp_target(target), 
            gl_texture_wrap(wrap)
        );
    }

    void on_set_border_color() override {
        glTextureParameterfv(
            m_texture_id,
            GL_TEXTURE_BORDER_COLOR,
            glm::value_ptr(m_border_color)
        );
    }

};

class RHI_texture_2D_OpenGL : public RHI_texture_OpenGL, public IRHI_texture_2D {
public: 
    RHI_texture_2D_OpenGL(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const Image_data& image
    ) : IRHI_texture_2D(image),
        RHI_texture_OpenGL(
        width,
        height,
        mipmap_levels,
        Texture_type::TEXTURE_2D,
        internal_format,
        wraps,
        filters
    ) {

        glCreateTextures(gl_texture_type(m_type), 1, &m_texture_id);
        glTextureStorage2D(
            m_texture_id, 
            m_mipmap_levels, 
            gl_texture_internal_format(m_internal_format), 
            m_width, 
            m_height
        );

        apply_filters();
        apply_wraps();
        upload_data(image);
    }

    virtual ~RHI_texture_2D_OpenGL() {}

    void upload_data(const Image_data& image) override {

        if (image.data == nullptr) {
            std::cout << "[RHI_texture_2D_OpenGL] Image data is nullptr" << std::endl;
            return;
        }

        glTextureSubImage2D(
            m_texture_id, 
            0, 
            0, 0, 
            m_width, m_height,
            gl_texture_external_format(image.external_format), 
            gl_texture_buffer_type(image.buffer_type), 
            image.data
        );

        if (m_mipmap_levels > 1) generate_mipmap();
        m_is_sync_image_data = true;
    }  

    void sync_image_data() override {
        if (m_is_sync_image_data) return;
        m_is_sync_image_data = true;
        if (m_image.buffer_type == Texture_buffer_type::UNSIGNED_BYTE) {
            auto data = new unsigned char[m_width * m_height * 4];
            glGetTextureImage(
                m_texture_id,
                0,
                gl_texture_external_format(m_image.external_format),
                gl_texture_buffer_type(m_image.buffer_type),
                static_cast<GLsizei>(m_width * m_height * 4),
                data
            );

            m_image = Image_data{
                .width = m_width,
               .height = m_height,
               .data = data,
               .buffer_type = m_image.buffer_type,
               .external_format = m_image.external_format
            };
        } else if (m_image.buffer_type == Texture_buffer_type::FLOAT) {
            auto data = new float[m_width * m_height];
            glGetTextureImage(
                m_texture_id,
                0,
                gl_texture_external_format(m_image.external_format),
                gl_texture_buffer_type(m_image.buffer_type),
                static_cast<GLsizei>(m_width * m_height),
                data
            );
            m_image = Image_data{
                .width = m_width,
                .height = m_height,
                .data = reinterpret_cast<unsigned char*>(data),
                .buffer_type = m_image.buffer_type,
                .external_format = m_image.external_format
            };
        } else {
            throw std::runtime_error("Unsupported texture buffer type");
        }
        
    }
};


class RHI_texture_cubemap_OpenGL : public RHI_texture_OpenGL, public IRHI_texture_cubemap {
public:
    RHI_texture_cubemap_OpenGL(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const std::unordered_map<Texture_cubemap_face, Image_data>& images
    ) : IRHI_texture_cubemap(images),
        RHI_texture_OpenGL(
        width,
        height,
        mipmap_levels,
        Texture_type::TEXTURE_CUBEMAP,
        internal_format,
        wraps,
        filters
    ) {
        glCreateTextures(gl_texture_type(m_type), 1, &m_texture_id);
        glTextureStorage2D(
            m_texture_id,
            m_mipmap_levels,
            gl_texture_internal_format(m_internal_format),
            m_width,
            m_height
        );

        apply_filters();
        apply_wraps();
        upload_data(images);
    }

    virtual ~RHI_texture_cubemap_OpenGL() {}

    void upload_data(const std::unordered_map<Texture_cubemap_face, Image_data>& images) override {
        
        for (const auto& [face, image] : images) {
    
            if (image.data == nullptr) {
                std::cout << "[RHI_texture_cubemap_OpenGL] Image data is nullptr" << std::endl;
                return;
            }

            glTextureSubImage3D(
                m_texture_id,
                0,
                0, 0, static_cast<unsigned int>(face),
                m_width, m_height, 1,
                gl_texture_external_format(image.external_format),
                gl_texture_buffer_type(image.buffer_type),
                image.data
            );
        }

        if (m_mipmap_levels > 1) generate_mipmap();
        m_is_sync_image_data = true;
    }

    void sync_image_data() override {
        std::runtime_error("Cubemap texture does not support sync image data");
    }
};

class RHI_texture_2D_array_OpenGL : public RHI_texture_OpenGL, public IRHI_texture_2D_array {
public:
    RHI_texture_2D_array_OpenGL(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const std::vector<Image_data>& images
    ) : IRHI_texture_2D_array(images),
        RHI_texture_OpenGL(
        width,
        height,
        mipmap_levels,
        Texture_type::TEXTURE_2D_ARRAY,
        internal_format,
        wraps,
        filters
    ) {
        glCreateTextures(gl_texture_type(m_type), 1, &m_texture_id);
        glTextureStorage3D(
            m_texture_id,
            m_mipmap_levels,
            gl_texture_internal_format(m_internal_format),
            m_width,
            m_height,
            m_images.size()
        );

        apply_filters();
        apply_wraps();

        upload_data(images);
    }

    virtual ~RHI_texture_2D_array_OpenGL() {}

    void upload_data_from_rhi(const std::vector<std::shared_ptr<RHI_texture>>& images) override {
        m_images.resize(images.size());
        for (size_t i = 0; i < images.size(); i++) {
            auto& image = images[i];
            if (auto gl_image = std::dynamic_pointer_cast<RHI_texture_2D_OpenGL>(image)) {
                glCopyImageSubData(
                    gl_image->texture_id(),
                    gl_texture_type(gl_image->type()),
                    0,
                    0, 0, 0,
                    m_texture_id,
                    gl_texture_type(m_type),
                    0,
                    0, 0, static_cast<unsigned int>(i),
                    m_width,
                    m_height,
                    1
                );
            } else {
                throw std::runtime_error("Unsupported texture type or invaild texture");
            }
        }
        if (m_mipmap_levels > 1) generate_mipmap();
        m_is_sync_image_data = false;
    }

    void upload_data(const std::vector<Image_data>& images) override {
        m_images = images;
        for (size_t i = 0; i < images.size(); i++) {
            const auto& image = images[i];
    
            if (image.data == nullptr) {
                std::cout << "[RHI_texture_2D_array_OpenGL] Image data is nullptr" << std::endl;
                return;
            }
            
            glTextureSubImage3D(
                m_texture_id,
                0,
                0, 0, static_cast<unsigned int>(i),
                m_width, m_height, 1,
                gl_texture_external_format(image.external_format),
                gl_texture_buffer_type(image.buffer_type),
                image.data
            );
        }

        if (m_mipmap_levels > 1) generate_mipmap();
        m_is_sync_image_data = true;
    }

    void sync_image_data() override {
        if (m_is_sync_image_data) return;
        m_is_sync_image_data = true;
        for (size_t i = 0; i < m_images.size(); i++) {
            auto& image = m_images[i];
            if (image.buffer_type == Texture_buffer_type::UNSIGNED_BYTE) {
                auto data = new unsigned char[m_width * m_height * 4];
                glGetTextureImage(
                    m_texture_id,
                    0,
                    gl_texture_external_format(image.external_format),
                    gl_texture_buffer_type(image.buffer_type),
                    static_cast<GLsizei>(m_width * m_height * 4),
                    data
                );
                image = Image_data{
                   .width = m_width,
                  .height = m_height,
                  .data = data,
                  .buffer_type = image.buffer_type,
                  .external_format = image.external_format
                };
            } else if (image.buffer_type == Texture_buffer_type::FLOAT) {
                auto data = new float[m_width * m_height];
                glGetTextureImage(
                    m_texture_id,
                    0,
                    gl_texture_external_format(image.external_format),
                    gl_texture_buffer_type(image.buffer_type),
                    static_cast<GLsizei>(m_width * m_height),
                    data
                );
                image = Image_data{
                    .width = m_width,
                    .height = m_height,
                    .data = reinterpret_cast<unsigned char*>(data),
                    .buffer_type = image.buffer_type,
                    .external_format = image.external_format
                };
            } else {
                throw std::runtime_error("Unsupported texture buffer type");
            }
        }
    }
};

class RHI_texture_builder_OpenGL : public RHI_texture_builder {
public:
    void build_texure_2D_array(
        std::shared_ptr<RHI_texture>& texture_2D_array, 
        const std::vector<std::shared_ptr<RHI_texture>>& texture_2Ds
    ) override {
        if (auto gl_texture_2D_array = std::dynamic_pointer_cast<RHI_texture_2D_array_OpenGL>(texture_2D_array)) {
            gl_texture_2D_array->upload_data_from_rhi(texture_2Ds);
            return;
        }
    }

    virtual ~RHI_texture_builder_OpenGL() {}
};


};