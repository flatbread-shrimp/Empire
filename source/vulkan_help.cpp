#include "../include/vulkan_help.hpp"

std::uint32_t find_min_index(std::vector<int> scores)
{
  std::uint32_t index = 0;
  int min = scores[index];
  for(std::uint32_t i=1; i<scores.size(); i++) 
  {
    if(min < scores[i])
    {
      min = scores[i];
      index = i;
    }
  }
  return index;
}

namespace vk
{
  void find_queue_indecies(Indecies* indecies, VkSurfaceKHR surface, VkPhysicalDevice device)
  {
    std::uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_family_props(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_props.data());

    std::vector<int> compute_scores(queue_family_count);
    std::vector<int> present_scores(queue_family_count);
    std::vector<int> graphics_scores(queue_family_count);
    std::vector<int> transfer_scores(queue_family_count);

    bool combined_graphics = false;
    for(std::uint32_t i=0; i<queue_family_count; i++)
    {
      auto props = queue_family_props[i];

      VkBool32 present_check;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_check);

      bool compute_check = props.queueFlags & VK_QUEUE_COMPUTE_BIT;
      bool graphics_check = props.queueFlags & VK_QUEUE_GRAPHICS_BIT;
      bool transfer_check = props.queueFlags & VK_QUEUE_TRANSFER_BIT;

      if(present_check && graphics_check && !combined_graphics)
      {
        combined_graphics = true;
        indecies->present = i;
        indecies->graphics = i;
      }

      int score = 0;
      if(compute_check)
      {
        score += 1;
      }
      if(present_check)
      {
        score += 1;
      }
      if(graphics_check)
      {
        score += 1;
      }
      if(transfer_check)
      {
        score += 1;
      }

      if(compute_check)
      {
        compute_scores[i] = score;
      }
      else
      { 
        compute_scores[i] = INT32_MAX;
      }

      if(present_check)
      {
        present_scores[i] = score;
      }
      else 
      {
        present_scores[i] = INT32_MAX;
      }

      if(graphics_check)
      {
        graphics_scores[i] = score;
      }
      else
      {
        graphics_scores[i] = INT32_MAX;
      }

      if(transfer_check)
      { 
        transfer_scores[i] = score;
      }
      else
      {
        transfer_scores[i] = INT32_MAX;
      }
    }

    indecies->compute = find_min_index(compute_scores);
    if(!combined_graphics)
    {
      indecies->present = find_min_index(present_scores);
      indecies->graphics = find_min_index(graphics_scores);
    }
    indecies->transfer = find_min_index(transfer_scores);
  }

  std::vector<VkDeviceQueueCreateInfo> create_queue_infos(Indecies& indecies, float* priorities)
  {
    std::vector<VkDeviceQueueCreateInfo> queue_infos;
    if(indecies.compute == indecies.present && indecies.present == indecies.graphics && indecies.graphics == indecies.transfer)
    {
      queue_infos.resize(1);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;
    }
    else if(indecies.compute == indecies.present && indecies.present == indecies.graphics)
    {
      queue_infos.resize(2);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.transfer;
      queue_infos[1].pQueuePriorities = priorities;
    }
    else if(indecies.compute == indecies.present && indecies.present == indecies.transfer)
    {
      queue_infos.resize(2);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.graphics;
      queue_infos[1].pQueuePriorities = priorities;
    }
    else if(indecies.compute == indecies.graphics && indecies.graphics == indecies.transfer)
    {
      queue_infos.resize(2);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.present;
      queue_infos[1].pQueuePriorities = priorities;
    }
    else if(indecies.present == indecies.graphics && indecies.graphics == indecies.transfer)
    {
      queue_infos.resize(2);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.transfer;
      queue_infos[1].pQueuePriorities = priorities;
    }
    else if(indecies.compute == indecies.present && indecies.graphics == indecies.transfer)
    {
      queue_infos.resize(2);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.transfer;
      queue_infos[1].pQueuePriorities = priorities;
    }
    else if(indecies.compute == indecies.transfer && indecies.present == indecies.graphics)
    {
      queue_infos.resize(2);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.present;
      queue_infos[1].pQueuePriorities = priorities;
    }
    else if(indecies.compute == indecies.graphics && indecies.present == indecies.transfer)
    {
      queue_infos.resize(2);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.transfer;
      queue_infos[1].pQueuePriorities = priorities;
    }
    else if(indecies.compute == indecies.present)
    {
      queue_infos.resize(3);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.graphics;
      queue_infos[1].pQueuePriorities = priorities;

      queue_infos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[2].pNext = nullptr;
      queue_infos[2].flags = 0;
      queue_infos[2].queueCount = 1;
      queue_infos[2].queueFamilyIndex = indecies.transfer;
      queue_infos[2].pQueuePriorities = priorities;
    }
    else if(indecies.compute == indecies.graphics)
    {
      queue_infos.resize(3);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.present;
      queue_infos[1].pQueuePriorities = priorities;

      queue_infos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[2].pNext = nullptr;
      queue_infos[2].flags = 0;
      queue_infos[2].queueCount = 1;
      queue_infos[2].queueFamilyIndex = indecies.transfer;
      queue_infos[2].pQueuePriorities = priorities;
    }
    else if(indecies.compute == indecies.transfer)
    {
      queue_infos.resize(3);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.present;
      queue_infos[1].pQueuePriorities = priorities;

      queue_infos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[2].pNext = nullptr;
      queue_infos[2].flags = 0;
      queue_infos[2].queueCount = 1;
      queue_infos[2].queueFamilyIndex = indecies.graphics;
      queue_infos[2].pQueuePriorities = priorities;
    }
    else if(indecies.present == indecies.graphics)
    {
      queue_infos.resize(3);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.graphics;
      queue_infos[1].pQueuePriorities = priorities;

      queue_infos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[2].pNext = nullptr;
      queue_infos[2].flags = 0;
      queue_infos[2].queueCount = 1;
      queue_infos[2].queueFamilyIndex = indecies.transfer;
      queue_infos[2].pQueuePriorities = priorities;
    } 
    else if(indecies.present == indecies.transfer)
    {
      queue_infos.resize(3);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.graphics;
      queue_infos[1].pQueuePriorities = priorities;

      queue_infos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[2].pNext = nullptr;
      queue_infos[2].flags = 0;
      queue_infos[2].queueCount = 1;
      queue_infos[2].queueFamilyIndex = indecies.transfer;
      queue_infos[2].pQueuePriorities = priorities;
    }
    else if(indecies.graphics == indecies.transfer)
    {
      queue_infos.resize(3);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.present;
      queue_infos[1].pQueuePriorities = priorities;

      queue_infos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[2].pNext = nullptr;
      queue_infos[2].flags = 0;
      queue_infos[2].queueCount = 1;
      queue_infos[2].queueFamilyIndex = indecies.transfer;
      queue_infos[2].pQueuePriorities = priorities;
    }
    else
    {
      queue_infos.resize(4);

      queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[0].pNext = nullptr;
      queue_infos[0].flags = 0;
      queue_infos[0].queueCount = 1;
      queue_infos[0].queueFamilyIndex = indecies.compute;
      queue_infos[0].pQueuePriorities = priorities;

      queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[1].pNext = nullptr;
      queue_infos[1].flags = 0;
      queue_infos[1].queueCount = 1;
      queue_infos[1].queueFamilyIndex = indecies.present;
      queue_infos[1].pQueuePriorities = priorities;

      queue_infos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[2].pNext = nullptr;
      queue_infos[2].flags = 0;
      queue_infos[2].queueCount = 1;
      queue_infos[2].queueFamilyIndex = indecies.graphics;
      queue_infos[2].pQueuePriorities = priorities;

      queue_infos[3].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_infos[3].pNext = nullptr;
      queue_infos[3].flags = 0;
      queue_infos[3].queueCount = 1;
      queue_infos[3].queueFamilyIndex = indecies.transfer;
      queue_infos[3].pQueuePriorities = priorities;
    }

    return queue_infos;
  }

  Queues get_device_queues(Indecies indecies, VkDevice device)
  {
    Queues queues = {};

    vkGetDeviceQueue(device, indecies.compute, 0, &queues.compute);
    vkGetDeviceQueue(device, indecies.present, 0, &queues.present);
    vkGetDeviceQueue(device, indecies.graphics, 0, &queues.graphics);
    vkGetDeviceQueue(device, indecies.transfer, 0, &queues.transfer);

    return  queues;
  }

  Surface_Meta get_surface_meta(SDL_Window* window, VkSurfaceKHR surface, VkPhysicalDevice device)
  {
    Surface_Meta meta = {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &meta.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, formats.data());

    bool format_found = false;
    for(std::uint32_t i=0; i<format_count; i++)
    {
      bool format_check = formats[i].format == VK_FORMAT_B8G8R8A8_SRGB;
      bool color_space_check = formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

      if(format_check && color_space_check)
      {
        meta.format = formats[i];
        format_found = true;
        break;
      }
    }
    if(!format_found)
    {
      meta.format = formats[0];
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, present_modes.data());

    bool present_mode_found = false;
    for(std::uint32_t i=0; i<present_mode_count; i++)
    {
      bool present_mode_check = present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR;

      if(present_mode_check)
      {
        meta.present_mode = present_modes[i];
        present_mode_found = true;
        break;
      }
    }
    if(!present_mode_found)
    {
      meta.present_mode = VK_PRESENT_MODE_FIFO_KHR;
    }

    if(meta.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
    {
      meta.extent = meta.capabilities.currentExtent;
    } 
    else 
    {
      int width, height;
      SDL_GetWindowSize(window, &width, &height);

      VkExtent2D actual_extent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
      };

      actual_extent.width = std::clamp(actual_extent.width, meta.capabilities.minImageExtent.width, meta.capabilities.maxImageExtent.width);
      actual_extent.height = std::clamp(actual_extent.height, meta.capabilities.minImageExtent.height, meta.capabilities.maxImageExtent.height);

      meta.extent = actual_extent;
    }

    meta.image_count = meta.capabilities.minImageCount + 1;
    if(meta.capabilities.maxImageCount > 0 && meta.image_count > meta.capabilities.maxImageCount) 
    {
      meta.image_count = meta.capabilities.maxImageCount;
    }

    return meta;
  }
}