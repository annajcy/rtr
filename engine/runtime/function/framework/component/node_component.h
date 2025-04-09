#pragma once

#include "engine/runtime/global/base.h"
#include "component_base.h"

namespace rtr {

class Node_component : public Component_base {
protected:

public:
	Node_component() : Component_base(Component_type::NODE) {}

};


};
