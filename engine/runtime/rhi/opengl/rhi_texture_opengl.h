#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/opengl/rhi_cast_opengl.h"
#include "engine/runtime/rhi/rhi_texture.h"

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
        Texture_format internal_format,
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
        glTextureStorage2D(m_texture_id, mipmap_levels, gl_texture_format(m_internal_format), m_width, m_height);

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

    void bind(unsigned int location) override {
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

};

class RHI_texture_2D_OpenGL : public RHI_texture_OpenGL, public IRHI_texture_2D {
protected:
    unsigned int m_texture_id{};
public: 
    RHI_texture_2D_OpenGL(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_format internal_format,
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

    virtual ~RHI_texture_2D_OpenGL() {
        RHI_texture_OpenGL::~RHI_texture_OpenGL();
    }

    void upload_data(const Image_data& image) override {
        glTextureSubImage2D(
            m_texture_id, 
            0, 
            0, 0, 
            m_width, m_height,
            gl_texture_format(image.external_format), 
            gl_texture_buffer_type(image.buffer_type), 
            image.data
        );
    }  
};


class RHI_texture_cubemap_OpenGL : public RHI_texture_OpenGL, public IRHI_texture_cubemap {
protected:
    unsigned int m_texture_id{};
public:
    RHI_texture_cubemap_OpenGL(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const std::unordered_map<Texture_cubemap_face, Image_data>& images
    ) : IRHI_texture_cubemap(),
        RHI_texture_OpenGL(
        width,
        height,
        mipmap_levels,
        Texture_type::TEXTURE_CUBE_MAP,
        internal_format,
        wraps,
        filters
    ) {
        upload_data(images);
    }

    virtual ~RHI_texture_cubemap_OpenGL() {
        RHI_texture_OpenGL::~RHI_texture_OpenGL();
    }

    void upload_data(const std::unordered_map<Texture_cubemap_face, Image_data>& images) override {
        for (const auto& [face, image] : images) {
            glTextureSubImage3D(
                m_texture_id,
                0,
                0, 0, static_cast<unsigned int>(face),
                m_width, m_height, 1,
                gl_texture_format(image.external_format),
                gl_texture_buffer_type(image.buffer_type),
                image.data
            );
        }
    }
};


};