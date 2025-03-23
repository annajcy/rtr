#include "engine/global/base.h"
#include "engine/runtime/renderer.h"

using namespace rtr;

int main() {

    Renderer_descriptor renderer_descriptor{};
    renderer_descriptor.width = 800;
    renderer_descriptor.height = 600;
    renderer_descriptor.title = "RTR Engine";
    auto renderer = std::make_shared<Renderer>(renderer_descriptor);

    
    
    
    return 0;
}