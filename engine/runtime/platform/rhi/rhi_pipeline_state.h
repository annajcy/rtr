#pragma once

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

    static Depth_state skybox() {
        return {
            true,
            false,
            Depth_function::LESS_EQUAL
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

    static Blend_state enabled() {
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


struct Pipeline_state {
    Depth_state depth_state{};
    Blend_state blend_state{};
    Polygon_offset_state polygon_offset_state{};
    Stencil_state stencil_state{};
    Cull_state cull_state{};


    static Pipeline_state shadow_pipeline_state() {
        return Pipeline_state{
            Depth_state::opaque(),
            Blend_state::disabled(),
            Polygon_offset_state::disabled(),
            Stencil_state::disabled(),
            Cull_state::disabled()
        };
    }
   
    static Pipeline_state opaque_pipeline_state() {
        return Pipeline_state{
            Depth_state::opaque(),
            Blend_state::disabled(),
            Polygon_offset_state::disabled(),
            Stencil_state::opaque(),
            Cull_state::back()
        };
    }

    static Pipeline_state translucent_pipeline_state() {
        return Pipeline_state{
            Depth_state::translucent(),
            Blend_state::enabled(),
            Polygon_offset_state::disabled(),
            Stencil_state::disabled(),
            Cull_state::back()
        };
    }

    static Pipeline_state edge_pipeline_state() {
        return Pipeline_state{
            Depth_state::opaque(),
            Blend_state::disabled(),
            Polygon_offset_state::disabled(),
            Stencil_state::edge(),
            Cull_state::back()
        };
    }

    static Pipeline_state skybox_pipeline_state() {
        return Pipeline_state{
            Depth_state::skybox(),
            Blend_state::disabled(),
            Polygon_offset_state::disabled(),
            Stencil_state::disabled(),
            Cull_state::disabled(),
        };
    }
};

class RHI_pipeline_state {

public:
    Pipeline_state state{};

    RHI_pipeline_state() { }
    RHI_pipeline_state(const Pipeline_state& pipeline) : state(pipeline) {}
    
    virtual ~RHI_pipeline_state() = default;
    
    void apply() {
        apply_blend_state();
        apply_cull_state();
        apply_depth_state();
        apply_polygon_offset_state();
        apply_stencil_state();
    }

    virtual void apply_blend_state() = 0;
    virtual void apply_cull_state() = 0;
    virtual void apply_depth_state() = 0;
    virtual void apply_polygon_offset_state() = 0;
    virtual void apply_stencil_state() = 0;

};

};