#pragma once
#include "engine/global/base.h"

#include "engine/runtime/rhi/texture/rhi_texture.h"

namespace rtr {

class RHI_frame_buffer
{
protected:
    unsigned int m_width{};
    unsigned int m_height{};
    std::vector<std::shared_ptr<RHI_texture_2D>> m_color_attachments{};
    std::shared_ptr<RHI_texture_2D> m_depth_attachment{};

public:
    RHI_frame_buffer(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}
    virtual ~RHI_frame_buffer() = default;

    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual bool is_valid() = 0;

    std::vector<std::shared_ptr<RHI_texture_2D>>& color_attachments() { return m_color_attachments; }
    std::shared_ptr<RHI_texture_2D>& depth_attachment() { return m_depth_attachment; }

    std::shared_ptr<RHI_texture_2D>& color_attachment(unsigned int index) { 
        if (index >= m_color_attachments.size()) {
            throw std::runtime_error("Index out of range");
        }
        return m_color_attachments[index]; 
    }

    void add_color_attachments(const std::vector<std::shared_ptr<RHI_texture_2D>>& attachments) { 
        m_color_attachments.insert(m_color_attachments.end(), attachments.begin(), attachments.end());
    }

    void add_color_attachment(const std::shared_ptr<RHI_texture_2D>& attachment) { 
        m_color_attachments.push_back(attachment); 
    }

    void remove_color_attachment(const std::shared_ptr<RHI_texture_2D>& attachment) { 
        m_color_attachments.erase(std::remove(m_color_attachments.begin(), m_color_attachments.end(), attachment), m_color_attachments.end()); 
    }
    
};

};