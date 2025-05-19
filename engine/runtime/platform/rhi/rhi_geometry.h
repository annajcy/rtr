#pragma once

#include "engine/runtime/global/base.h" 
#include "rhi_buffer.h"


namespace rtr {

enum class Draw_mode {
    POINTS,
    LINES,
    LINE_LOOP,
    LINE_STRIP,
    TRIANGLES,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
};

class RHI_geometry  {
protected:
    std::shared_ptr<RHI_buffer> m_element_buffer{};
    std::unordered_map<unsigned int, std::shared_ptr<RHI_buffer>> m_vertex_buffers{};

public:
    RHI_geometry(
        const std::unordered_map<unsigned int, std::shared_ptr<RHI_buffer>> &vertex_buffers, 
        const std::shared_ptr<RHI_buffer>& element_buffer
    ) : m_vertex_buffers(vertex_buffers) , 
        m_element_buffer(element_buffer) {}

    using Ptr = std::shared_ptr<RHI_geometry>;

    virtual ~RHI_geometry() {}

    virtual void bind_buffers() = 0;
    virtual void bind_vertex_buffer(unsigned int location, const std::shared_ptr<RHI_buffer>& vbo) = 0;

    virtual void draw(Draw_mode mode = Draw_mode::TRIANGLES) = 0;
    virtual void draw_instanced(unsigned int instance_count, Draw_mode mode = Draw_mode::TRIANGLES) = 0;

};

}
