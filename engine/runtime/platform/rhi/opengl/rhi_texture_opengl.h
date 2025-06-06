#pragma once

#include "engine/runtime/tool/base.h" 

#include "../rhi_texture.h"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace rtr {

inline constexpr unsigned int gl_texture_type(Texture_type type) {
    switch (type) {
        case Texture_type::TEXTURE_2D:
            return GL_TEXTURE_2D;
        case Texture_type::TEXTURE_CUBEMAP:
            return GL_TEXTURE_CUBE_MAP;
        case Texture_type::TEXTURE_2D_ARRAY:
            return GL_TEXTURE_2D_ARRAY;
        default:
            return GL_TEXTURE_2D;
    }
}

inline constexpr unsigned int gl_texture_internal_format(Texture_internal_format format) {
    switch (format) {
        case Texture_internal_format::R_8F:
            return GL_R8;
        case Texture_internal_format::R_16F:
            return GL_R16F;
        case Texture_internal_format::R_32F:
            return GL_R32F;
        case Texture_internal_format::RG_8F:
            return GL_RG8;  
        case Texture_internal_format::RG_16F:
            return GL_RG16F; 
        case Texture_internal_format::RG_32F:
            return GL_RG32F; 
        case Texture_internal_format::RGB_8F:
            return GL_RGB8; 
        case Texture_internal_format::RGB_16F:
            return GL_RGB16F; 
        case Texture_internal_format::RGB_32F:
            return GL_RGB32F; 
        case Texture_internal_format::RGB_ALPHA_8F:
            return GL_RGBA8;
        case Texture_internal_format::RGB_ALPHA_16F:
            return GL_RGBA16F;
        case Texture_internal_format::RGB_ALPHA_32F:
            return GL_RGBA32F;
        case rtr::Texture_internal_format::DEPTH_24F:
            return GL_DEPTH_COMPONENT24;
        case Texture_internal_format::DEPTH_32F:
            return GL_DEPTH_COMPONENT32F;
        case Texture_internal_format::SRGB_ALPHA_8F:
            return GL_SRGB8_ALPHA8;  // 添加位数
        case Texture_internal_format::SRGB_8F:
            return GL_SRGB8;         // 添加位数
        case Texture_internal_format::DEPTH_STENCIL_24F_8F:
            return GL_DEPTH24_STENCIL8;
        default:
            return GL_RGB8;  // 默认也带位数
    }
}

inline constexpr unsigned int gl_texture_external_format(Texture_external_format format) {
    switch (format) {
        case Texture_external_format::RGB:
            return GL_RGB;
        case Texture_external_format::RGB_ALPHA:
            return GL_RGBA;
        case Texture_external_format::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        case Texture_external_format::DEPTH:
            return GL_DEPTH_COMPONENT;
        case Texture_external_format::SRGB_ALPHA:
            return GL_SRGB_ALPHA;
        case Texture_external_format::SRGB:
            return GL_SRGB;
        default:
            return GL_RGB;
    }
}

inline constexpr unsigned int gl_texture_buffer_type(Texture_buffer_type type) {
    switch (type) {
        case Texture_buffer_type::UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case Texture_buffer_type::UNSIGNED_INT:
            return GL_UNSIGNED_INT;
        case Texture_buffer_type::UNSIGNED_INT_24_8:
            return GL_UNSIGNED_INT_24_8;
        case Texture_buffer_type::FLOAT:
            return GL_FLOAT;
        default:
            return GL_UNSIGNED_BYTE;
    }
}

inline constexpr unsigned int gl_texture_wrap(Texture_wrap wrap) {
    switch (wrap) {
        case Texture_wrap::REPEAT:
            return GL_REPEAT;
        case Texture_wrap::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case Texture_wrap::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case Texture_wrap::CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
        default:
            return GL_REPEAT;
    }
}

inline constexpr unsigned int gl_texture_filter(Texture_filter filter) {
    switch (filter) {
        case Texture_filter::NEAREST:
            return GL_NEAREST;
        case Texture_filter::LINEAR:
            return GL_LINEAR;
        case Texture_filter::NEAREST_MIPMAP_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        case Texture_filter::LINEAR_MIPMAP_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;
        case Texture_filter::NEAREST_MIPMAP_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        case Texture_filter::LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
            return GL_NEAREST;
    }
}

inline constexpr unsigned int gl_texture_cube_map_face(Texture_cubemap_face face) {
    switch (face) {
        case Texture_cubemap_face::RIGHT:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        case Texture_cubemap_face::LEFT:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case Texture_cubemap_face::TOP:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        case Texture_cubemap_face::BOTTOM:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case Texture_cubemap_face::FRONT:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        case Texture_cubemap_face::BACK:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        default:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    }
}

inline constexpr unsigned int gl_texture_warp_target(Texture_wrap_target target) {
    switch (target) {
        case Texture_wrap_target::U:
            return GL_TEXTURE_WRAP_S;
        case Texture_wrap_target::V:
            return GL_TEXTURE_WRAP_T;
        case Texture_wrap_target::W:
            return GL_TEXTURE_WRAP_R;
        default:
            return GL_TEXTURE_WRAP_S;
    }
}

inline constexpr unsigned int gl_texture_filter_target(Texture_filter_target target) {
    switch (target) {
        case Texture_filter_target::MIN:
            return GL_TEXTURE_MIN_FILTER;
        case Texture_filter_target::MAG:
            return GL_TEXTURE_MAG_FILTER;
        default:
            return GL_TEXTURE_MIN_FILTER;
    }
}

class RHI_texture_OpenGL : public RHI_texture {
protected:
    unsigned int m_texture_id{};

public:
    RHI_texture_OpenGL(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_type type,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters
    ) : RHI_texture(
        width,
        height,
        mipmap_levels,
        type,
        internal_format,
        wraps,
        filters
    ) {}
        
    virtual ~RHI_texture_OpenGL() {
        if (m_texture_id) {
            glDeleteTextures(1, &m_texture_id);
        }
    }

    void bind_to_unit(unsigned int location) override {
        glBindTextureUnit(location, m_texture_id);
    }

    unsigned int texture_id() const {
        return m_texture_id;
    }

    void generate_mipmap() override {
        glGenerateTextureMipmap(m_texture_id);
    }

    void set_filter(Texture_filter_target target, Texture_filter filter) override {
        glTextureParameteri(
            m_texture_id, 
            gl_texture_filter_target(target), 
            gl_texture_filter(filter)
        );
    }

    void set_wrap(Texture_wrap_target target, Texture_wrap wrap) override {
        glTextureParameteri(
            m_texture_id, 
            gl_texture_warp_target(target), 
            gl_texture_wrap(wrap)
        );
    }

    void on_set_border_color() override {
        glTextureParameterfv(
            m_texture_id,
            GL_TEXTURE_BORDER_COLOR,
            glm::value_ptr(m_border_color)
        );
    }

};

class RHI_texture_2D_OpenGL : public RHI_texture_OpenGL, public IRHI_texture_2D {
public: 
    RHI_texture_2D_OpenGL(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters
    ) : RHI_texture_OpenGL(
        width,
        height,
        mipmap_levels,
        Texture_type::TEXTURE_2D,
        internal_format,
        wraps,
        filters
    ) {

        glCreateTextures(gl_texture_type(m_type), 1, &m_texture_id);
        glTextureStorage2D(
            m_texture_id, 
            m_mipmap_levels, 
            gl_texture_internal_format(m_internal_format), 
            m_width, 
            m_height
        );

        apply_filters();
        apply_wraps();
    }

    virtual ~RHI_texture_2D_OpenGL() {}

    bool upload_data(const Image_data& image) override {

        if (image.data == nullptr) {
            std::cout << "[RHI_texture_2D_OpenGL] Image data is nullptr" << std::endl;
            return false;
        }

        glTextureSubImage2D(
            m_texture_id, 
            0, 
            0, 0, 
            m_width, m_height,
            gl_texture_external_format(image.external_format), 
            gl_texture_buffer_type(image.buffer_type), 
            image.data
        );

        if (m_mipmap_levels > 1) generate_mipmap();
        return true;
    }  

    bool upload_data(const std::shared_ptr<RHI_texture>& image) override {
        if (auto gl_image = std::dynamic_pointer_cast<RHI_texture_2D_OpenGL>(image)) {
            glCopyImageSubData(
                gl_image->texture_id(),
                gl_texture_type(gl_image->type()),
                0,
                0, 0, 0,
                m_texture_id,
                gl_texture_type(m_type),
                0,
                0, 0, 0,
                m_width,
                m_height,
                1
            );
            if (m_mipmap_levels > 1) generate_mipmap();
            return true;
        } else {
            std::cerr << "Unsupported texture type or invaild texture" << std::endl;
            return false;
        }
    }

    Image_data get_image_data() override {
        
        if (m_internal_format == Texture_internal_format::SRGB_ALPHA_8F) {
            auto data = new unsigned char[m_width * m_height * 4];

            glGetTextureImage(
                m_texture_id,
                0,
                gl_texture_external_format(Texture_external_format::RGB_ALPHA),
                gl_texture_buffer_type(Texture_buffer_type::UNSIGNED_BYTE),
                static_cast<GLsizei>(m_width * m_height * 4),
                data
            );

            return Image_data{
                .width = m_width,
                .height = m_height,
                .data = data,
                .buffer_type = Texture_buffer_type::UNSIGNED_BYTE,
                .external_format = Texture_external_format::RGB_ALPHA,
                .has_ownership = true
            };

        } else if (m_internal_format == Texture_internal_format::DEPTH_32F) {
            auto data = new float[m_width * m_height];
            glGetTextureImage(
                m_texture_id,
                0,
                gl_texture_external_format(Texture_external_format::DEPTH),
                gl_texture_buffer_type(Texture_buffer_type::FLOAT),
                static_cast<GLsizei>(m_width * m_height),
                data
            );

            return Image_data{
                .width = m_width,
                .height = m_height,
                .data = reinterpret_cast<unsigned char*>(data),
                .buffer_type = Texture_buffer_type::FLOAT,
                .external_format = Texture_external_format::DEPTH,
                .has_ownership = true
            };
        } else {
            throw std::runtime_error("Unsupported texture buffer type");
        }
        
    }
};


class RHI_texture_cubemap_OpenGL : public RHI_texture_OpenGL, public IRHI_texture_cubemap {
public:
    RHI_texture_cubemap_OpenGL(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters
    ) : IRHI_texture_cubemap(),
        RHI_texture_OpenGL(
        width,
        height,
        mipmap_levels,
        Texture_type::TEXTURE_CUBEMAP,
        internal_format,
        wraps,
        filters
    ) {
        glCreateTextures(gl_texture_type(m_type), 1, &m_texture_id);
        glTextureStorage2D(
            m_texture_id,
            m_mipmap_levels,
            gl_texture_internal_format(m_internal_format),
            m_width,
            m_height
        );

        apply_filters();
        apply_wraps();
    }

    virtual ~RHI_texture_cubemap_OpenGL() {}

    bool upload_data(const std::unordered_map<Texture_cubemap_face, Image_data>& images) override {
        
        for (const auto& [face, image] : images) {
    
            if (image.data == nullptr) {
                std::cout << "[RHI_texture_cubemap_OpenGL] Image data is nullptr" << std::endl;
                return false;
            }

            glTextureSubImage3D(
                m_texture_id,
                0,
                0, 0, static_cast<unsigned int>(face),
                m_width, m_height, 1,
                gl_texture_external_format(image.external_format),
                gl_texture_buffer_type(image.buffer_type),
                image.data
            );
        }

        if (m_mipmap_levels > 1) generate_mipmap();
        return true;
    }

    bool upload_data(const std::unordered_map<Texture_cubemap_face, std::shared_ptr<RHI_texture>>& images) override {
        for (const auto& [face, image] : images) {
            if (auto gl_image = std::dynamic_pointer_cast<RHI_texture_2D_OpenGL>(image)) {
                glCopyImageSubData(
                    gl_image->texture_id(),
                    gl_texture_type(gl_image->type()),
                    0,
                    0, 0, 0,
                    m_texture_id,
                    gl_texture_type(m_type),
                    0,
                    0, 0, static_cast<unsigned int>(face),
                    m_width,
                    m_height,
                    1
                );
            } else {
                std::cerr << "Unsupported texture type or invaild texture" << std::endl;
                return false;
            }
        }
        if (m_mipmap_levels > 1) generate_mipmap();
        return true;
    }

    std::unordered_map<Texture_cubemap_face, Image_data> get_image_data() override {
        std::unordered_map<Texture_cubemap_face, Image_data> images{};
        const std::vector<Texture_cubemap_face> faces = {
            Texture_cubemap_face::FRONT,
            Texture_cubemap_face::BACK,
            Texture_cubemap_face::LEFT,
            Texture_cubemap_face::RIGHT,
            Texture_cubemap_face::TOP,
            Texture_cubemap_face::BOTTOM
        };

        for (const auto& face : faces) {
            if (m_internal_format == Texture_internal_format::SRGB_ALPHA_8F) {
                auto data = new unsigned char[m_width * m_height * 4];
                glGetTextureSubImage(
                    m_texture_id,
                    0,
                    0, 0, static_cast<unsigned int>(face),
                    m_width, m_height, 1,
                    gl_texture_external_format(Texture_external_format::RGB_ALPHA),
                    gl_texture_buffer_type(Texture_buffer_type::UNSIGNED_BYTE),
                    static_cast<GLsizei>(m_width * m_height * 4),
                    data
                );
                
                images[face] = Image_data{
                    .width = m_width,
                    .height = m_height,
                    .data = data,
                    .buffer_type = Texture_buffer_type::UNSIGNED_BYTE,
                    .external_format = Texture_external_format::RGB_ALPHA,
                    .has_ownership = true
                };
            } else if (m_internal_format == Texture_internal_format::DEPTH_32F) {
                auto data = new float[m_width * m_height];
                glGetTextureSubImage(
                    m_texture_id,
                    0,
                    0, 0, static_cast<unsigned int>(face),
                    m_width, m_height, 1,
                    gl_texture_external_format(Texture_external_format::DEPTH),
                    gl_texture_buffer_type(Texture_buffer_type::FLOAT),
                    static_cast<GLsizei>(m_width * m_height),
                    data
                );
                images[face] = Image_data{
                    .width = m_width,
                    .height = m_height,
                    .data = reinterpret_cast<unsigned char*>(data),
                    .buffer_type = Texture_buffer_type::FLOAT,
                    .external_format = Texture_external_format::DEPTH,
                    .has_ownership = true
                };
            } else {
                throw std::runtime_error("Unsupported texture buffer type");
            }
        }
        return images;
    }

};

class RHI_texture_2D_array_OpenGL : public RHI_texture_OpenGL, public IRHI_texture_2D_array {
public:
    RHI_texture_2D_array_OpenGL(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const unsigned int layer_count
    ) : IRHI_texture_2D_array(layer_count),
        RHI_texture_OpenGL(
        width,
        height,
        mipmap_levels,
        Texture_type::TEXTURE_2D_ARRAY,
        internal_format,
        wraps,
        filters
    ) {
        glCreateTextures(gl_texture_type(m_type), 1, &m_texture_id);
        glTextureStorage3D(
            m_texture_id,
            m_mipmap_levels,
            gl_texture_internal_format(m_internal_format),
            m_width,
            m_height,
            m_layer_count
        );

        apply_filters();
        apply_wraps();
    }

    virtual ~RHI_texture_2D_array_OpenGL() {}

    bool upload_data(const std::vector<std::shared_ptr<RHI_texture>>& images) override {
        if (images.size() != m_layer_count) {
            std::cerr << "Image count does not match layer count" << std::endl;
            return false;
        }

        for (size_t i = 0; i < images.size(); i++) {
            auto& image = images[i];
            if (auto gl_image = std::dynamic_pointer_cast<RHI_texture_2D_OpenGL>(image)) {
                glCopyImageSubData(
                    gl_image->texture_id(),
                    gl_texture_type(gl_image->type()),
                    0,
                    0, 0, 0,
                    m_texture_id,
                    gl_texture_type(m_type),
                    0,
                    0, 0, static_cast<unsigned int>(i),
                    m_width,
                    m_height,
                    1
                );
            } else {
                std::cerr << "Unsupported texture type or invaild texture" << std::endl;
                return false;
            }
        }
        if (m_mipmap_levels > 1) generate_mipmap();
        return true;
    }

    bool upload_data(const std::vector<Image_data>& images) override {
        if (images.size() != m_layer_count) {
            std::cerr << "Image count does not match layer count" << std::endl;
            return false;
        }

        for (size_t i = 0; i < images.size(); i++) {
            const auto& image = images[i];
    
            if (image.data == nullptr) {
                std::cout << "[RHI_texture_2D_array_OpenGL] Image data is nullptr" << std::endl;
                return false;
            }
            
            glTextureSubImage3D(
                m_texture_id,
                0,
                0, 0, static_cast<unsigned int>(i),
                m_width, m_height, 1,
                gl_texture_external_format(image.external_format),
                gl_texture_buffer_type(image.buffer_type),
                image.data
            );
        }

        if (m_mipmap_levels > 1) generate_mipmap();
        return true;
    }

    std::vector<Image_data> get_image_data() override {
        std::vector<Image_data> images{};
        for (size_t i = 0; i < m_layer_count; i++) {
            if (m_internal_format == Texture_internal_format::SRGB_ALPHA_8F) {
                auto data = new unsigned char[m_width * m_height * 4];
                glGetTextureSubImage(
                    m_texture_id,
                    0,
                    0, 0, static_cast<unsigned int>(i),
                    m_width, m_height, 1,
                    gl_texture_external_format(Texture_external_format::RGB_ALPHA),
                    gl_texture_buffer_type(Texture_buffer_type::UNSIGNED_BYTE),
                    static_cast<GLsizei>(m_width * m_height * 4),
                    data
                );
                images.push_back(Image_data{
                    .width = m_width,
                    .height = m_height,
                    .data = data,
                    .buffer_type = Texture_buffer_type::UNSIGNED_BYTE,
                    .external_format = Texture_external_format::RGB_ALPHA,
                    .has_ownership = true
                });
            } else if (m_internal_format == Texture_internal_format::DEPTH_32F) {
                auto data = new float[m_width * m_height];
                glGetTextureSubImage(
                    m_texture_id,
                    0,
                    0, 0, static_cast<unsigned int>(i),
                    m_width, m_height, 1,
                    gl_texture_external_format(Texture_external_format::DEPTH),
                    gl_texture_buffer_type(Texture_buffer_type::FLOAT),
                    static_cast<GLsizei>(m_width * m_height),
                    data
                );
                images.push_back(Image_data{
                   .width = m_width,
                   .height = m_height,
                   .data = reinterpret_cast<unsigned char*>(data),
                   .buffer_type = Texture_buffer_type::FLOAT,
                   .external_format = Texture_external_format::DEPTH,
                   .has_ownership = true
                });
            }
        }
        return images;
    }
};

class RHI_texture_builder_OpenGL : public RHI_texture_builder {
public:
    RHI_texture_builder_OpenGL() {}
    
    virtual ~RHI_texture_builder_OpenGL() {}

    bool build_texture_2D(
        std::shared_ptr<RHI_texture>& texture_2D,
        const std::shared_ptr<RHI_texture>& texture_2D_source
    ) override {
        if (auto gl_texture_2D = std::dynamic_pointer_cast<RHI_texture_2D_OpenGL>(texture_2D)) {
            return gl_texture_2D->upload_data(texture_2D_source);
        }
        return false;
    }

    bool build_texture_2D(
        std::shared_ptr<RHI_texture>& texture_2D,
        const Image_data& image
    ) override {
        if (auto gl_texture_2D = std::dynamic_pointer_cast<RHI_texture_2D_OpenGL>(texture_2D)) {
            return  gl_texture_2D->upload_data(image);
        }
        return false;
    }

    bool build_texture_cubemap(
        std::shared_ptr<RHI_texture>& texture_cubemap,
        const std::unordered_map<Texture_cubemap_face, std::shared_ptr<RHI_texture>>& texture_cubemaps
    ) override {
        if (auto gl_texture_cubemap = std::dynamic_pointer_cast<RHI_texture_cubemap_OpenGL>(texture_cubemap)) {
           return gl_texture_cubemap->upload_data(texture_cubemaps);
        }
        return false;
    }

    bool build_texture_cubemap(
        std::shared_ptr<RHI_texture>& texture_cubemap,
        const std::unordered_map<Texture_cubemap_face, Image_data>& images
    ) override {
        if (auto gl_texture_cubemap = std::dynamic_pointer_cast<RHI_texture_cubemap_OpenGL>(texture_cubemap)) {
            gl_texture_cubemap->upload_data(images);
            return true;
        }
        return false;
    }

    bool build_texture_2D_array(
        std::shared_ptr<RHI_texture>& texture_2D_array, 
        const std::vector<std::shared_ptr<RHI_texture>>& texture_2Ds
    ) override {
        if (auto gl_texture_2D_array = std::dynamic_pointer_cast<RHI_texture_2D_array_OpenGL>(texture_2D_array)) {
            return gl_texture_2D_array->upload_data(texture_2Ds);
        }
        return false;
    }
    
    bool build_texture_2D_array(
        std::shared_ptr<RHI_texture>& texture_2D_array,
        const std::vector<Image_data>& images
    ) override {
        if (auto gl_texture_2D_array = std::dynamic_pointer_cast<RHI_texture_2D_array_OpenGL>(texture_2D_array)) {
            return  gl_texture_2D_array->upload_data(images);
        }
        return false;
    }

};


};