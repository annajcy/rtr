#pragma once
#include "engine/global/base.h"

namespace rtr {

enum class Loader_type {
    STB_IMAGE
};

enum class Image_format {
    RGB,
    GRAY,
    RGB_ALPHA,
    GRAY_ALPHA
};

class Image {

protected:
    int m_width{};
    int m_height{};
    int m_channels{};
    unsigned char* m_data{nullptr};
    unsigned int m_data_size{};
    bool m_flip_y{true};
    Image_format m_format{};
    std::string m_path{};
    
public:
    
    Image(
        Image_format format,
        unsigned char* data, 
        int data_size, 
        bool flip_y = true
    ) : m_format(format),
        m_data(data), 
        m_data_size(data_size),
        m_flip_y(flip_y) {}

    Image(
        Image_format format,
        const std::string& path, 
        bool flip_y = true
    ) : m_format(format),
        m_path(path), 
        m_flip_y(flip_y) {}

    virtual ~Image() = default;
    
    virtual void init_from_path() = 0;
    virtual void init_from_data() = 0;

    virtual void destroy() = 0;

    int width() const { return m_width; }
    int height() const { return m_height; }
    int channels() const { return m_channels; }
    int data_size() const { return m_data_size; }
    Image_format format() const { return m_format; }
    std::string path() const { return m_path; }
    bool flip_y() const { return m_flip_y; }
    unsigned char* data() const { return m_data; }

};

class Image_loader {
protected:
    Loader_type m_loader_type{};
public:
    Image_loader(Loader_type loader_type) : m_loader_type(loader_type) {}
    virtual ~Image_loader() = default;

    virtual std::shared_ptr<Image> load_from_path(
        Image_format format,
        const std::string& path,
        bool flip_y = true
    ) = 0;

    virtual std::shared_ptr<Image> load_from_data(
        Image_format format,
        unsigned char* data,
        int data_size,
        bool flip_y = true
    ) = 0;
};

};