#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "stb_image.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <cmath>
#include <functional>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <type_traits>
#include <filesystem>
#include <bitset>
#include <chrono>

#include "engine/runtime/global/event.h"
#include "engine/runtime/global/logger.h"
#include "engine/runtime/global/singleton.h"
#include "engine/runtime/global/math.h"
#include "engine/runtime/global/enum.h"
#include "engine/runtime/global/timer.h"



