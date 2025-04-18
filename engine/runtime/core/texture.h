#pragma once

#include "engine/runtime/global/enum.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include <memory>
#include <unordered_map>
namespace rtr {

class Texture {
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

};



};