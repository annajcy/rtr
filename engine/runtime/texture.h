#pragma once
#include "engine/global/base.h"
#include "engine/runtime/loader/image_loader.h"
#include "engine/runtime/rhi/texture/rhi_texture.h"
#include <memory>

namespace rtr {

class Texture {
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
    ) : m_type(type),
        m_internal_format(internal_format),
        m_external_format(external_format),
        m_buffer_type(buffer_type) {}

};

class Texture_2D : public Texture {
protected:
    int m_width{};
    int m_height{};
    std::shared_ptr<Image> m_image{};

public:
    Texture_2D(
        int width,
        int height,
        std::shared_ptr<Image> image
    ) : Texture(Texture_type::TEXTURE_2D, Texture_format::SRGB_ALPHA, Texture_format::RGB_ALPHA, Texture_buffer_type::UNSIGNED_BYTE),
        m_width(width),
        m_height(height),
        m_image(image) {}

};

class Texture_cube_map : public Texture {
protected:
    std::unordered_map<Texture_cube_map_face, std::shared_ptr<Image>> m_face_images{};
public:
    Texture_cube_map(
        std::unordered_map<Texture_cube_map_face, std::shared_ptr<Image>> face_images
    ) : Texture(Texture_type::TEXTURE_CUBE_MAP, Texture_format::SRGB_ALPHA, Texture_format::RGB_ALPHA, Texture_buffer_type::UNSIGNED_BYTE),
        m_face_images(face_images) {}

};

}