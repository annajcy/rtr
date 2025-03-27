#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/rhi_frame_buffer.h"

namespace rtr {

class RHI_frame_buffer_OpenGL : public RHI_frame_buffer {
protected:
    unsigned int m_frame_buffer_id{};

public:
    RHI_frame_buffer_OpenGL(
        int width, 
        int height,
        const std::vector<unsigned int>& color_attachments,
        unsigned int depth_attachment
    ) : RHI_frame_buffer(width, height, color_attachments, depth_attachment) 
    {
        int max_color_attachments{};
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
        if (color_attachments.size() > static_cast<size_t>(max_color_attachments)) {
            std::cerr << "Color attachments exceed limit: " 
                      << max_color_attachments << std::endl;
            return;
        }

        glCreateFramebuffers(1, &m_frame_buffer_id);
        attach();
        if (!is_valid()) {
            std::cerr << "ERROR::FRAMEBUFFER:: Initialization failed" << std::endl;
        }
    } 

    virtual ~RHI_frame_buffer_OpenGL() { 
        glDeleteFramebuffers(1, &m_frame_buffer_id);
    }

    void attach() {
        // 附加颜色附件
        for (size_t i = 0; i < m_color_attachments.size(); ++i) {
            if (glIsTexture(m_color_attachments[i])) {
                glNamedFramebufferTexture(
                    m_frame_buffer_id,
                    GL_COLOR_ATTACHMENT0 + i,
                    m_color_attachments[i],
                    0
                );
            } else {
                std::cerr << "Invalid color attachment texture ID: " 
                          << m_color_attachments[i] << std::endl;
            }
        }

        // 附加深度附件
        if (m_depth_attachment != 0) {
            if (glIsTexture(m_depth_attachment)) {
                glNamedFramebufferTexture(
                    m_frame_buffer_id,
                    GL_DEPTH_ATTACHMENT,
                    m_depth_attachment,
                    0
                );
            } else {
                std::cerr << "Invalid depth attachment texture ID: " 
                          << m_depth_attachment << std::endl;
            }
        }

        // 设置绘制目标
        std::vector<GLenum> drawBuffers;
        for (size_t i = 0; i < m_color_attachments.size(); ++i) {
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        if (!drawBuffers.empty()) {
            glNamedFramebufferDrawBuffers(
                m_frame_buffer_id,
                static_cast<GLsizei>(drawBuffers.size()),
                drawBuffers.data()
            );
        } else {
            // 无颜色附件时，显式设置不绘制颜色
            glNamedFramebufferDrawBuffers(m_frame_buffer_id, 0, nullptr);
        }
    }

    virtual bool is_valid() override {
        GLenum status = glCheckNamedFramebufferStatus(m_frame_buffer_id, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            // 输出具体错误原因（如GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT）
            log_framebuffer_error(status);
            return false;
        }
        return true;
    }

    virtual void bind() override {
        glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer_id);
    }

    virtual void unbind() override {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    virtual unsigned int id() override {
        return m_frame_buffer_id;
    }

private:
    void log_framebuffer_error(GLenum status) {
        const char* error = "Unknown error";
        switch(status) {
            case GL_FRAMEBUFFER_UNDEFINED: error = "GL_FRAMEBUFFER_UNDEFINED"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: error = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: error = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
        }
        std::cerr << "Framebuffer error: " << error << std::endl;
    }
};

} // namespace rtr