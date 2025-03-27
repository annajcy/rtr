#pragma once
#include "engine/global/base.h"
#include <unordered_map>

namespace rtr {

class RHI_binding_state {
protected:
    unsigned int m_geometry{};
    unsigned int m_shader_program{};
    unsigned int m_frame_buffer{};
    std::unordered_map<unsigned int, unsigned int> m_textures{};

public:

    RHI_binding_state() = default;
    virtual ~RHI_binding_state() = default;

    virtual void init() {}

    virtual void bind() {}

    virtual void unbind() {}

    void clear() {}
    
};

};