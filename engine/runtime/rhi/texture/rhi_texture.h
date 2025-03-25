#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"

namespace rtr {



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
    virtual void set_filter(Texture_filter_target target, Texture_filter filter) = 0;
    virtual void set_wrap(Texture_wrap_target target, Texture_wrap wrap) = 0;
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
    const unsigned char* m_data{nullptr};

public:
    RHI_texture_2D(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        int width,
        int height,
        const unsigned char* data
    ) : RHI_texture(
        Texture_type::TEXTURE_2D, 
        internal_format, 
        external_format, 
        buffer_type), 
        m_width(width),
        m_height(height),
        m_data(data) {}

    RHI_texture_2D(
        int width,
        int height,
        const unsigned char* data
    ) : RHI_texture(
        Texture_type::TEXTURE_2D, 
        Texture_format::SRGB_ALPHA, 
        Texture_format::RGB_ALPHA, 
        Texture_buffer_type::UNSIGNED_BYTE),
        m_width(width),
        m_height(height),
        m_data(data) {}
  
    virtual ~RHI_texture_2D() = default;
    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void bind(unsigned int slot) = 0;
    virtual void unbind() = 0;
    virtual void set_data() = 0;
    virtual void set_filter(Texture_filter_target target, Texture_filter filter) = 0;
    virtual void set_wrap(Texture_wrap_target target, Texture_wrap wrap) = 0;
    virtual void generate_mipmap() = 0;

    int width() const { return m_width; }
    int height() const { return m_height; }
    const unsigned char* data() const { return m_data; }
    
};

class RHI_texture_cube_map : public RHI_texture {   
public:
    struct Face_data {
        int width{};
        int height{};
        const unsigned char* data{nullptr};
    };
    
protected:
    std::unordered_map<Texture_cube_map_face, Face_data> m_face_data{};
    
public:
    RHI_texture_cube_map(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        const std::unordered_map<Texture_cube_map_face, Face_data>& face_data
    ) : RHI_texture(Texture_type::TEXTURE_CUBE_MAP, internal_format, external_format, buffer_type),
        m_face_data(face_data) {}
    
    RHI_texture_cube_map(
        const std::unordered_map<Texture_cube_map_face, Face_data>& face_data
    ) : RHI_texture(Texture_type::TEXTURE_CUBE_MAP, Texture_format::SRGB_ALPHA, Texture_format::RGB_ALPHA, Texture_buffer_type::UNSIGNED_BYTE),
        m_face_data(face_data) {}

    virtual ~RHI_texture_cube_map() = default;
    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void bind(unsigned int slot) = 0;
    virtual void unbind() = 0;
    virtual void set_data() = 0;
    virtual void set_filter(Texture_filter_target target, Texture_filter filter) = 0;
    virtual void set_wrap(Texture_wrap_target target, Texture_wrap wrap) = 0;
    virtual void generate_mipmap() = 0;

    std::unordered_map<Texture_cube_map_face, Face_data>& face_data() { return m_face_data; }
    const std::unordered_map<Texture_cube_map_face, Face_data>& face_data() const { return m_face_data; }
    const Face_data& face_data(Texture_cube_map_face face) const { return m_face_data.at(face); }


};

};