#pragma once

#include "macros.hpp"
#include "headers.hpp"

namespace vk
{
  struct Draw_Info
  {
    VkPipelineLayout layout;
    VkPipeline pipeline;

    std::vector<VkDescriptorSetLayout> descriptor_set_layouts;

    VkDevice device;
  };

  void clean_draw_info(entt::registry&, entt::entity);

  struct Compute_Info
  {
    VkPipelineLayout layout;
    VkPipeline pipeline;
    VkCommandBuffer command;

    std::vector<VkDescriptorSetLayout> descriptor_set_layouts;

    VkCommandPool pool;
    VkDevice device;
  };

  void clean_compute_info(entt::registry&, entt::entity);

  struct Buffer_Info
  {
    std::vector<VkBuffer> buffers;
    std::vector<VmaAllocation> allocations;

    VmaAllocator allocator;
  };

  void clean_buffer_info(entt::registry&, entt::entity);

  struct Texture_Info
  {
    std::vector<VkImage> textures;
    std::vector<VkImageView> views;
    std::vector<VmaAllocation> memory;

    std::vector<VkDescriptorPool> pools;
    std::vector<VkDescriptorSet> compute_sets;
    std::vector<VkDescriptorSet> graphics_sets;

    VkSampler sampler;

    VkDevice device;
    VmaAllocator allocator;
  };

  void clean_texture_info(entt::registry&, entt::entity);
}