#pragma once

#include "vulkan_dynamic.hpp"

namespace background
{
  vk::Draw_Info create_quad_draw_info(std::uint32_t, VkDevice);
  vk::Texture_Info create_texture_info(std::uint32_t, std::uint32_t, std::uint32_t, VkQueue, VkCommandPool, VkDevice, VmaAllocator, vk::Draw_Info&, vk::Compute_Info&);
  vk::Compute_Info create_texture_compute_info(std::uint32_t, VkDevice);

  void transfer_image_between_layouts(VkImageLayout, VkImageLayout, std::uint32_t, vk::Texture_Info&, VkQueue, VkCommandPool, VkDevice);

  void record_quad_command_buffer(std::uint32_t, std::uint32_t, VkImage, VkImageView, VkPipeline, VkCommandBuffer);
}