#pragma once

#include <memory>

namespace rtr {

class RHI_device;
class Input_system;
class Render_system;
class Window_system;
class File_service;
class World;

struct Global_context {
    std::shared_ptr<RHI_device> rhi_device{};
    std::shared_ptr<Input_system> input_system{};
    std::shared_ptr<Render_system> render_system{};
    std::shared_ptr<Window_system> window_system{};

    std::shared_ptr<File_service> file_service{};
    std::shared_ptr<World> world{};

};

struct Engine_tick_context {
    std::shared_ptr<Global_context> global_context{};
    float delta_time{};
    
};
}