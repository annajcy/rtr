#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/rhi_resource.h"


namespace rtr {

class RHI_frame_buffer : public RHI_resource
{
protected:
    int m_width{};
    int m_height{};
    std::vector<unsigned int> m_color_attachments{};
    unsigned int m_depth_attachment{};

public:
    RHI_frame_buffer(
        int width, 
        int height,
        const std::vector<unsigned int>& color_attachments,
        unsigned int depth_attachment
    ) : RHI_resource(RHI_resource_type::FRAME_BUFFER), 
        m_width(width), 
        m_height(height),
        m_color_attachments(color_attachments),
        m_depth_attachment(depth_attachment) {
            RHI_resource_manager::add_resource(this);
            for (auto& attachment : m_color_attachments) {
                RHI_resource_manager::add_dependency(guid(), attachment);
            }
            if (m_depth_attachment) {
                RHI_resource_manager::add_dependency(guid(), m_depth_attachment);
            }
        }

    virtual ~RHI_frame_buffer() {
        RHI_resource_manager::remove_resource(guid());
    }

    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual bool is_valid() = 0;
    virtual unsigned int id() = 0;

    std::vector<unsigned int>& color_attachments() { return m_color_attachments; }
    unsigned int& depth_attachment() { return m_depth_attachment; }

    unsigned int& color_attachment(unsigned int index) { 
        if (index >= m_color_attachments.size()) {
            throw std::runtime_error("Index out of range");
        }
        return m_color_attachments[index]; 
    }

    int width() { return m_width; }
    int height() { return m_height; }
    
};

};