#pragma once

#include "engine/runtime/global/enum.h"
#include "engine/runtime/platform/rhi/rhi_linker.h"
#include "engine/runtime/platform/rhi/rhi_texture.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include <memory>
#include <unordered_map>
namespace rtr {

class Texture : public GUID, public RHI_linker<RHI_texture> {
protected:
    Texture_type m_texture_type{};
public:
    Texture(Texture_type texture_type) : m_texture_type(texture_type) {}
    virtual ~Texture() {}
    Texture_type texture_type() const { return m_texture_type; }
};

class Texture2D : public Texture {
protected:
    std::shared_ptr<Image> m_image{};

public:
    Texture2D(
        const std::shared_ptr<Image>& image
    ) : Texture(Texture_type::TEXTURE_2D) {}
    virtual ~Texture2D() {}

    const std::shared_ptr<Image>& image() const { return m_image; }
    void set_image(const std::shared_ptr<Image>& image) { m_image = image; }

    void link(const std::shared_ptr<RHI_device>& device) override {

        if (m_image == nullptr) {
            std::cout << "Texture2D::link: m_image is nullptr" << std::endl;
            return;
        }

        m_rhi_resource = device->create_texture_2D(
            m_image->width(),
            m_image->height(),
            4,
            Texture_internal_format::SRGB_ALPHA,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
            }, 
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::LINEAR},
                {Texture_filter_target::MAG, Texture_filter::LINEAR}
            }, 
            Image_data{
                m_image->width(),
                m_image->height(),
                m_image->data_ptr(),
                Texture_buffer_type::UNSIGNED_BYTE,
                Texture_external_format::RGB_ALPHA
            }
        );
    }

    static std::shared_ptr<Texture2D> create(const std::shared_ptr<Image>& image) {
        return std::make_shared<Texture2D>(image);
    }

};

class Texture_color_attachment : public Texture {
protected:
    int m_width{};
    int m_height{};

public:
    Texture_color_attachment(
        int width,
        int height
    ) : Texture(Texture_type::TEXTURE_2D), m_width(width), m_height(height) {}
    virtual ~Texture_color_attachment() {}
    int width() const { return m_width; }
    int height() const { return m_height; }

    void link(const std::shared_ptr<RHI_device>& device) override {
        m_rhi_resource = device->create_texture_color_attachment(m_width, m_height);
    }

    static std::shared_ptr<Texture_color_attachment> create(int width, int height) {
        return std::make_shared<Texture_color_attachment>(width, height);
    }
};

class Texture_depth_attachment : public Texture {
protected:
    int m_width{};
    int m_height{};

public:
    Texture_depth_attachment(
        int width,
        int height
    ) : Texture(Texture_type::TEXTURE_2D), m_width(width), m_height(height) {}
    virtual ~Texture_depth_attachment() {}
    int width() const { return m_width; }
    int height() const { return m_height; }
    void link(const std::shared_ptr<RHI_device>& device) override {
        m_rhi_resource = device->create_texture_depth_attachment(m_width, m_height);
    }

    static std::shared_ptr<Texture_depth_attachment> create(int width, int height) {
        return std::make_shared<Texture_depth_attachment>(width, height);
    }
};

class Texture_cubemap : public Texture {
protected:
    std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>> m_images{};
public:
    Texture_cubemap(
        std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>> images
    ) : Texture(Texture_type::TEXTURE_CUBEMAP), m_images(images) {}
    virtual ~Texture_cubemap() {}
    const std::shared_ptr<Image>& image(Texture_cubemap_face face) const { return m_images.at(face); }
    void set_image(Texture_cubemap_face face, const std::shared_ptr<Image>& image) { m_images[face] = image; }

    void link(const std::shared_ptr<RHI_device>& device) override {
        if (m_images.size() != 6) {
            std::cout << "Texture_cubemap::link: number of m_images is not vaild" << std::endl;
            return;
        }

        std::unordered_map<Texture_cubemap_face, Image_data> image_datas{};
        for (const auto& [face, image] : m_images) {
            image_datas[face] = Image_data{
                image->width(),
                image->height(),
                image->data_ptr(),
                Texture_buffer_type::UNSIGNED_BYTE,
                Texture_external_format::RGB_ALPHA
            };
        }

        m_rhi_resource = device->create_texture_cubemap(
            m_images[Texture_cubemap_face::FRONT]->width(),
            m_images[Texture_cubemap_face::FRONT]->height(),
            1,
            Texture_internal_format::SRGB_ALPHA,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::LINEAR},
                {Texture_filter_target::MAG, Texture_filter::LINEAR}
            },
            image_datas
        );

    }

    static std::shared_ptr<Texture_cubemap> create(
        std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>> images
    ) {
        return std::make_shared<Texture_cubemap>(images);
    }

};

};