#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/frame_buffer/rhi_frame_buffer.h"
#include "engine/runtime/rhi/texture/rhi_texture_opengl.h"

namespace rtr {

class RHI_frame_buffer_OpenGL : public RHI_frame_buffer {
protected:
    unsigned int m_frame_buffer_id{};
    std::vector<unsigned int> m_color_attachment_ids{};

public:
    RHI_frame_buffer_OpenGL(
        int width, 
        int height,
        const std::vector<std::shared_ptr<RHI_texture_2D>>& color_attachments,
        const std::shared_ptr<RHI_texture_2D>& depth_attachment
    ) : RHI_frame_buffer(width, height, color_attachments, depth_attachment) {
        init();
    } 

    virtual ~RHI_frame_buffer_OpenGL() { destroy(); }
    virtual void init() override {
        glGenFramebuffers(1, &m_frame_buffer_id);
    }

    void update_color_attachment_ids() {
        m_color_attachment_ids.clear();
        for (auto& attachment : m_color_attachments) {
            auto gl_attachment = std::dynamic_pointer_cast<RHI_texture_2D_OpenGL>(attachment);
            if (gl_attachment) {
                m_color_attachment_ids.push_back(gl_attachment->texture_id());
            }
        }
    }

    virtual void bind() override {
        glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer_id);
        glDrawBuffers(m_color_attachments.size(), m_color_attachment_ids.data());
    }

    virtual void unbind() override {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);
    }

    virtual void destroy() override {
        glDeleteFramebuffers(1, &m_frame_buffer_id);
    }

    virtual bool is_valid() override {
        bind();
        auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        unbind();
        return result;
    }

    void attach() {
        bind();
        
        for (int i = 0; i < m_color_attachments.size(); i++) {
            auto gl_color_attachment = std::dynamic_pointer_cast<RHI_texture_2D_OpenGL>(m_color_attachments[i]);
            
            if (!gl_color_attachment) {
                continue;
            }
            
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + i,
                GL_TEXTURE_2D,
                gl_color_attachment->texture_id(),
                0
            );
        }

        if (m_depth_attachment) {
            auto gl_depth_attachment = std::dynamic_pointer_cast<RHI_texture_2D_OpenGL>(m_depth_attachment);
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                GL_TEXTURE_2D,
                gl_depth_attachment->texture_id(),
                0
            );
        }

        unbind();
    }

};

};