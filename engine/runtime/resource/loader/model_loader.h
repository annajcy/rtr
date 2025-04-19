#pragma once

#include "engine/runtime/framework/game_object.h"
#include "engine/runtime/global/base.h" 
#include "engine/runtime/resource/resource_base.h"
#include <memory>
#include <vector>

namespace rtr {

class Model : public Resource_base {
    std::vector<std::shared_ptr<Game_object>> m_model_game_objects{};

public:
    Model(const std::string& path) : 
    Resource_base(
        Resource_type::MODEL, 
        Hash::from_string(path)) {}

    virtual ~Model() = default;


};

class Model_loader {
public:
    static std::shared_ptr<Model> load(const std::string& path) { return nullptr; }
};

};