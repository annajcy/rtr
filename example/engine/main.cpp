#include "engine/runtime/function/framework/component/node/node_component.h"
#include "engine/runtime/global/base.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/function/framework/game_object.h"
#include "engine/runtime/function/framework/scene.h"
#include "engine/runtime/function/framework/world.h"
#include "engine/runtime/function/framework/component/component_base.h"
#include "engine/runtime/core/geometry.h"

using namespace rtr;

int main() {

    auto game_object = Game_object::create("go1");

    auto node_component = Node_component::create();
    game_object->add_component<Node_component>(node_component);

    auto geometry = Geometry::create_box();
    
    return 0;
}