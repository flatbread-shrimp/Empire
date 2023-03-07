#pragma once

#include "data.hpp"

namespace vk
{
  void find_queue_indecies(Indecies*, VkSurfaceKHR, VkPhysicalDevice);
  std::vector<VkDeviceQueueCreateInfo> create_queue_infos(Indecies&, float*);
  Queues get_device_queues(Indecies, VkDevice);
  Surface_Meta get_surface_meta(SDL_Window*, VkSurfaceKHR, VkPhysicalDevice);
}