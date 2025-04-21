#pragma once

#include "engine/runtime/context/engine_tick_context.h"
#include "engine/runtime/core/geometry.h"
#include "engine/runtime/core/shader.h"
#include "engine/runtime/core/texture.h"
#include "engine/runtime/global/enum.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include <memory>

namespace rtr {
    
class Render_pass {

public:
    Render_pass() {}
    virtual ~Render_pass() {}
    virtual void excute() = 0;
};

class Main_pass : public Render_pass {

};

class Gamma_pass : public Render_pass {

};



}