#pragma once

#include "engine/runtime/global/base.h" 
#include "engine/runtime/global/enum.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include <memory>
#include <vector>

namespace rtr {

struct Image_data {
    int width{};
    int height{};
    const unsigned char* data{ nullptr };
    Texture_buffer_type buffer_type{ Texture_buffer_type::UNSIGNED_BYTE };
    Texture_external_format external_format{ Texture_external_format::RGB_ALPHA };

    std::shared_ptr<Image> create_image() const {
        if (buffer_type == Texture_buffer_type::FLOAT) {
            return Image_loader::load_from_data(Image_format::GRAY, data, width * height * sizeof(float));
        }
        else if (buffer_type == Texture_buffer_type::UNSIGNED_BYTE) {
            return Image_loader::load_from_data(Image_format::RGB_ALPHA, data, width * height * sizeof(unsigned char));
        }
        else throw std::runtime_error("Unsupported buffer type");
    }
        
};

class RHI_texture {
protected:
    int m_width{};
    int m_height{};
    Texture_type m_type{};
    Texture_internal_format m_internal_format{};
    std::unordered_map<Texture_wrap_target, Texture_wrap> m_wraps{};
    std::unordered_map<Texture_filter_target, Texture_filter> m_filters{};
    unsigned int m_mipmap_levels{};
    glm::vec4 m_border_color{};
    bool m_is_sync_image_data{ false };
    
public:
    RHI_texture (
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_type type, 
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters
    ) : m_type(type), 
        m_internal_format(internal_format),
        m_width(width),
        m_height(height),
        m_mipmap_levels(mipmap_levels),
        m_wraps(wraps),
        m_filters(filters) {}

    virtual ~RHI_texture() {}
    
    virtual void set_filter(Texture_filter_target target, Texture_filter filter) = 0;
    virtual void set_wrap(Texture_wrap_target target, Texture_wrap wrap) = 0;
    
    virtual void generate_mipmap() = 0;
    virtual void bind_to_unit(unsigned int location) = 0;

    virtual void on_set_border_color() = 0;
    virtual void sync_image_data() = 0; 

    bool is_sync_image_data() const { return m_is_sync_image_data; }

    void set_border_color(const glm::vec4& color) {
        m_border_color = color;
        on_set_border_color();
    }

    void apply_filters() {
        for (const auto& [target, filter] : m_filters) {
            set_filter(target, filter);
        }
    }
    
    void apply_wraps() {
        for (const auto& [target, wrap] : m_wraps) {
            set_wrap(target, wrap);
        }
    }
   
    Texture_type type() const { return m_type; }
    Texture_internal_format internal_format() const { return m_internal_format; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    unsigned int mipmap_levels() const { return m_mipmap_levels; }
    const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps() const { return m_wraps; }
    const std::unordered_map<Texture_filter_target, Texture_filter>& filters() const { return m_filters; }
    const glm::vec4& border_color() const { return m_border_color; }
};

class IRHI_texture_2D {
protected:
    Image_data m_image{};

public:
    IRHI_texture_2D(
        const Image_data& image
    ) : m_image(image) {}
    virtual ~IRHI_texture_2D() {}
    virtual void upload_data(const Image_data& image) = 0;
    const Image_data& image() const { return m_image; }
};

class IRHI_texture_cubemap {
protected:
    std::unordered_map<Texture_cubemap_face, Image_data> m_images{};

public:
    IRHI_texture_cubemap(
        const std::unordered_map<Texture_cubemap_face, Image_data>& images
    ) : m_images(images) {}
    virtual ~IRHI_texture_cubemap() {}
    virtual void upload_data(const std::unordered_map<Texture_cubemap_face, Image_data>& images) = 0;
    const std::unordered_map<Texture_cubemap_face, Image_data>& images() const { return m_images; }
};

class IRHI_texture_2D_array {
protected:
    std::vector<Image_data> m_images{};

public:
    IRHI_texture_2D_array(
        const std::vector<Image_data>& images
    ) : m_images(images) {}

    virtual ~IRHI_texture_2D_array() {}
    virtual void upload_data(const std::vector<Image_data>& images) = 0;
    virtual void upload_data_from_rhi(const std::vector<std::shared_ptr<RHI_texture>>& images) = 0;

    const std::vector<Image_data>& images() const { return m_images; }
};

class RHI_texture_builder {
public:
    virtual void build_texure_2D_array(std::shared_ptr<RHI_texture>& texture_2D_array, const std::vector<std::shared_ptr<RHI_texture>>& texture_2Ds) = 0;
    virtual ~RHI_texture_builder() {}
};

};