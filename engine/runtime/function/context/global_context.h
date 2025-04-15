#pragma once
#include "engine/runtime/function/render/render_struct.h"
#include <memory>

namespace rtr {

class RHI_device;
class Input_system;
class Render_system;
class Window_system;
class File_service;
class World;

struct Global_context {
    inline static std::shared_ptr<RHI_device> rhi_device{};
    
    inline static std::shared_ptr<Input_system> input_system{};
    inline static std::shared_ptr<Render_system> render_system{};
    inline static std::shared_ptr<Window_system> window_system{};

    inline static std::shared_ptr<File_service> file_service{};
    inline static std::shared_ptr<World> world{};

};

}