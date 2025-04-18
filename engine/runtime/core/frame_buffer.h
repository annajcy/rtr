#pragma once

#include "engine/runtime/core/texture.h"
#include "engine/runtime/global/guid.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_frame_buffer.h"
#include "engine/runtime/platform/rhi/rhi_linker.h"
#include <memory>
#include <vector>
namespace rtr {

class Frame_buffer : public GUID, public RHI_linker<RHI_frame_buffer> {
protected:
    int m_width{};
    int m_height{};
    std::vector<std::shared_ptr<Texture>> m_color_attachments{};
    std::shared_ptr<Texture> m_depth_attachment{};

public:
    Frame_buffer(
        int width,
        int height,
        const std::vector<std::shared_ptr<Texture>>& color_attachments,
        const std::shared_ptr<Texture>& depth_attachment
    ) : m_width(width),
        m_height(height),
        m_color_attachments(color_attachments),
        m_depth_attachment(depth_attachment) {}

    virtual ~Frame_buffer() {}

    int width() const { return m_width; }
    int height() const { return m_height; }

    const std::vector<std::shared_ptr<Texture>>& color_attachments() const { return m_color_attachments; }
    const std::shared_ptr<Texture>& depth_attachment() const { return m_depth_attachment; }

    void link(const std::shared_ptr<RHI_device>& device) override {

        std::vector<std::shared_ptr<RHI_texture>> rhi_color_attachments{};

        for (const auto& color_attachment : m_color_attachments) {
            if (!color_attachment->is_linked()) {
                color_attachment->link(device);
            }
            rhi_color_attachments.push_back(color_attachment->rhi_resource());
        }

        if (!m_depth_attachment->is_linked()) {
            m_depth_attachment->link(device);
        }
        
        m_rhi_resource = device->create_frame_buffer(
            m_width, 
            m_height,
            rhi_color_attachments, 
            m_depth_attachment->rhi_resource()
        );
        
    }

    static std::shared_ptr<Frame_buffer> create(
        int width,
        int height,
        const std::vector<std::shared_ptr<Texture>>& color_attachments,
        const std::shared_ptr<Texture>& depth_attachment
    ) {
        return std::make_shared<Frame_buffer>(
            width,
            height,
            color_attachments, 
            depth_attachment
        );
    }
    
};

}