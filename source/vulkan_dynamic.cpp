#include "../include/vulkan_dynamic.hpp"

namespace vk
{
  void clean_draw_info(entt::registry& registry, entt::entity entity)
  {
    auto info = registry.get<Draw_Info>(entity);

    vkDestroyPipelineLayout(info.device, info.layout, nullptr);
    vkDestroyPipeline(info.device, info.pipeline, nullptr);

    for(std::uint32_t i=0; i<info.descriptor_set_layouts.size(); i++)
    {
      vkDestroyDescriptorSetLayout(info.device, info.descriptor_set_layouts[i], nullptr);
    }
  }

  void clean_compute_info(entt::registry& registry, entt::entity entity)
  {
    auto info = registry.get<Compute_Info>(entity);

    vkDestroyPipelineLayout(info.device, info.layout, nullptr);
    vkDestroyPipeline(info.device, info.pipeline, nullptr);
    
    for(std::uint32_t i=0; i<info.descriptor_set_layouts.size(); i++)
    {
      vkDestroyDescriptorSetLayout(info.device, info.descriptor_set_layouts[i], nullptr);
    }
  }

  void clean_buffer_info(entt::registry& registry, entt::entity entity)
  {
    auto info = registry.get<Buffer_Info>(entity);
    for(std::uint32_t i=0; i<info.buffers.size(); i++)
    {
      vmaDestroyBuffer(info.allocator, info.buffers[i], info.allocations[i]);
    }
  }

  void clean_texture_info(entt::registry& registry, entt::entity entity)
  {
    auto info = registry.get<Texture_Info>(entity);

    for(std::uint32_t i=0; i<info.memory.size(); i++)
    {
      vmaDestroyImage(info.allocator, info.textures[i], info.memory[i]);
      vkDestroyImageView(info.device, info.views[i], nullptr);
    }

    for(std::uint32_t i=0; i<info.pools.size(); i++)
    {
      vkDestroyDescriptorPool(info.device, info.pools[i], nullptr);
    }

    vkDestroySampler(info.device, info.sampler, nullptr);
  }
}