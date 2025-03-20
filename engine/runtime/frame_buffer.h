#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/frame_buffer/rhi_frame_buffer.h"
#include "engine/runtime/texture.h"
#include <memory>
#include <vector>
namespace rtr {

class Frame_buffer : public GUID  {
protected:
    unsigned int m_width{};
    unsigned int m_height{};
    std::vector<std::shared_ptr<Texture_2D>> m_color_attachments{};
    std::shared_ptr<Texture_2D> m_depth_attachment{};

public:

    Frame_buffer(
        unsigned int width,
        unsigned int height,
        unsigned int color_attachment_count
    ) : GUID(),
        m_width(width),
        m_height(height),
        m_color_attachments(Texture_2D::create_color_attachments(width, height, color_attachment_count)),
        m_depth_attachment(Texture_2D::create_depth_attachment(width, height)) {}

    std::vector<std::shared_ptr<Texture_2D>>& color_attachments() { return m_color_attachments; }
    std::shared_ptr<Texture_2D>& depth_attachment() { return m_depth_attachment; }

};

};