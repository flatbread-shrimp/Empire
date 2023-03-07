#pragma once

/* --- the standerd template library headers that the program needs --- */
#include <iostream>
#include <vector>
#include <array>
#include <functional>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <cinttypes>
#include <chrono>
#include <sstream>
#include <limits>

/* --- graphics libraries --- */
#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "../3dparty/sdl2/include/SDL_ttf.h"
#undef main

/* --- sql engine --- */
#include "../3dparty/sql-light/sqlite3.h"

/* --- entity component system --- */
#include "../3dparty/entt/entt.hpp"