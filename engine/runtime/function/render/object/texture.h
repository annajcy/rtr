#pragma once

#include "engine/runtime/function/render/core/render_resource.h"
#include "engine/runtime/platform/rhi/rhi_linker.h"
#include "engine/runtime/platform/rhi/rhi_texture.h"
#include "engine/runtime/resource/guid.h"
#include "engine/runtime/resource/hash.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include "engine/runtime/resource/resource_manager.h"
#include <memory>
#include <unordered_map>

namespace rtr {

class Texture : public RHI_linker<RHI_texture>, public Render_resource {
protected:
    Texture_type m_texture_type{};
    Texture_internal_format m_internal_format{};
    std::unordered_map<Texture_wrap_target, Texture_wrap> m_wraps{};
    std::unordered_map<Texture_filter_target, Texture_filter> m_filters{};
    unsigned int m_mipmap_levels{};

public:
    Texture(
        Texture_type texture_type,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) : Render_resource(Render_resource_type::TEXTURE),
        m_texture_type(texture_type),
        m_internal_format(internal_format),
        m_mipmap_levels(mipmap_levels),
        m_wraps(wraps),
        m_filters(filters) {}

    virtual ~Texture() {}
    Texture_type texture_type() const { return m_texture_type; }
    Texture_internal_format internal_format() const { return m_internal_format; }
    const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps() const { return m_wraps; }
    const std::unordered_map<Texture_filter_target, Texture_filter>& filters() const { return m_filters; }
    std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps() { return m_wraps; }
    std::unordered_map<Texture_filter_target, Texture_filter>& filters() { return m_filters; }
};

class Texture_2D : public Texture {
protected:
    int m_width{};
    int m_height{};
    std::shared_ptr<Image> m_image{};

public:
    Texture_2D(
        const std::shared_ptr<Image>& image,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) : Texture(
        Texture_type::TEXTURE_2D,
        mipmap_levels,
        internal_format,
        wraps,
        filters
    ),  m_image(image),
        m_width(image->width()),
        m_height(image->height()) {}

    Texture_2D(
        int width, 
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) : Texture(
        Texture_type::TEXTURE_2D,
        mipmap_levels,
        internal_format,
        wraps,
        filters
    ),  m_width(width),
        m_height(height) {}
        
    virtual ~Texture_2D() {}

    const std::shared_ptr<Image>& image() const { return m_image; }
    std::shared_ptr<Image>& image() { return m_image; }

    int width() const { return m_width; }
    int height() const { return m_height; }

    void link(const std::shared_ptr<RHI_device>& device) override {

        m_rhi_resource = device->create_texture_2D(
            m_width,
            m_height,
            m_mipmap_levels,
            m_internal_format,
            m_wraps,
            m_filters
        );

        if (!m_image) {
            //std::cout << "Texture2D::link: m_image is nullptr" << std::endl;
            return;
        }
        
        auto texture_builder = device->create_texture_builder();
        texture_builder->build_texture_2D(
            m_rhi_resource, 
            Image_data{
                m_image->width(),
                m_image->height(),
                m_image->data_ptr(),
                Texture_buffer_type::UNSIGNED_BYTE,
                Texture_external_format::RGB_ALPHA
            }
        );
    }

    static std::shared_ptr<Texture_2D> create(
        const std::shared_ptr<Image>& image,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) {
        if (!image) {
            std::cout << "Texture2D::create failed: image is nullptr" << std::endl;
            return nullptr;
        }

        return std::make_shared<Texture_2D>(
            image,
            mipmap_levels,
            internal_format,
            wraps,
            filters
        );
    }

    static std::shared_ptr<Texture_2D> create(
        int width, 
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) {
        return std::make_shared<Texture_2D>(
            width, 
            height,
            mipmap_levels,
            internal_format,
            wraps,
            filters
        );
    }

    static std::shared_ptr<Texture_2D> create_image(
        const std::shared_ptr<Image>& image,
        Texture_internal_format internal_format = Texture_internal_format::SRGB_ALPHA
    ) {
        return create(
            image,
            4,
            internal_format,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::LINEAR_MIPMAP_LINEAR},
                {Texture_filter_target::MAG, Texture_filter::LINEAR}
            }
        );
    }

    static std::shared_ptr<Texture_2D> create_color_attachemnt(
        int width,
        int height
    ) {
        return create(
            width,
            height,
            1,
            Texture_internal_format::RGB_ALPHA,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::LINEAR},
                {Texture_filter_target::MAG, Texture_filter::LINEAR}
            }
        );
    }

    static std::shared_ptr<Texture_2D> create_depth_attachemnt(
        int width,
        int height
    ) {
        return create(
            width,
            height,
            1,
            Texture_internal_format::DEPTH_32F,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_BORDER},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_BORDER}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::NEAREST},
                {Texture_filter_target::MAG, Texture_filter::NEAREST}
            }
        );
    }

    static std::shared_ptr<Texture_2D> create_depth_stencil_attachemnt(
        int width,
        int height
    ) {
        return create(
            width,
            height,
            1,
            Texture_internal_format::DEPTH_STENCIL_24_8,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                // 修改过滤模式为NEAREST
                {Texture_filter_target::MIN, Texture_filter::NEAREST},
                {Texture_filter_target::MAG, Texture_filter::NEAREST}
            }
        );
    }
};

class Texture_2D_array : public Texture {
protected:
    std::vector<std::shared_ptr<Image>> m_images{};
    int m_width{};
    int m_height{};
    int m_layer_count{};

public:
    Texture_2D_array(
        const std::vector<std::shared_ptr<Image>>& images,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) : Texture(
        Texture_type::TEXTURE_2D_ARRAY,
        mipmap_levels,
        internal_format,
        wraps,
        filters
    ),  m_images(images),
        m_width(images[0]->width()),
        m_height(images[0]->height()),
        m_layer_count(images.size()) {}

    Texture_2D_array(
        int width,
        int height,
        int layer_count,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) : Texture(
        Texture_type::TEXTURE_2D_ARRAY,
        mipmap_levels,
        internal_format,
        wraps,
        filters
    ),  m_width(width),
        m_height(height),
        m_layer_count(layer_count) {}

    virtual ~Texture_2D_array() {}
    const std::vector<std::shared_ptr<Image>>& images() const { return m_images; }
    std::vector<std::shared_ptr<Image>>& images() { return m_images; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    int layer_count() const { return m_layer_count; }

    void link(const std::shared_ptr<RHI_device>& device) override {
        
        m_rhi_resource = device->create_texture_2D_array(
            m_width,
            m_height,
            m_mipmap_levels,
            m_internal_format,
            m_wraps,
            m_filters,
            m_layer_count
        );

        if (m_images.size() == 0) {
            //std::cout << "Texture2DArray::link: m_images is empty" << std::endl;
            return;
        }

        auto texture_builder = device->create_texture_builder();
        std::vector<Image_data> images_data{};
        for (const auto& image : m_images) {
            images_data.push_back(Image_data{
                image->width(),
                image->height(),
                image->data_ptr(),
                Texture_buffer_type::UNSIGNED_BYTE,
                Texture_external_format::RGB_ALPHA
            });
        }

        texture_builder->build_texture_2D_array(
            m_rhi_resource, 
            images_data
        );
    }

    static std::shared_ptr<Texture_2D_array> create(
        const std::vector<std::shared_ptr<Image>>& images,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) {
        if (images.size() == 0) {
            std::cout << "Texture2DArray::create failed: images is empty" << std::endl;
            return nullptr;
        }
        return std::make_shared<Texture_2D_array>(
            images,
            mipmap_levels,
            internal_format,
            wraps,
            filters
        );
    }

    static std::shared_ptr<Texture_2D_array> create(
        int width,
        int height,
        int layer_count,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) {
        return std::make_shared<Texture_2D_array>(
            width,
            height,
            layer_count,
            mipmap_levels,
            internal_format,
            wraps,
            filters
        );
    }

    static std::shared_ptr<Texture_2D_array> create_image(
        const std::vector<std::shared_ptr<Image>>& images
    ) {
        return create(
            images,
            4,
            Texture_internal_format::SRGB_ALPHA,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::LINEAR_MIPMAP_LINEAR},
                {Texture_filter_target::MAG, Texture_filter::LINEAR}
            }
        );
    }
    
    static std::shared_ptr<Texture_2D_array> create_depth_attachemnt(
        int width,
        int height,
        int layer_count
    ) {
        return create(
            width,
            height,
            layer_count,
            1,
            Texture_internal_format::DEPTH_32F,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_BORDER},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_BORDER}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::NEAREST},
                {Texture_filter_target::MAG, Texture_filter::NEAREST}
            }
        );
    }
  
    static std::shared_ptr<Texture_2D_array> create_color_attachemnt(
        int width,
        int height,
        int layer_count
    ) {
        return create(
            width,
            height,
            layer_count,
            1,
            Texture_internal_format::RGB_ALPHA,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::LINEAR},
                {Texture_filter_target::MAG, Texture_filter::LINEAR}
            }
        );
    }

    static std::shared_ptr<Texture_2D_array> create_depth_stencil_attachemnt(
        int width,
        int height,
        int layer_count
    ) {
        return create(
            width,
            height,
            layer_count,
            1,
            Texture_internal_format::DEPTH_STENCIL_24_8,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::NEAREST},
                {Texture_filter_target::MAG, Texture_filter::NEAREST}
            }
        );
    }
};

class Texture_cubemap : public Texture {
protected:
    std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>> m_images{};
    int m_width{};
    int m_height{};

public:
    Texture_cubemap(
        std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>> images,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) : Texture(
        Texture_type::TEXTURE_CUBEMAP,
        mipmap_levels,
        internal_format,
        wraps,
        filters
    ), m_images(images),
        m_width(images.at(Texture_cubemap_face::FRONT)->width()),
        m_height(images.at(Texture_cubemap_face::FRONT)->height()) {}

    Texture_cubemap(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) : Texture(
        Texture_type::TEXTURE_CUBEMAP,
        mipmap_levels,
        internal_format,
        wraps,
        filters
    ) , m_width(width),
        m_height(height) {}

    virtual ~Texture_cubemap() {}
    const std::shared_ptr<Image>& image(Texture_cubemap_face face) const { return m_images.at(face); }
    std::shared_ptr<Image>& image(Texture_cubemap_face face) { return m_images.at(face); }
    const std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>>& images() const { return m_images; }
    std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>>& images() { return m_images; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    void link(const std::shared_ptr<RHI_device>& device) override {

        m_rhi_resource = device->create_texture_cubemap(
            m_width,
            m_height,
            m_mipmap_levels,
            m_internal_format,
            m_wraps,
            m_filters
        );

        if (m_images.size() != 6) {
            //std::cout << "Texture_cubemap::link: number of m_images is not vaild" << std::endl;
            return;
        }

        std::unordered_map<Texture_cubemap_face, Image_data> images{};
        for (const auto& [face, image] : m_images) {
            images[face] = Image_data{
                image->width(),
                image->height(),
                image->data_ptr(),
                Texture_buffer_type::UNSIGNED_BYTE,
                Texture_external_format::RGB_ALPHA
            };
        }

        auto texture_builder = device->create_texture_builder();
        texture_builder->build_texture_cubemap(m_rhi_resource, images);
    }

    static std::shared_ptr<Texture_cubemap> create(
        std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>> images,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) {
        if (images.size() != 6) {
            std::cout << "Texture_cubemap::create failed: number of m_images is not vaild" << std::endl;
            return nullptr;
        }
        return std::make_shared<Texture_cubemap>(
            images,
            mipmap_levels,
            internal_format,
            wraps,
            filters
        );
    }

    static std::shared_ptr<Texture_cubemap> create(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        std::unordered_map<Texture_wrap_target, Texture_wrap> wraps,
        std::unordered_map<Texture_filter_target, Texture_filter> filters
    ) {
        return std::make_shared<Texture_cubemap>(
            width,
            height,
            mipmap_levels,
            internal_format,
            wraps,
            filters
        );
    }

    static std::shared_ptr<Texture_cubemap> create_image(
        const std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>>& images
    ) {
        return create(
            images,
            1,
            Texture_internal_format::SRGB_ALPHA,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::W, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::LINEAR},
                {Texture_filter_target::MAG, Texture_filter::LINEAR}
            }
        );
    }

    static std::shared_ptr<Texture_cubemap> create_color_attachemnt(
        int width,
        int height
    ) {
        return create(
            width,
            height,
            1,
            Texture_internal_format::RGB_ALPHA,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::W, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::LINEAR},
                {Texture_filter_target::MAG, Texture_filter::LINEAR}
            }
        );
    }

    static std::shared_ptr<Texture_cubemap> create_depth_attachemnt(
        int width,
        int height
    ) {
        return create(
            width,
            height,
            1,
            Texture_internal_format::DEPTH_32F,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_BORDER},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_BORDER},
                {Texture_wrap_target::W, Texture_wrap::CLAMP_TO_BORDER}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::NEAREST},
                {Texture_filter_target::MAG, Texture_filter::NEAREST}
            }
        );
    }

    static std::shared_ptr<Texture_cubemap> create_depth_stencil_attachemnt(
        int width,
        int height
    ) {
        return create(
            width,
            height,
            1,
            Texture_internal_format::DEPTH_STENCIL_24_8,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::W, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::NEAREST},
                {Texture_filter_target::MAG, Texture_filter::NEAREST}
            }
        );
    }
};


};