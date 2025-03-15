#pragma once
#include "engine/global/base.h"
#include "engine/runtime/loader/image_loader.h"

namespace rtr {

inline unsigned int stbi_image_format(Image_format format) {
    switch (format) {
        case Image_format::RGB:
            return STBI_rgb;
        case Image_format::GRAY:
            return STBI_grey;
        case Image_format::RGB_ALPHA:
            return STBI_rgb_alpha;
        case Image_format::GRAY_ALPHA:
            return STBI_grey_alpha;
        default:
            return STBI_rgb;
    }
}

class Image_stb : public Image {

public:
    Image_stb(
        Image_format format,
        unsigned char* data,
        int data_size,
        bool flip_y = true
    ) : Image(format, data, data_size, flip_y) {
        init_from_data();
    }

    Image_stb(
        Image_format format,
        const std::string& path,
        bool flip_y = true
    ) : Image(format, path, flip_y) {
        init_from_path();
    }

    virtual ~Image_stb() {
        destroy();
    }

    virtual void init_from_path() override {
        stbi_set_flip_vertically_on_load(m_flip_y);
        m_data = stbi_load(m_path.c_str(), &m_width, &m_height, &m_channels, stbi_image_format(m_format));
        m_data_size = m_width * m_height * m_channels * sizeof(unsigned char);
    }

    virtual void init_from_data() override {
        stbi_set_flip_vertically_on_load(m_flip_y);
        m_data = stbi_load_from_memory(m_data, m_data_size, &m_width, &m_height, &m_channels, stbi_image_format(m_format));
    }

    virtual void destroy() override {
        stbi_image_free(m_data);
    }

};

class Image_loader_stb : public Image_loader {
public:
    Image_loader_stb() = default;
    virtual ~Image_loader_stb() = default;
    virtual std::shared_ptr<Image> load_from_path(
        Image_format format,
        const std::string& path,
        bool flip_y = true
    ) override {
        return std::make_shared<Image_stb>(format, path, flip_y);
    }

    virtual std::shared_ptr<Image> load_from_data(
        Image_format format,
        unsigned char* data,
        int data_size,
        bool flip_y = true
    ) override {
        return std::make_shared<Image_stb>(format, data, data_size, flip_y);
    }

};

}