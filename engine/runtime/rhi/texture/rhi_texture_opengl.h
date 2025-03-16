#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/texture/rhi_texture.h"

namespace rtr {

inline constexpr unsigned int gl_texture_type(Texture_type type) {
    switch (type) {
        case Texture_type::TEXTURE_2D:
            return GL_TEXTURE_2D;
        case Texture_type::TEXTURE_CUBE_MAP:
            return GL_TEXTURE_CUBE_MAP;
        default:
            return GL_TEXTURE_2D;
    }
}

inline constexpr unsigned int gl_texture_format(Texture_format format) {
    switch (format) {
        case Texture_format::RGB:
            return GL_RGB;
        case Texture_format::RGB_ALPHA:
            return GL_RGBA;
        case Texture_format::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        case Texture_format::SRGB_ALPHA:
            return GL_SRGB_ALPHA;
        case Texture_format::SRGB:
            return GL_SRGB;
        case Texture_format::DEPTH_STENCIL_24_8:
            return GL_DEPTH24_STENCIL8;
        default:
            return GL_RGB;
    }
}

inline constexpr unsigned int gl_texture_buffer_type(Texture_buffer_type type) {
    switch (type) {
        case Texture_buffer_type::UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case Texture_buffer_type::UNSIGNED_INT:
            return GL_UNSIGNED_INT;
        case Texture_buffer_type::UNSIGNED_INT_24_8:
            return GL_UNSIGNED_INT_24_8;
        default:
            return GL_UNSIGNED_BYTE;
    }
}

inline constexpr unsigned int gl_texture_wrap(Texture_wrap wrap) {
    switch (wrap) {
        case Texture_wrap::REPEAT:
            return GL_REPEAT;
        case Texture_wrap::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case Texture_wrap::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case Texture_wrap::CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
        default:
            return GL_REPEAT;
    }
}

inline constexpr unsigned int gl_texture_filter(Texture_filter filter) {
    switch (filter) {
        case Texture_filter::NEAREST:
            return GL_NEAREST;
        case Texture_filter::LINEAR:
            return GL_LINEAR;
        case Texture_filter::NEAREST_MIPMAP_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        case Texture_filter::LINEAR_MIPMAP_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;
        case Texture_filter::NEAREST_MIPMAP_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        case Texture_filter::LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
            return GL_NEAREST;
    }
}

inline constexpr unsigned int gl_texture_cube_map_face(Texture_cube_map_face face) {
    switch (face) {
        case Texture_cube_map_face::POSITIVE_X:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        case Texture_cube_map_face::NEGATIVE_X:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case Texture_cube_map_face::POSITIVE_Y:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        case Texture_cube_map_face::NEGATIVE_Y:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case Texture_cube_map_face::POSITIVE_Z:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        case Texture_cube_map_face::NEGATIVE_Z:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        default:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    }
}

inline constexpr unsigned int gl_texture_warp_target(Texture_warp_target target) {
    switch (target) {
        case Texture_warp_target::U:
            return GL_TEXTURE_WRAP_S;
        case Texture_warp_target::V:
            return GL_TEXTURE_WRAP_T;
        case Texture_warp_target::W:
            return GL_TEXTURE_WRAP_R;
        default:
            return GL_TEXTURE_WRAP_S;
    }
}

inline constexpr unsigned int gl_texture_filter_target(Texture_filter_target target) {
    switch (target) {
        case Texture_filter_target::MIN:
            return GL_TEXTURE_MIN_FILTER;
        case Texture_filter_target::MAG:
            return GL_TEXTURE_MAG_FILTER;
        default:
            return GL_TEXTURE_MIN_FILTER;
    }
}

class RHI_texture_2D_OpenGL : public RHI_texture_2D {

protected:
    unsigned int m_texture_id{};

public:
    RHI_texture_2D_OpenGL(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        int width,
        int height,
        unsigned char* data
    ) : RHI_texture_2D(internal_format, external_format, buffer_type, width, height, data) {
        init();
        set_data();
    }

    RHI_texture_2D_OpenGL(
        int width,
        int height,
        unsigned char* data
    ) : RHI_texture_2D(width, height, data) {
        init();
        set_data();
    }

    virtual ~RHI_texture_2D_OpenGL() {
        destroy();
    }
    
    virtual void set_data() override {

        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        if (!m_data) {
            std::cout << "invalid image data, create empty texture" << std::endl;
        } else {
            std::cout << "image data valid" << std::endl;
        }

        bind(0);

        glTexImage2D(
            gl_texture_type(m_type),
            0,
            gl_texture_format(m_internal_format),
            m_width,
            m_height,
            0,
            gl_texture_format(m_external_format),
            gl_texture_buffer_type(m_buffer_type),
            m_data
        );

        unbind();
    }

    virtual void init() override {
        glGenTextures(1, &m_texture_id);
    }

    virtual void destroy() override {
        if (m_texture_id) {
            glDeleteTextures(1, &m_texture_id);
        }
    }

    virtual void bind(unsigned int slot) override {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        if (slot >= 32) {
            std::cout << "invalid texture slot" << std::endl;
            return;
        }

        if (slot != m_slot) {
            m_slot = slot;
        }

        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(gl_texture_type(m_type), m_texture_id);
    }

    virtual void unbind() override {
        glBindTexture(gl_texture_type(m_type), 0);
    }

    virtual void set_wrap(Texture_wrap wrap, Texture_warp_target target) override {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }
        bind(0);
        glTexParameteri(
            gl_texture_type(m_type),
            gl_texture_warp_target(target),
            gl_texture_wrap(wrap)
        );
        unbind();
    }

    virtual void set_filter(Texture_filter filter, Texture_filter_target target) override {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        bind(0);
        glTexParameteri(
            gl_texture_type(m_type),
            gl_texture_filter_target(target),
            gl_texture_filter(filter)
        );
        unbind();
    }

    virtual void generate_mipmap() override {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        bind(0);
        glGenerateMipmap(gl_texture_type(m_type));
        unbind();

    }

    unsigned int texture_id() const { return m_texture_id; }

};

class RHI_texture_cube_map_OpenGL : public RHI_texture_cube_map {
protected:
    unsigned int m_texture_id{};   

public:
    RHI_texture_cube_map_OpenGL(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        std::unordered_map<Texture_cube_map_face, Face_data> face_data
    ) : RHI_texture_cube_map(internal_format, external_format, buffer_type, face_data) {
        init();
        set_data();
    }

    RHI_texture_cube_map_OpenGL(
        std::unordered_map<Texture_cube_map_face, Face_data> face_data
    ) : RHI_texture_cube_map(face_data) {
        init();
        set_data();
    }

    virtual ~RHI_texture_cube_map_OpenGL() {
        destroy();
    }

    virtual void init() override {
        glGenTextures(1, &m_texture_id);
    }

    virtual void destroy() override {
        if (m_texture_id) {
            glDeleteTextures(1, &m_texture_id);
        }
    }

    virtual void bind(unsigned int slot) override {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        if (slot >= 32) {
            std::cout << "invalid texture slot" << std::endl;
            return;
        }

        if (slot != m_slot) {
            m_slot = slot;
        }

        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(gl_texture_type(m_type), m_texture_id);
    }

    virtual void unbind() override {
        glBindTexture(gl_texture_type(m_type), 0);
    }

    virtual void set_wrap(Texture_wrap wrap, Texture_warp_target target) override {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }
        bind(0);
        glTexParameteri(
            gl_texture_type(m_type),
            gl_texture_warp_target(target),
            gl_texture_wrap(wrap)
        );
        unbind();
    }

    virtual void set_filter(Texture_filter filter, Texture_filter_target target) override {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        bind(0);
        glTexParameteri(
            gl_texture_type(m_type),
            gl_texture_filter_target(target),
            gl_texture_filter(filter)
        );
        unbind();
    }

    virtual void generate_mipmap() override {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        bind(0);
        glGenerateMipmap(gl_texture_type(m_type));
        unbind();

    }

    virtual void set_data() override {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        bind(0);
        for (const auto& [face, data] : m_face_data) {
            glTexImage2D(
                gl_texture_cube_map_face(face),
                0,
                gl_texture_format(m_internal_format),
                data.width,
                data.height,
                0,
                gl_texture_format(m_external_format),
                gl_texture_buffer_type(m_buffer_type),
                data.data
            );
        }
        unbind();
    }

    unsigned int texture_id() const { return m_texture_id; }

};

};