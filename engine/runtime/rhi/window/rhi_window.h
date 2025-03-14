#pragma once

#include "engine/global/base.h" 

namespace rtr {

class RHI_window {
protected:
    unsigned int m_width{};
    unsigned int m_height{};
    std::string m_title{};

public:

    RHI_window(
        unsigned int width, 
        unsigned int height, 
        std::string title
    ) : m_width(width), 
        m_height(height), 
        m_title(title) {}

    virtual ~RHI_window() = default;
    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void poll_events() = 0;
    virtual void swap_buffers() = 0;
    virtual bool is_active() = 0;


    const unsigned int& width() { return m_width; }
    const unsigned int& height() { return m_height; }
    const std::string& title() { return m_title; }




};

};


