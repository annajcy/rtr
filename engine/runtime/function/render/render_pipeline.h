#pragma once

#include "engine/runtime/function/render/render_pass.h"
#include "engine/runtime/global/base.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include <memory>
#include <vector>

namespace rtr {

class Render_pipeline {
public:
    Render_pipeline() {}
    virtual ~Render_pipeline() {}
    virtual void init(const std::shared_ptr<RHI_device>& device) = 0;
    virtual void excute(const Execute_context& context) = 0;
};
}