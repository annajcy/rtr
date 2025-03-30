#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_resource.h"

namespace rtr {

struct Image_data {
    int width{};
    int height{};
    const unsigned char* data{ nullptr };
    Texture_buffer_type buffer_type{ Texture_buffer_type::UNSIGNED_BYTE };
    Texture_external_format external_format{ Texture_external_format::RGB_ALPHA };
};

class RHI_texture : public RHI_resource  {
protected:
    int m_width{};
    int m_height{};
    Texture_type m_type{};
    Texture_internal_format m_internal_format{};
    unsigned int m_mipmap_levels{};
    
public:
    RHI_texture (
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_type type, 
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters
    ) : RHI_resource(RHI_resource_type::TEXTURE),
        m_type(type), 
        m_internal_format(internal_format),
        m_width(width),
        m_height(height),
        m_mipmap_levels(mipmap_levels) {}

    using Ptr = std::shared_ptr<RHI_texture>;

    virtual ~RHI_texture() {}
    
    virtual void set_filter(Texture_filter_target target, Texture_filter filter) = 0;
    virtual void set_wrap(Texture_wrap_target target, Texture_wrap wrap) = 0;
    virtual void generate_mipmap() = 0;
    virtual void bind(unsigned int location) = 0;
   
    Texture_type type() const { return m_type; }
    Texture_internal_format internal_format() const { return m_internal_format; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    unsigned int mipmap_levels() const { return m_mipmap_levels; }
};

class IRHI_texture_2D {
public:
    IRHI_texture_2D() {}
    virtual ~IRHI_texture_2D() {}
    virtual void upload_data(const Image_data& image) = 0;
};

class IRHI_texture_cubemap {
public:
    IRHI_texture_cubemap() {}
    virtual ~IRHI_texture_cubemap() {}
    virtual void upload_data(const std::unordered_map<Texture_cubemap_face, Image_data>& images) = 0;
};

};