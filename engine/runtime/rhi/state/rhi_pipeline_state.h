#pragma once
#include "engine/global/base.h"

namespace rtr {

enum class Depth_function {
    NEVER,
    LESS,
    EQUAL,
    LESS_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_EQUAL,
    ALWAYS,
};

struct Depth_state {
    bool test_enable{true};
    bool write_enable{true};
    Depth_function function{Depth_function::LESS};

    static Depth_state opaque() {
        return {
            true,
            true,
            Depth_function::LESS
        };
    }

    static Depth_state translucent() {
        return {
            true,
            false,
            Depth_function::LESS
        };
    }
};

struct Polygon_offset_state {
    bool point_enabled{false};
    bool line_enabled{false};
    bool fill_enabled{false};
    float factor{0.0f};
    float units{0.0f};

    static Polygon_offset_state disabled() {
        return {
            false,
            false,
            false,
            0.0f,
            0.0f
        };
    }

    static Polygon_offset_state enabled() {
        return {
            true,
            true,
            true,
            1.0f,
            1.0f
        };
    }
};

enum class Stencil_operation {
    KEEP,
    ZERO,
    REPLACE,
    INCR,
    DECR,
    INVERT,
    INCR_WRAP,
    DECR_WRAP,  
};

enum class Stencil_function {
    NEVER,
    LESS,
    EQUAL,
    LESS_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_EQUAL,
    ALWAYS
};

struct Stencil_state {
    bool enable{false};
    Stencil_function function{Stencil_function::ALWAYS};
    Stencil_operation stencil_fail{Stencil_operation::KEEP};
    Stencil_operation depth_fail{Stencil_operation::KEEP};
    Stencil_operation depth_pass{Stencil_operation::KEEP};
    unsigned int reference{1};
    unsigned int mask{0xff};
    unsigned int function_mask{0xff};

    static Stencil_state disabled() {
        return {
            false,
            Stencil_function::ALWAYS,
            Stencil_operation::KEEP,
            Stencil_operation::KEEP,
            Stencil_operation::KEEP,
            1,
            0xff,
            0xff
        };
    }

    static Stencil_state opaque() {
        return {
            true,
            Stencil_function::ALWAYS,
            Stencil_operation::KEEP,
            Stencil_operation::KEEP,
            Stencil_operation::REPLACE,
            1,
            0xff,
            0xff
        };
    }

    static Stencil_state edge() {
        return {
            true,
            Stencil_function::NOT_EQUAL,
            Stencil_operation::KEEP,
            Stencil_operation::KEEP,
            Stencil_operation::KEEP,
            1,
            0x00,
            0xff
        };
    }
};

enum class Blend_factor {
    ZERO,
    ONE,
    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,
    DST_COLOR,
    ONE_MINUS_DST_COLOR,
    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA
};

enum class Blend_operation {
    ADD,
    SUBTRACT,
    REVERSE_SUBTRACT,
    MIN,
    MAX,
};

struct Blend_state {
    bool enable{false};
    Blend_factor src_color_factor{Blend_factor::ONE};
    Blend_factor dst_color_factor{Blend_factor::ZERO};

    Blend_factor src_alpha_factor{Blend_factor::ONE};
    Blend_factor dst_alpha_factor{Blend_factor::ZERO};

    Blend_operation color_operation{Blend_operation::ADD};
    Blend_operation alpha_operation{Blend_operation::ADD};

    
    static Blend_state disabled() {
        return {
            false,
            Blend_factor::ONE,
            Blend_factor::ZERO,
            Blend_factor::ONE,
            Blend_factor::ZERO,
            Blend_operation::ADD,
            Blend_operation::ADD
        };
    }

    static Blend_state opaque() {
        return {
            true,
            Blend_factor::ONE,
            Blend_factor::ZERO,
            Blend_factor::ONE,
            Blend_factor::ZERO,
            Blend_operation::ADD,
            Blend_operation::ADD
        };
    }

    static Blend_state translucent() {
        return {
            true,
            Blend_factor::SRC_ALPHA,
            Blend_factor::ONE_MINUS_SRC_ALPHA,
            Blend_factor::ONE,
            Blend_factor::ZERO,
            Blend_operation::ADD,
            Blend_operation::ADD
        };
    }

};

enum class Cull_mode {
    NONE,
    FRONT,
    BACK,
    FRONT_AND_BACK,
};

enum class Front_face {
    COUNTER_CLOCKWISE,
    CLOCKWISE
};

struct Cull_state {
    bool enable{false};
    Cull_mode mode{Cull_mode::BACK};
    Front_face front_face{Front_face::COUNTER_CLOCKWISE};

    static Cull_state disabled() {
        return {
            false,
            Cull_mode::BACK,
            Front_face::COUNTER_CLOCKWISE
        };
    }

    static Cull_state front() {
        return {
            true,
            Cull_mode::FRONT,
            Front_face::COUNTER_CLOCKWISE
        };
    }

    static Cull_state back() {
        return {
            true,
            Cull_mode::BACK,
            Front_face::COUNTER_CLOCKWISE
        };
    }

    static Cull_state front_and_back() {
        return {
            true,
            Cull_mode::FRONT_AND_BACK,
            Front_face::COUNTER_CLOCKWISE
        };
    }

};

struct Clear_state {
    bool color{false};
    bool depth{false};
    bool stencil{false};

    float depth_clear_value{1.0f};
    unsigned int stencil_clear_value{0x00};
    glm::vec4 color_clear_value{0.0f, 0.0f, 0.0f, 0.0f};

    static Clear_state enabled() {
        return {
            true,
            true,
            true,
            1.0f,
            0x00,
            glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
        };
    }
};

enum class State_global_profile {
    UNSPECIFIED,
    OPAQUE,
    TRANSLUCENT,
    EDGE,
};

class RHI_pipeline_state {

protected:
    State_global_profile m_profile{State_global_profile::UNSPECIFIED};
    Depth_state m_depth_state{};
    Polygon_offset_state m_polygon_offset_state{};
    Stencil_state m_stencil_state{};
    Blend_state m_blend_state{};
    Cull_state m_cull_state{};
    Clear_state m_clear_state{};
    unsigned int m_clear_mask{};

public:

    RHI_pipeline_state() = default;
    RHI_pipeline_state(State_global_profile profile) : m_profile(profile) {}
    virtual ~RHI_pipeline_state() = default;
    
    void init() {

        if (m_profile == State_global_profile::UNSPECIFIED) {
            return;
        }

        if (m_profile == State_global_profile::OPAQUE) {
            m_depth_state = Depth_state::opaque();
            m_polygon_offset_state = Polygon_offset_state::disabled();
            m_stencil_state = Stencil_state::opaque();
            m_blend_state = Blend_state::opaque();
            m_cull_state = Cull_state::back();
            m_clear_state = Clear_state::enabled();
        } else if (m_profile == State_global_profile::TRANSLUCENT) {
            m_depth_state = Depth_state::translucent();
            m_polygon_offset_state = Polygon_offset_state::disabled();
            m_stencil_state = Stencil_state::opaque();
            m_blend_state = Blend_state::translucent();
            m_cull_state = Cull_state::back();
            m_clear_state = Clear_state::enabled();
        } else if (m_profile == State_global_profile::EDGE) {
            m_depth_state = Depth_state::opaque();
            m_polygon_offset_state = Polygon_offset_state::enabled();
            m_stencil_state = Stencil_state::edge();
            m_blend_state = Blend_state::opaque();
            m_cull_state = Cull_state::back();
            m_clear_state = Clear_state::enabled();
        }
    }
    
    void apply() {
        apply_blend_state();
        apply_cull_state();
        apply_depth_state();
        apply_polygon_offset_state();
        apply_stencil_state();
        apply_clear_state();
    }

    virtual void apply_blend_state() = 0;
    virtual void apply_cull_state() = 0;
    virtual void apply_depth_state() = 0;
    virtual void apply_polygon_offset_state() = 0;
    virtual void apply_stencil_state() = 0;
    virtual void apply_clear_state() = 0;

    State_global_profile& profile() { return m_profile; }
    Blend_state& blend_state() { return m_blend_state; }
    Cull_state& cull_state() { return m_cull_state; }
    Depth_state& depth_test_state() { return m_depth_state; }
    Polygon_offset_state& polygon_offset_state() { return m_polygon_offset_state; }
    Stencil_state& stencil_state() { return m_stencil_state; }
    Clear_state& clear_buffers() { return m_clear_state; }

    unsigned int clear_mask() { return m_clear_mask; }

};

};