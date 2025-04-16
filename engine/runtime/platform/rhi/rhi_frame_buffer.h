#pragma once

#include "engine/runtime/global/base.h" 
#include "rhi_texture.h"
#include "rhi_window.h"
#include <memory>


namespace rtr {

class RHI_frame_buffer 
{
protected:
    int m_width{};
    int m_height{};
    std::vector<std::shared_ptr<RHI_texture>> m_color_attachments{};
    std::shared_ptr<RHI_texture> m_depth_attachment{};
    std::shared_ptr<RHI_window> m_window{};

public:
    RHI_frame_buffer(
        const std::shared_ptr<RHI_window>& window
    ) : m_window(window) {}

    RHI_frame_buffer(
        int width, 
        int height,
        const std::vector<std::shared_ptr<RHI_texture>>& color_attachments,
        const std::shared_ptr<RHI_texture>& depth_attachment
    ) : m_width(width), 
        m_height(height),
        m_color_attachments(color_attachments),
        m_depth_attachment(depth_attachment),
        m_window(nullptr) {}

    virtual ~RHI_frame_buffer() {}
    virtual bool is_valid() const = 0;
    bool is_screen() const {
        return m_window != nullptr;
    }

    int width() { return m_width; }
    int height() { return m_height; }

    const std::vector<std::shared_ptr<RHI_texture>>& color_attachments() const { return m_color_attachments; }
    const std::shared_ptr<RHI_texture>& depth_attachment() const { return m_depth_attachment; }
    const RHI_window::Ptr& window() const { return m_window; }
    
};

};