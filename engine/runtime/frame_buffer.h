#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/device/rhi_device.h"
#include "engine/runtime/rhi/frame_buffer/rhi_frame_buffer.h"
#include "engine/runtime/texture.h"
#include <memory>
#include <vector>
namespace rtr {

class Frame_buffer : public GUID  {
protected:
    int m_width{};
    int m_height{};
    std::vector<std::shared_ptr<Texture_2D>> m_color_attachments{};
    std::shared_ptr<Texture_2D> m_depth_attachment{};

public:

    Frame_buffer(
        int width,
        int height,
        unsigned int color_attachment_count
    ) : GUID(),
        m_width(width),
        m_height(height),
        m_color_attachments(Texture_2D::create_color_attachments(width, height, color_attachment_count)),
        m_depth_attachment(Texture_2D::create_depth_attachment(width, height)) {}

    std::vector<std::shared_ptr<Texture_2D>>& color_attachments() { return m_color_attachments; }
    std::shared_ptr<Texture_2D>& depth_attachment() { return m_depth_attachment; }

    int width() const { return m_width; }
    int height() const { return m_height; }

    std::shared_ptr<RHI_frame_buffer> create_rhi_frame_buffer(const std::shared_ptr<RHI_device>& device) {
        std::vector<std::shared_ptr<RHI_texture_2D>> rhi_color_attachments{};
        for (auto& attachment : m_color_attachments) {
            rhi_color_attachments.push_back(attachment->create_rhi_texture_2D(device));
        }
        auto rhi_depth_attachment = m_depth_attachment->create_rhi_texture_2D(device);
        return std::make_shared<RHI_frame_buffer>(
            device,
            rhi_color_attachments,
            rhi_depth_attachment
        );
    }

};

};