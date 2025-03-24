#pragma once
#include "engine/global/base.h"
#include "engine/runtime/loader/image_loader.h"
#include "engine/runtime/rhi/device/rhi_device.h"
#include "engine/runtime/rhi/texture/rhi_texture.h"
#include <memory>
#include <unordered_map>

namespace rtr {

class Texture : public GUID {
protected:
    Texture_type m_type{};
    Texture_format m_internal_format{};
    Texture_format m_external_format{};
    Texture_buffer_type m_buffer_type{};
    std::unordered_map<Texture_wrap_target, Texture_wrap> m_wrap_map{};
    std::unordered_map<Texture_filter_target, Texture_filter> m_filter_map{};
    bool is_generate_mipmap{};

public:

    Texture(
        Texture_type type,
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wrap_map,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filter_map,
        bool is_generate_mipmap
    ) : GUID(),
        m_type(type),
        m_internal_format(internal_format),
        m_external_format(external_format),
        m_buffer_type(buffer_type),
        m_wrap_map(wrap_map),
        m_filter_map(filter_map),
        is_generate_mipmap(is_generate_mipmap) {}

    Texture_type type() const { return m_type; }
    Texture_format internal_format() const { return m_internal_format; }
    Texture_format external_format() const { return m_external_format; }
    Texture_buffer_type buffer_type() const { return m_buffer_type; }
    virtual ~Texture() = default;

};

class Texture_2D : public Texture {
protected:
    int m_width{};
    int m_height{};
    std::shared_ptr<Image> m_image{};

public:
    Texture_2D(
        Texture_type type,
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wrap_map,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filter_map,
        bool is_generate_mipmap,
        const std::shared_ptr<Image>& image
    ) : Texture(
        type, 
        internal_format, 
        external_format, 
        buffer_type,
        wrap_map,
        filter_map,
        is_generate_mipmap),
        m_width(image->width()),
        m_height(image->height()),
        m_image(image) {}

    Texture_2D(
        Texture_type type,
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wrap_map,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filter_map,
        bool is_generate_mipmap,
        int width,
        int height
    ) : Texture(
        type, 
        internal_format, 
        external_format, 
        buffer_type,
        wrap_map,
        filter_map,
        is_generate_mipmap),
        m_width(width),
        m_height(height),
        m_image(nullptr) {}

    Texture_2D(
        const std::shared_ptr<Image>& image
    ) : Texture(
        Texture_type::TEXTURE_2D, 
        Texture_format::SRGB_ALPHA, 
        Texture_format::RGB_ALPHA, 
        Texture_buffer_type::UNSIGNED_BYTE,
        std::unordered_map<Texture_wrap_target, Texture_wrap>{
            {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
            {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE} 
        },
        std::unordered_map<Texture_filter_target, Texture_filter>{
            {Texture_filter_target::MIN, Texture_filter::LINEAR_MIPMAP_LINEAR}, // 需要与 mipmap 生成匹配
            {Texture_filter_target::MAG, Texture_filter::LINEAR}  // 修正 MAG 过滤器类型
        },
        true),  
        m_width(image->width()),
        m_height(image->height()),
        m_image(image) {}

    int width() const { return m_width; }
    int height() const { return m_height; }
    std::shared_ptr<Image>& image() { return m_image; }
    const std::shared_ptr<Image>& image() const { return m_image; }
    virtual ~Texture_2D() = default;

    std::shared_ptr<RHI_texture_2D> create_rhi_texture_2D(const std::shared_ptr<RHI_device>& device) {
        auto tex = device->create_texture_2D(
            id(),
            internal_format(),
            external_format(),
            buffer_type(),
            width(),
            height(),
            image()->data()
        );

        for (auto& [target, wrap] : m_wrap_map) {
            tex->set_wrap(target, wrap);
        }

        for (auto& [target, filter] : m_filter_map) {
            tex->set_filter(target, filter);
        }

        if (is_generate_mipmap) {
            tex->generate_mipmap();
        }

        return tex;
    }


    static std::shared_ptr<Texture_2D> create_color_attachment(
        int width,
        int height
    ) {
        return std::make_shared<Texture_2D>(
            Texture_type::TEXTURE_2D,
            Texture_format::RGB_ALPHA,
            Texture_format::RGB_ALPHA,
            Texture_buffer_type::UNSIGNED_BYTE,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::LINEAR},
                {Texture_filter_target::MAG, Texture_filter::LINEAR}
            },
            false,
            width,
            height
        );
    }

    static std::vector<std::shared_ptr<Texture_2D>> create_color_attachments(
        int width,
        int height,
        unsigned int count
    ) {
        std::vector<std::shared_ptr<Texture_2D>> attachments{};
        for (int i = 0; i < count; i++) {
            attachments.push_back(create_color_attachment(width, height));
        }
        return attachments;
    }

    static std::shared_ptr<Texture_2D> create_depth_attachment(
        int width,
        int height
    ) {
        return std::make_shared<Texture_2D>(
            Texture_type::TEXTURE_2D,
            Texture_format::DEPTH_STENCIL_24_8,
            Texture_format::DEPTH_STENCIL,
            Texture_buffer_type::UNSIGNED_INT_24_8,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_BORDER},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_BORDER}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::NEAREST},
                {Texture_filter_target::MAG, Texture_filter::NEAREST}
            },
            false,
            width,
            height
        );
    }
 
};

class Texture_cube_map : public Texture {
protected:
    std::unordered_map<Texture_cube_map_face, std::shared_ptr<Image>> m_face_images{};
public:
    Texture_cube_map(
        const std::unordered_map<Texture_cube_map_face, std::shared_ptr<Image>>& face_images
    ) : Texture(
        Texture_type::TEXTURE_CUBE_MAP, 
        Texture_format::SRGB_ALPHA, 
        Texture_format::RGB_ALPHA, 
        Texture_buffer_type::UNSIGNED_BYTE,
        std::unordered_map<Texture_wrap_target, Texture_wrap>{   // 环绕模式建议保持 CLAMP_TO_EDGE
            {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
            {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE},
            {Texture_wrap_target::W, Texture_wrap::CLAMP_TO_EDGE} // 立方体贴图必须设置 W 轴
        },
        std::unordered_map<Texture_filter_target, Texture_filter>{   // 过滤模式建议升级为带 mipmap 的配置
            {Texture_filter_target::MIN, Texture_filter::LINEAR_MIPMAP_LINEAR}, // 改为三线性过滤
            {Texture_filter_target::MAG, Texture_filter::LINEAR}
        }, 
        true), // 启用 mipmap 生成
        m_face_images(face_images) {}

    Texture_cube_map(
        const std::unordered_map<Texture_cube_map_face, std::shared_ptr<Image>>& face_images,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wrap_map,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filter_map,
        const bool is_generate_mipmap
    ) : Texture(
        Texture_type::TEXTURE_CUBE_MAP, 
        Texture_format::SRGB_ALPHA, 
        Texture_format::RGB_ALPHA, 
        Texture_buffer_type::UNSIGNED_BYTE,
        wrap_map,
        filter_map,
        is_generate_mipmap),
        
        m_face_images(face_images) {}

    std::unordered_map<Texture_cube_map_face, std::shared_ptr<Image>>& face_images() { return m_face_images; }
    const std::unordered_map<Texture_cube_map_face, std::shared_ptr<Image>>& face_images() const { return m_face_images; }
    virtual ~Texture_cube_map() = default;

    std::shared_ptr<RHI_texture_cube_map> create_rhi_texture_cube_map(const std::shared_ptr<RHI_device>& device) {

        if (face_images().size() != 6) {
            throw std::runtime_error("Texture_cube_map must have 6 face images");
        }

        std::unordered_map<Texture_cube_map_face, RHI_texture_cube_map::Face_data> face_data{};

        for (auto& [face, image] : face_images()) {
            face_data[face] = {
                image->width(),
                image->height(),
                image->data()
            };
        }

        auto tex = device->create_texture_cube_map(
            id(),
            internal_format(),
            external_format(),
            buffer_type(),
            face_data
        );

        for (auto& [target, wrap] : m_wrap_map) {
            tex->set_wrap(target, wrap);
        }

        for (auto& [target, filter] : m_filter_map) {
            tex->set_filter(target, filter);
        }

        if (is_generate_mipmap) {
            tex->generate_mipmap();
        }

        return tex;
    }

};

using Binded_texture = std::pair<unsigned int, std::shared_ptr<Texture>>;

}