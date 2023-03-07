#pragma once

#include "headers.hpp"
#include "ui.hpp"
#include "data.hpp"

namespace ui
{
  namespace update
  {
    int panel(float, float, float, entt::entity, entt::registry&, VkDevice, VmaAllocator);
    void tab(float, float, entt::entity, entt::entity, entt::registry&, VmaAllocator);
    void element(entt::entity, entt::entity, entt::entity, entt::registry&);
  }
}