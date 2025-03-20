#pragma once
#include "engine/global/base.h"
#include "engine/runtime/loader/image_loader.h"
#include "engine/runtime/rhi/texture/rhi_texture.h"
#include <memory>
#include <vector>

namespace rtr {

class Texture : public GUID {
protected:
    Texture_type m_type{};
    Texture_format m_internal_format{};
    Texture_format m_external_format{};
    Texture_buffer_type m_buffer_type{};

public:
    Texture(
        Texture_type type,
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type
    ) : GUID(),
        m_type(type),
        m_internal_format(internal_format),
        m_external_format(external_format),
        m_buffer_type(buffer_type) {}

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
        int width,
        int height,
        std::shared_ptr<Image> image
    ) : Texture(type, internal_format, external_format, buffer_type),
        m_width(width),
        m_height(height),
        m_image(image) {}

    Texture_2D(
        int width,
        int height,
        std::shared_ptr<Image> image
    ) : Texture(Texture_type::TEXTURE_2D, Texture_format::SRGB_ALPHA, Texture_format::RGB_ALPHA, Texture_buffer_type::UNSIGNED_BYTE),
        m_width(width),
        m_height(height),
        m_image(image) {}

    int width() const { return m_width; }
    int height() const { return m_height; }
    std::shared_ptr<Image>& image() { return m_image; }
    std::shared_ptr<Image> image() const { return m_image; }
    virtual ~Texture_2D() = default;


    static std::shared_ptr<Texture_2D> create_color_attachment(
        int width,
        int height
    ) {
        return std::make_shared<Texture_2D>(
            Texture_type::TEXTURE_2D, 
            Texture_format::RGB_ALPHA, 
            Texture_format::RGB_ALPHA, 
            Texture_buffer_type::UNSIGNED_BYTE, 
            width, height, nullptr
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
            width, height, nullptr
        );
    }
 
};

class Texture_cube_map : public Texture {
protected:
    std::unordered_map<Texture_cube_map_face, std::shared_ptr<Image>> m_face_images{};
public:
    Texture_cube_map(
        std::unordered_map<Texture_cube_map_face, std::shared_ptr<Image>> face_images
    ) : Texture(Texture_type::TEXTURE_CUBE_MAP, Texture_format::SRGB_ALPHA, Texture_format::RGB_ALPHA, Texture_buffer_type::UNSIGNED_BYTE),
        m_face_images(face_images) {}

    std::unordered_map<Texture_cube_map_face, std::shared_ptr<Image>>& face_images() { return m_face_images; }
    std::unordered_map<Texture_cube_map_face, std::shared_ptr<Image>> face_images() const { return m_face_images; }
    virtual ~Texture_cube_map() = default;

};

}