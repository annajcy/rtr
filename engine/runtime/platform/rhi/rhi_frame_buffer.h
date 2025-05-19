#pragma once

#include "rhi_texture.h"
#include "rhi_window.h"
#include <memory>

namespace rtr {

class RHI_frame_buffer_base {
public:
    RHI_frame_buffer_base() {}
    virtual ~RHI_frame_buffer_base() {}
    virtual bool is_valid() const = 0;
    virtual int width() = 0;
    virtual int height() = 0;
};

class RHI_frame_buffer : public RHI_frame_buffer_base
{
protected:
    int m_width{};
    int m_height{};
    std::vector<std::shared_ptr<RHI_texture>> m_color_attachments{};
    std::shared_ptr<RHI_texture> m_depth_attachment{};
    
public:
    RHI_frame_buffer() {}

    RHI_frame_buffer(
        int width, 
        int height,
        const std::vector<std::shared_ptr<RHI_texture>>& color_attachments,
        const std::shared_ptr<RHI_texture>& depth_attachment
    ) : m_width(width), 
        m_height(height),
        m_color_attachments(color_attachments),
        m_depth_attachment(depth_attachment) {}

    virtual ~RHI_frame_buffer() {}

    virtual int width() override { return m_width; }
    virtual int height() override { return m_height; }

    const std::vector<std::shared_ptr<RHI_texture>>& color_attachments() const { return m_color_attachments; }
    const std::shared_ptr<RHI_texture>& depth_attachment() const { return m_depth_attachment; }
};

class RHI_screen_buffer : public RHI_frame_buffer_base {
protected:
    std::shared_ptr<RHI_window> m_window{};

public:
    RHI_screen_buffer(
        const std::shared_ptr<RHI_window>& window
    ) : m_window(window) {}

    virtual ~RHI_screen_buffer() {}

    const std::shared_ptr<RHI_window>& window() const { return m_window; }

    virtual int width() override { return m_window->width(); }
    virtual int height() override { return m_window->height(); }

};

};