#pragma once
#include "engine/global/base.h"

#include "engine/runtime/resource/loader/image_loader.h"
#include "engine/runtime/resource/loader/model_loader.h"
#include "engine/runtime/resource/loader/text_loader.h"

namespace rtr {

class Resource_loader {

public:

    Image::Ptr load_image(
        Image_format format,
        const std::string& path,
        bool flip_y = true
    ) {
        return Image_loader::load_from_path(format, path, flip_y);
    }

    Image::Ptr load_image(
        Image_format format,
        const unsigned char* data,
        unsigned int size,
        bool flip_y = true
    ) {
        return Image_loader::load_from_data(format, data, size, flip_y);
    }

    Model::Ptr load_model(const std::string& path) {
        return Model_loader::load(path);
    }

    Text::Ptr load_text(const std::string& path) {
        return Text_loader::load_from_path(path);
    }
};


}