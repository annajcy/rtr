#pragma once

#include "engine/runtime/resource/loader/image.h"
#include "glm/ext/vector_float4.hpp"
#include <memory>
#include <vector>

namespace rtr {


enum class Texture_buffer_type {
    UNSIGNED_BYTE,
    UNSIGNED_INT,
    UNSIGNED_INT_24_8,
    FLOAT
};

enum class Texture_type {
    TEXTURE_2D,
    TEXTURE_CUBEMAP,
    TEXTURE_2D_ARRAY
};

enum class Texture_internal_format {
    R_8F, R_16F, R_32F,
    RG_8F, RG_16F, RG_32F,
    RGB_8F, RGB_16F, RGB_32F,
    RGB_ALPHA_8F, RGB_ALPHA_16F, RGB_ALPHA_32F,
    DEPTH_STENCIL_24F_8F,
    DEPTH_24F, DEPTH_32F,
    SRGB_ALPHA_8F,
    SRGB_8F
};

enum class Texture_external_format {
    RGB,
    RGB_ALPHA,
    DEPTH_STENCIL,
    DEPTH,
    SRGB_ALPHA,
    SRGB
};

enum class Texture_wrap {
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};

enum class Texture_wrap_target {
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

enum class Texture_cubemap_face {
    RIGHT,
    LEFT,
    TOP,
    BOTTOM,
    BACK,
    FRONT,
};


struct Image_data {
    int width{};
    int height{};
    const unsigned char* data{ nullptr };
    Texture_buffer_type buffer_type{ Texture_buffer_type::UNSIGNED_BYTE };
    Texture_external_format external_format{ Texture_external_format::RGB_ALPHA };
    bool has_ownership{ false };

    ~Image_data() {
        if (has_ownership) {
            delete[] data;
        }
    }

    std::shared_ptr<Image> create_image() const {
        if (buffer_type == Texture_buffer_type::FLOAT) {
            return Image::create(Image_format::GRAY, data, width * height * sizeof(float));
        }
        else if (buffer_type == Texture_buffer_type::UNSIGNED_BYTE) {
            return Image::create(Image_format::RGB_ALPHA, data, width * height * sizeof(unsigned char));
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
public:
    IRHI_texture_2D() {}
    virtual ~IRHI_texture_2D() {}
    virtual bool upload_data(const Image_data& image) = 0;
    virtual bool upload_data(const std::shared_ptr<RHI_texture>& image) = 0;
    virtual Image_data get_image_data() = 0;
};

class IRHI_texture_cubemap {

public:
    IRHI_texture_cubemap() {}
    virtual ~IRHI_texture_cubemap() {}
    virtual bool upload_data(const std::unordered_map<Texture_cubemap_face, Image_data>& images) = 0;
    virtual bool upload_data(const std::unordered_map<Texture_cubemap_face, std::shared_ptr<RHI_texture>>& images) = 0;
    virtual std::unordered_map<Texture_cubemap_face, Image_data> get_image_data() = 0;
};

class IRHI_texture_2D_array {
protected:
    unsigned int m_layer_count{};

public:
    IRHI_texture_2D_array(
        unsigned int layer_count
    ) : m_layer_count(layer_count) {}

    virtual ~IRHI_texture_2D_array() {}
    virtual bool upload_data(const std::vector<Image_data>& images) = 0;
    virtual bool upload_data(const std::vector<std::shared_ptr<RHI_texture>>& images) = 0;
    virtual std::vector<Image_data> get_image_data() = 0;
};

class RHI_texture_builder {
public:
    RHI_texture_builder() {}
    virtual ~RHI_texture_builder() {}
    
    virtual bool build_texture_2D(std::shared_ptr<RHI_texture>& texture_2D, const Image_data& image) = 0;
    virtual bool build_texture_2D(std::shared_ptr<RHI_texture>& texture_2D, const std::shared_ptr<RHI_texture>& image) = 0;
    virtual bool build_texture_cubemap(std::shared_ptr<RHI_texture>& texture_cubemap, const std::unordered_map<Texture_cubemap_face, Image_data>& images) = 0;
    virtual bool build_texture_cubemap(std::shared_ptr<RHI_texture>& texture_cubemap, const std::unordered_map<Texture_cubemap_face, std::shared_ptr<RHI_texture>>& images) = 0;
    virtual bool build_texture_2D_array(std::shared_ptr<RHI_texture>& texture_2D_array, const std::vector<Image_data>& texture_2Ds) = 0;
    virtual bool build_texture_2D_array(std::shared_ptr<RHI_texture>& texture_2D_array, const std::vector<std::shared_ptr<RHI_texture>>& texture_2Ds) = 0;
};

};