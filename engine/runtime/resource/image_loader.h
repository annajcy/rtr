#pragma once
#include "engine/global/base.h"
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

class Image : public Resource_base {

protected:
    int m_width{};
    int m_height{};
    int m_channels{};
    const unsigned char* m_data{nullptr};
    unsigned int m_data_size{};
    Image_format m_format{};
    
public:
    
    Image(
        Image_format format,
        const unsigned char* data, 
        unsigned int data_size, 
        bool flip_y = true
    ) : Resource_base(Resource_type::IMAGE, Hash::from_raw_data(data, data_size)),
        m_format(format){}

    Image(
        Image_format format,
        const std::string& path, 
        bool flip_y = true
    ) : Resource_base(Resource_type::IMAGE, Hash::from_string(path)),
        m_format(format){}

    virtual ~Image() = default;

    int width() const { return m_width; }
    int height() const { return m_height; }
    int channels() const { return m_channels; }
    Image_format format() const { return m_format; }
    const unsigned char* data() const { return m_data; }
    int data_size() const { return m_data_size; }

};

class Image_loader {
protected:
    Image_loader_type m_type{};
public:
    Image_loader(Image_loader_type type) : m_type(type) {}
    virtual ~Image_loader() = default;

    virtual std::shared_ptr<Image> load_from_path(
        Image_format format,
        const std::string& path,
        bool flip_y = true
    ) = 0;

    virtual std::shared_ptr<Image> load_from_data(
        Image_format format,
        const unsigned char* data,
        unsigned int data_size,
        bool flip_y = true
    ) = 0;
};

};