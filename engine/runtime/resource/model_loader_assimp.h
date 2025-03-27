#pragma once
#include "engine/global/base.h"
#include "engine/runtime/resource/loader/model_loader.h"

namespace rtr {

class Model_assimp : public Model {
public:
    Model_assimp(const std::string& path) : Model(path) {
        
    }
};

class Model_loader_assimp : public Model_loader { 
public:

};



};