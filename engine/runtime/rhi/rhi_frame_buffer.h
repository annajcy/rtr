#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/rhi_resource.h"
#include "engine/runtime/rhi/rhi_texture.h"


namespace rtr {

class RHI_frame_buffer : public RHI_resource
{
protected:
    int m_width{};
    int m_height{};
    std::vector<RHI_texture::Ptr> m_color_attachments{};
    RHI_texture::Ptr m_depth_attachment{};

public:
    RHI_frame_buffer(
        int width, 
        int height,
        const std::vector<RHI_texture::Ptr>& color_attachments,
        const RHI_texture::Ptr& depth_attachment
    ) : RHI_resource(RHI_resource_type::FRAME_BUFFER), 
        m_width(width), 
        m_height(height),
        m_color_attachments(color_attachments),
        m_depth_attachment(depth_attachment) {}

    using Ptr = std::shared_ptr<RHI_frame_buffer>;

    virtual ~RHI_frame_buffer() {}

    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual bool is_valid() = 0;

    int width() { return m_width; }
    int height() { return m_height; }
    
};

};