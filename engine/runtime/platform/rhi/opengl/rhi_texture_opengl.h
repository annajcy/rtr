#pragma once

#include "engine/runtime/global/base.h" 

#include "../rhi_texture.h"
#include "glm/ext/vector_float4.hpp"
#include "rhi_cast_opengl.h"
#include "rhi_error_opengl.h"

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
    ) {
        glCreateTextures(gl_texture_type(m_type), 1, &m_texture_id);
        glTextureStorage2D(
            m_texture_id, 
            m_mipmap_levels, 
            gl_texture_internal_format(m_internal_format), 
            m_width, 
            m_height
        );

        for (const auto& [target, wrap] : wraps) {
            set_wrap(target, wrap);
        }

        for (const auto& [target, filter] : filters) {
            set_filter(target, filter);
        }
    }
        
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
    ) : IRHI_texture_2D(),
        RHI_texture_OpenGL(
        width,
        height,
        mipmap_levels,
        Texture_type::TEXTURE_2D,
        internal_format,
        wraps,
        filters
    ) {
        upload_data(image);
    }

    virtual ~RHI_texture_2D_OpenGL() {}

    void upload_data(const Image_data& image) override {

        if (image.external_format == Texture_external_format::SRGB_ALPHA || 
            image.external_format == Texture_external_format::SRGB
        ) {
            glEnable(GL_FRAMEBUFFER_SRGB);
        } else {
            glDisable(GL_FRAMEBUFFER_SRGB);
        }

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
    ) : IRHI_texture_cubemap(),
        RHI_texture_OpenGL(
        width,
        height,
        mipmap_levels,
        Texture_type::TEXTURE_CUBEMAP,
        internal_format,
        wraps,
        filters
    ) {
        upload_data(images);
    }

    virtual ~RHI_texture_cubemap_OpenGL() {}

    void upload_data(const std::unordered_map<Texture_cubemap_face, Image_data>& images) override {
        
        for (const auto& [face, image] : images) {
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
    }
};


};