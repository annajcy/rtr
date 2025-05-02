#pragma once

#include "engine/runtime/global/base.h" 
#include "rhi_texture_opengl.h"
#include "../rhi_frame_buffer.h"
#include "../rhi_texture.h"
#include <memory>

namespace rtr {

class RHI_frame_buffer_base_OpenGL {
protected:
    unsigned int m_frame_buffer_id{0};

public:
    RHI_frame_buffer_base_OpenGL() {}

    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer_id);
    }

    void unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int frame_buffer_id() const { return m_frame_buffer_id; }
    
};

class RHI_frame_buffer_OpenGL : public RHI_frame_buffer, public RHI_frame_buffer_base_OpenGL {
public:

    RHI_frame_buffer_OpenGL(
        int width, 
        int height,
        const std::vector<std::shared_ptr<RHI_texture>>& color_attachments,
        const std::shared_ptr<RHI_texture>& depth_attachment
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
        if (m_frame_buffer_id) {
            glDeleteFramebuffers(1, &m_frame_buffer_id);
        }
    }

    void attach() {
        // 附加颜色附件
        for (size_t i = 0; i < m_color_attachments.size(); ++i) {
            auto color_attachment = std::dynamic_pointer_cast<RHI_texture_2D_OpenGL>(m_color_attachments[i]);
            
            if (!color_attachment) {
                std::cerr << "Invalid color attachment: not a RHI_texture_2D_OpenGL" << std::endl;
                return;
            }

            if (glIsTexture(color_attachment->texture_id())) {
                glNamedFramebufferTexture(
                    m_frame_buffer_id,
                    GL_COLOR_ATTACHMENT0 + i,
                    color_attachment->texture_id(),
                    0
                );
            } else {
                std::cerr << "Invalid color attachment texture ID: "  << std::endl;
            }
        }

        // 附加深度附件
        if (auto depth_attachment = std::dynamic_pointer_cast<RHI_texture_2D_OpenGL>(m_depth_attachment)) {
            if (glIsTexture(depth_attachment->texture_id())) {
                glNamedFramebufferTexture(
                    m_frame_buffer_id,
                    GL_DEPTH_ATTACHMENT,
                    depth_attachment->texture_id(),
                    0
                );
            } else {
                std::cerr << "Invalid depth attachment texture " << std::endl;
            }
        } else {
            std::cerr << "Invalid depth attachment: not a RHI_texture_2D_OpenGL" << std::endl;
        }

        // 设置绘制目标
        std::vector<unsigned int> draw_buffers;
        for (size_t i = 0; i < m_color_attachments.size(); ++i) {
            draw_buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
        }

        if (!draw_buffers.empty()) {
            glNamedFramebufferDrawBuffers(
                m_frame_buffer_id,
                static_cast<int>(draw_buffers.size()),
                draw_buffers.data()
            );
        } else {
            glNamedFramebufferDrawBuffer(m_frame_buffer_id, GL_NONE);
        }
    }

    bool is_valid() const override {
        unsigned int status = glCheckNamedFramebufferStatus(m_frame_buffer_id, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            // 输出具体错误原因（如GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT）
            log_framebuffer_error(status);
            return false;
        }
        return true;
    }

private:
    void log_framebuffer_error(GLenum status) const {
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

class RHI_screen_buffer_OpenGL : public RHI_screen_buffer, public RHI_frame_buffer_base_OpenGL {
public:
    RHI_screen_buffer_OpenGL(
        const std::shared_ptr<RHI_window>& window
    ) : RHI_screen_buffer(window) {}

    virtual ~RHI_screen_buffer_OpenGL() {}

    bool is_valid() const override {
        return m_window != nullptr;
    }
};

} // namespace rtr