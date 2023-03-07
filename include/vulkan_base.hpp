#pragma once

#include "headers.hpp"
#include "data.hpp"

namespace vk
{
  enum device_type{DISCRETE, INTEGRATED};
  VkPhysicalDevice select_device(device_type, VkInstance);

  VkInstance create_instance(SDL_Window*);
  VkDevice create_device(std::vector<VkDeviceQueueCreateInfo>, VkPhysicalDevice);

  VkDebugUtilsMessengerEXT create_messenger(VkInstance);
  void destroy_messenger(VkDebugUtilsMessengerEXT, VkInstance);

  VkSwapchainKHR create_swapchain(VkSurfaceKHR, Indecies, Surface_Meta, VkDevice);
  std::vector<VkImage> get_images(Surface_Meta*, VkSwapchainKHR, VkDevice);

  VkImage create_image_view(VkImage, VkDevice);
  std::vector<VkImageView> create_image_views(Surface_Meta, std::vector<VkImage>, VkDevice);

  VkCommandPool create_command_pool(std::uint32_t, VkDevice);
  VkCommandPool create_command_pool2(std::uint32_t, VkDevice);

  Synchs create_synchs(std::uint32_t, VkDevice);
}