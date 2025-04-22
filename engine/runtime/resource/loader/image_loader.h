#pragma once
#include "engine/runtime/global/base.h" 
#include "engine/runtime/resource/resource_base.h"

namespace rtr {

enum class Image_loader_type {
    STB_IMAGE
};

enum class Image_format {
    RGB,
    GRAY,
    RGB_ALPHA,
    GRAY_ALPHA
};
    

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

class Image {
protected:
    int m_width{};
    int m_height{};
    int m_channels{};
    const unsigned char* m_data{nullptr};
    unsigned int m_data_size{};

public:
    Image(
        Image_format format,
        const unsigned char* data,
        unsigned int data_size,
        bool flip_y = true
    ) {
        stbi_set_flip_vertically_on_load(flip_y);
        m_data = stbi_load_from_memory(data, data_size, &m_width, &m_height, &m_channels, stbi_image_format(format));
        m_data_size = m_width * m_height * m_channels * sizeof(unsigned char);
    }

    Image(
        Image_format format,
        const std::string& path,
        bool flip_y = true
    ) {
        stbi_set_flip_vertically_on_load(flip_y);
        m_data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, stbi_image_format(format));
        m_data_size = m_width * m_height * m_channels * sizeof(unsigned char);
    }

    ~Image() {
        stbi_image_free(const_cast<void*>(reinterpret_cast<const void*>(m_data)));
    }

    int width() const { return m_width; }
    int height() const { return m_height; }
    int channels() const { return m_channels; }
    const unsigned char* data_ptr() const { return m_data; }
    int data_size() const { return m_data_size; }

};

class Image_loader {
public:
    static std::shared_ptr<Image> load_from_path(
        Image_format format,
        const std::string& path,
        bool flip_y = true
    ) {
        return std::make_shared<Image>(format, path, flip_y);
    }

    static std::shared_ptr<Image> load_from_data(
        Image_format format,
        const unsigned char* data,
        unsigned int data_size,
        bool flip_y = true
    ) {
        return std::make_shared<Image>(format, data, data_size, flip_y);
    }

};

}