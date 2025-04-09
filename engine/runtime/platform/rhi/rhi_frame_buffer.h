#pragma once

#include "engine/runtime/global/base.h" 
#include "rhi_resource.h"
#include "rhi_texture.h"
#include "rhi_window.h"


namespace rtr {

class RHI_frame_buffer : public RHI_resource
{
protected:
    int m_width{};
    int m_height{};
    std::vector<RHI_texture::Ptr> m_color_attachments{};
    RHI_texture::Ptr m_depth_attachment{};
    RHI_window::Ptr m_window{};

public:
    RHI_frame_buffer(
        const RHI_window::Ptr& window
    ) : RHI_resource(RHI_resource_type::FRAME_BUFFER), m_window(window) {}

    RHI_frame_buffer(
        int width, 
        int height,
        const std::vector<RHI_texture::Ptr>& color_attachments,
        const RHI_texture::Ptr& depth_attachment
    ) : RHI_resource(RHI_resource_type::FRAME_BUFFER), 
        m_width(width), 
        m_height(height),
        m_color_attachments(color_attachments),
        m_depth_attachment(depth_attachment),
        m_window(nullptr) {
            for (auto& attachment : m_color_attachments) {
                this->add_dependency(attachment);
            }
            if (m_depth_attachment) {
                this->add_dependency(m_depth_attachment);
            }
        }

    using Ptr = std::shared_ptr<RHI_frame_buffer>;

    virtual ~RHI_frame_buffer() {
        for (auto& attachment : m_color_attachments) {
            this->remove_dependency(attachment);
        }
        if (m_depth_attachment) {
            this->remove_dependency(m_depth_attachment);
        }
    }
    virtual bool is_valid() const = 0;
    bool is_screen() const {
        return m_window != nullptr;
    }

    int width() { return m_width; }
    int height() { return m_height; }

    const std::vector<RHI_texture::Ptr>& color_attachments() const { return m_color_attachments; }
    const RHI_texture::Ptr& depth_attachment() const { return m_depth_attachment; }
    const RHI_window::Ptr& window() const { return m_window; }
    
};

};