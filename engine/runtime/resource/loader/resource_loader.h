#pragma once
#include "engine/global/base.h"

#include "engine/runtime/resource/loader/image_loader.h"
#include "engine/runtime/resource/loader/model_loader.h"
#include "engine/runtime/resource/loader/text_loader.h"

namespace rtr {

class Resource_loader {
protected:
    std::shared_ptr<Image_loader> m_image_loader{};
    std::shared_ptr<Model_loader> m_model_loader{};
    std::shared_ptr<Text_loader> m_text_loader{};

public:
    Resource_loader() : 
    m_image_loader(std::make_shared<Image_loader>()),
    m_text_loader(std::make_shared<Text_loader>()),
    m_model_loader(std::make_shared<Model_loader>()) {}
    virtual ~Resource_loader() = default;
    virtual void load() = 0;
    virtual void unload() = 0;

    std::shared_ptr<Image> load_image(
        Image_format format,
        const std::string& path,
        bool flip_y = true
    ) {
        return m_image_loader->load_from_path(format, path, flip_y);
    }

    std::shared_ptr<Image> load_image(
        Image_format format,
        const unsigned char* data,
        unsigned int size,
        bool flip_y = true
    ) {
        return m_image_loader->load_from_data(format, data, size, flip_y);
    }

    std::shared_ptr<Model> load_model(const std::string& path) {
        return m_model_loader->load(path);
    }

    std::shared_ptr<Text> load_text(const std::string& path) {
        return m_text_loader->load_from_path(path);
    }
};


}