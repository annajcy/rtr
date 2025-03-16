#pragma once
#include "engine/global/base.h"

namespace rtr {

enum class Texture_buffer_type {
    UNSIGNED_BYTE,
    UNSIGNED_INT,
    UNSIGNED_INT_24_8,
};

enum class Texture_type {
    TEXTURE_2D,
    TEXTURE_CUBE_MAP
};

enum class Texture_format {
    RGB,
    RGB_ALPHA,
    DEPTH_STENCIL,
    DEPTH_STENCIL_24_8,
    SRGB_ALPHA,
    SRGB
};

enum class Texture_wrap {
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};

enum class Texture_warp_target {
    U, V, W
};

enum class Texture_filter {
    NEAREST,
    LINEAR,
    NEAREST_MIPMAP_NEAREST,
    LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR,
};

enum class Texture_filter_target {
    MIN,
    MAG,
};

enum class Texture_cube_map_face {
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    NEGATIVE_Z,
    POSITIVE_Z,
};

class RHI_texture {
protected:
    Texture_type m_type{};
    Texture_format m_internal_format{};
    Texture_format m_external_format{};
    Texture_buffer_type m_buffer_type{};
    
public:
    RHI_texture(
        Texture_type type, 
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type
    ) : m_type(type), 
        m_internal_format(internal_format),
        m_external_format(external_format),
        m_buffer_type(buffer_type) {}

    virtual ~RHI_texture() = default;
    virtual void init() = 0;
    virtual void destroy() = 0;

    virtual void bind(unsigned int slot) = 0;
    virtual void unbind() = 0;
    virtual void set_data() = 0;
    virtual void set_wrap(Texture_wrap wrap, Texture_warp_target target) = 0;
    virtual void set_filter(Texture_filter filter, Texture_filter_target target) = 0;
    virtual void generate_mipmap() = 0;
   
    Texture_type type() const { return m_type; }
    Texture_format internal_format() const { return m_internal_format; }
    Texture_format external_format() const { return m_external_format; }
    Texture_buffer_type buffer_type() const { return m_buffer_type; }

};

class RHI_texture_2D : public RHI_texture {
protected:
    int m_width{};
    int m_height{};
    unsigned char* m_data{nullptr};

public:
    RHI_texture_2D(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        int width,
        int height,
        unsigned char* data
    ) : RHI_texture(Texture_type::TEXTURE_2D, internal_format, external_format, buffer_type), 
        m_width(width),
        m_height(height),
        m_data(data) {}

    RHI_texture_2D(
        int width,
        int height,
        unsigned char* data
    ) : RHI_texture(Texture_type::TEXTURE_2D, Texture_format::SRGB_ALPHA, Texture_format::RGB_ALPHA, Texture_buffer_type::UNSIGNED_BYTE),
        m_width(width),
        m_height(height),
        m_data(data) {}

    virtual ~RHI_texture_2D() = default;
    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void bind(unsigned int slot) = 0;
    virtual void unbind() = 0;
    virtual void set_data() = 0;

};

class RHI_texture_cube_map : public RHI_texture {   
public:
    struct Face_data {
        int width{};
        int height{};
        unsigned char* data{nullptr};
    };
    
protected:
    std::unordered_map<Texture_cube_map_face, Face_data> m_face_data{};
    
public:
    RHI_texture_cube_map(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        std::unordered_map<Texture_cube_map_face, Face_data> face_data
    ) : RHI_texture(Texture_type::TEXTURE_CUBE_MAP, internal_format, external_format, buffer_type),
        m_face_data(face_data) {}
    
    RHI_texture_cube_map(
        std::unordered_map<Texture_cube_map_face, Face_data> face_data
    ) : RHI_texture(Texture_type::TEXTURE_CUBE_MAP, Texture_format::SRGB_ALPHA, Texture_format::RGB_ALPHA, Texture_buffer_type::UNSIGNED_BYTE),
        m_face_data(face_data) {}

    virtual ~RHI_texture_cube_map() = default;
    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void bind(unsigned int slot) = 0;
    virtual void unbind() = 0;
    virtual void set_data() = 0;

};

};