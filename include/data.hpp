#pragma once

#include "headers.hpp"

/* --- the global vulkan bundel data --- */
struct Indecies
{
  std::uint32_t compute;
  std::uint32_t present;
  std::uint32_t graphics;
  std::uint32_t transfer;
};

struct Queues
{
  VkQueue compute;
  VkQueue present;
  VkQueue graphics;
  VkQueue transfer;
};

struct Surface_Meta
{
  VkPresentModeKHR present_mode;
  VkSurfaceFormatKHR format;
  VkSurfaceCapabilitiesKHR capabilities;
  VkExtent2D extent;
  std::uint32_t image_count;
};

struct Synchs
{
  std::vector<VkFence> in_flight_fences;
  std::vector<VkSemaphore> render_finished_semaphores;
  std::vector<VkSemaphore> image_available_semaphores;
};

struct Click
{
  bool down;
  bool up;
  float down_x;
  float down_y;
  float up_x;
  float up_y;
};

struct Wheel
{
  std::uint32_t direction;
  float dx;
  float dy;
};

/* --- the structs for the ui elements of empire --- */
struct Ui{};
struct Quad{};
struct Button{};
struct Panel{};