#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/texture/rhi_texture.h"

#define MAX_TEXTURE_SLOTS 32

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

// 新增抽象基类
class RHI_texture_OpenGL {
protected:
    unsigned int m_texture_id{};
    unsigned int m_slot{};
    unsigned int m_gl_type{};

public:

    RHI_texture_OpenGL(unsigned int gl_type) : m_gl_type(gl_type) {}
    virtual ~RHI_texture_OpenGL() = default;

    void gl_init() {
        glGenTextures(1, &m_texture_id);
    }

    void gl_destroy() {
        if (m_texture_id) {
            glDeleteTextures(1, &m_texture_id);
        }
    }

    void gl_bind(unsigned int slot) {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        if (slot >= MAX_TEXTURE_SLOTS) {
            std::cout << "invalid texture slot" << std::endl;
            return;
        }

        if (slot != m_slot) {
            m_slot = slot;
        }

        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(m_gl_type, m_texture_id);
    }

    void gl_unbind() {
        glBindTexture(m_gl_type, 0);
    }

    void gl_set_wrap(Texture_wrap wrap, Texture_warp_target target) {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        gl_bind(0);
        glTexParameteri(
            m_gl_type,
            gl_texture_warp_target(target),
            gl_texture_wrap(wrap)
        );

        gl_unbind();
    }

    void gl_set_filter(Texture_filter filter, Texture_filter_target target) {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        gl_bind(0);
        glTexParameteri(
            m_gl_type,
            gl_texture_filter_target(target),
            gl_texture_filter(filter)
        );
        gl_unbind();
    }

    void gl_generate_mipmap() {
        if (!m_texture_id) {
            std::cout << "invalid texture id" << std::endl;
            return;
        }

        gl_bind(0);
        glGenerateMipmap(m_gl_type);
        gl_unbind();

    }

    unsigned int texture_id() const { return m_texture_id; }
    unsigned int slot() const { return m_slot; }
    unsigned int gl_type() const { return m_gl_type; }

};

// 修改现有实现
class RHI_texture_2D_OpenGL : public RHI_texture_2D, public RHI_texture_OpenGL {
    
public:
    RHI_texture_2D_OpenGL(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        int width,
        int height,
        unsigned char* data
    ) : RHI_texture_2D(internal_format, external_format, buffer_type, width, height, data), 
        RHI_texture_OpenGL(GL_TEXTURE_2D) {
        init();
        set_data();
    }

    RHI_texture_2D_OpenGL(
        int width,
        int height,
        unsigned char* data
    ) : RHI_texture_2D(width, height, data),
        RHI_texture_OpenGL(GL_TEXTURE_2D) {
        init();
        set_data();
    }

    virtual ~RHI_texture_2D_OpenGL() {
        destroy();
    }

    virtual void init() override {
        gl_init();
    }

    virtual void destroy() override {
        gl_destroy();
    }

    virtual void bind(unsigned int slot) override {
        gl_bind(slot);
    }

    virtual void unbind() override {
        gl_unbind();
    }

    virtual void set_wrap(Texture_wrap wrap, Texture_warp_target target) override {
        gl_set_wrap(wrap, target);
    }

    virtual void set_filter(Texture_filter filter, Texture_filter_target target) override {
        gl_set_filter(filter, target);
    }

    virtual void generate_mipmap() override {
        gl_generate_mipmap();
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

    

};

// 立方体贴图同理
class RHI_texture_cube_map_OpenGL : public RHI_texture_cube_map, public RHI_texture_OpenGL {

public:
    RHI_texture_cube_map_OpenGL(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        std::unordered_map<Texture_cube_map_face, Face_data> face_data
    ) : RHI_texture_cube_map(internal_format, external_format, buffer_type, face_data),
        RHI_texture_OpenGL(GL_TEXTURE_CUBE_MAP) {
        init();
        set_data();
    }

    RHI_texture_cube_map_OpenGL(
        std::unordered_map<Texture_cube_map_face, Face_data> face_data
    ) : RHI_texture_cube_map(face_data),
        RHI_texture_OpenGL(GL_TEXTURE_CUBE_MAP) {
        init();
        set_data();
    }

    virtual ~RHI_texture_cube_map_OpenGL() {
        destroy();
    }

    virtual void init() override {
        gl_init();
    }

    virtual void destroy() override {
        gl_destroy();
    }

    virtual void bind(unsigned int slot) override {
        gl_bind(slot);
    }

    virtual void unbind() override {
        gl_unbind();
    }

    virtual void set_wrap(Texture_wrap wrap, Texture_warp_target target) override {
        gl_set_wrap(wrap, target);
    }

    virtual void set_filter(Texture_filter filter, Texture_filter_target target) override {
        gl_set_filter(filter, target);
    }

    virtual void generate_mipmap() override {
        gl_generate_mipmap();
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

};

};