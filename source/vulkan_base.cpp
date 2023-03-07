#include "../include/vulkan_base.hpp"
#include "../include/macros.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
  if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
  {
    std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;
    std::cout << std::endl;
  }

  return VK_FALSE;
}

VkResult create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
{
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) 
  {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } 
  else 
  {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) 
{
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if(func != nullptr) 
  {
    func(instance, debugMessenger, pAllocator);
  }
}

namespace vk
{
  VkPhysicalDevice select_device(device_type type, VkInstance instance)
  {
    std::uint32_t device_count;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    VkPhysicalDevice found_device = VK_NULL_HANDLE;

    bool device_found = false;
    for(std::uint32_t i=0; i<devices.size(); i++)
    {
      VkPhysicalDeviceProperties device_props;
      vkGetPhysicalDeviceProperties(devices[i], &device_props);

      switch(device_props.deviceType)
      {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
          if(type == DISCRETE)
          {
            device_found = true;
            found_device = devices[i];
          }
          break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
          if(type == INTEGRATED)
          {
            device_found = true;
            found_device = devices[i];
          }
          break;
        default:
          continue;
      }

      if(device_found)
      {
        break;
      }
    }
    if(!device_found)
    {
      std::cout << "Select Device Faild!" << std::endl;
      throw;
    }

    return found_device;
  }

  VkInstance create_instance(SDL_Window* window)
  {
    VkApplicationInfo application_info = {};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pNext = nullptr;
    application_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instance_create_info = {};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = nullptr;
    instance_create_info.flags = 0;
    instance_create_info.pApplicationInfo = &application_info;

    #ifdef VK_DEBUG
      std::vector<const char*> layer_names = { "VK_LAYER_KHRONOS_validation" };

      instance_create_info.enabledLayerCount = 1;
      instance_create_info.ppEnabledLayerNames = layer_names.data();
    #else
      instance_create_info.enabledLayerCount = 0;
      instance_create_info.ppEnabledLayerNames = nullptr;
    #endif

    std::uint32_t extension_count;
    SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr);
    std::vector<const char*> extension_names(extension_count);
    SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extension_names.data());

    #ifdef VK_DEBUG
      extension_names.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
    #endif

    instance_create_info.enabledExtensionCount = extension_names.size();
    instance_create_info.ppEnabledExtensionNames = extension_names.data();

    VkInstance instance = VK_NULL_HANDLE;
    auto result = vkCreateInstance(&instance_create_info, nullptr, &instance);
    if(result != VK_SUCCESS)
    {
      std::cout << "The instance creation faild with error code: " << result << std::endl;
      throw;
    }

    return instance;
  }

  VkDevice create_device(std::vector<VkDeviceQueueCreateInfo> queue_infos, VkPhysicalDevice physical_device)
  {
    VkPhysicalDeviceFeatures features = {};
    features.fullDrawIndexUint32 = true;
    features.samplerAnisotropy = true;

    VkPhysicalDeviceVulkan13Features vulkan_features_13 = {};
    vulkan_features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    vulkan_features_13.pNext = nullptr;
    vulkan_features_13.computeFullSubgroups = true;
    vulkan_features_13.dynamicRendering = true;
    vulkan_features_13.maintenance4 = true;

    VkPhysicalDeviceFeatures2 features2 = {};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &vulkan_features_13;
    features2.features = features;

    std::vector<const char*> extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = &features2;
    device_create_info.flags = 0;
    device_create_info.queueCreateInfoCount = queue_infos.size();
    device_create_info.pQueueCreateInfos = queue_infos.data();
    device_create_info.pEnabledFeatures = nullptr;
    device_create_info.enabledExtensionCount = extensions.size();
    device_create_info.ppEnabledExtensionNames = extensions.data();
    device_create_info.enabledLayerCount = 0;
    device_create_info.ppEnabledLayerNames = nullptr;

    VkDevice device = VK_NULL_HANDLE;
    auto result = vkCreateDevice(physical_device, &device_create_info, nullptr, &device);
    if(result != VK_SUCCESS)
    {
      std::cout << "The device could not be created. Error code: " << result << std::endl;
    }

    return device;
  }

  VkDebugUtilsMessengerEXT create_messenger(VkInstance instance)
  {
    VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = {};
    messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    messenger_create_info.pNext = nullptr;
    messenger_create_info.flags = 0;
    messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    messenger_create_info.pfnUserCallback = debugCallback;
    messenger_create_info.pUserData = nullptr;

    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    auto result = create_debug_utils_messenger_EXT(instance, &messenger_create_info, nullptr, &messenger);
    if(result != VK_SUCCESS)
    {
      std::cout << "The messenger could not be created. Error code: " << result << std::endl;
      throw;
    }
    return messenger;
  }

  void destroy_messenger(VkDebugUtilsMessengerEXT messenger, VkInstance instance)
  {
    destroy_debug_utils_messenger_EXT(instance, messenger, nullptr);
  }

  VkSwapchainKHR create_swapchain(VkSurfaceKHR surface, Indecies indecies, Surface_Meta meta, VkDevice device)
  {
    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.pNext = nullptr;
    swapchain_create_info.flags = 0;
    swapchain_create_info.surface = surface;
    swapchain_create_info.minImageCount = meta.image_count;
    swapchain_create_info.imageFormat = meta.format.format;
    swapchain_create_info.imageColorSpace = meta.format.colorSpace;
    swapchain_create_info.imageExtent = meta.extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    std::uint32_t index_array[] = {indecies.present, indecies.graphics};
    if(indecies.present != indecies.graphics) 
    {
      swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      swapchain_create_info.queueFamilyIndexCount = 2;
      swapchain_create_info.pQueueFamilyIndices = index_array;
    } 
    else 
    {
      swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      swapchain_create_info.queueFamilyIndexCount = 0;
      swapchain_create_info.pQueueFamilyIndices = nullptr;
    }

    swapchain_create_info.preTransform = meta.capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = meta.present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    auto result = vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &swapchain);
    if(result != VK_SUCCESS)
    {
      std::cout << "The swapchain creation faild. Error code: " << result << std::endl;
    }

    return swapchain;
  }

  std::vector<VkImage> get_images(Surface_Meta* meta, VkSwapchainKHR swapchain, VkDevice device)
  {
    vkGetSwapchainImagesKHR(device, swapchain, &meta->image_count, nullptr);
    std::vector<VkImage> images(meta->image_count);
    vkGetSwapchainImagesKHR(device, swapchain, &meta->image_count, images.data());

    return images;
  }

  VkImage create_image_view(VkImage, VkDevice)
  {
    return VK_NULL_HANDLE;
  }

  std::vector<VkImageView> create_image_views(Surface_Meta meta, std::vector<VkImage> images, VkDevice device)
  {
    std::vector<VkImageView> views(images.size());
    for(std::uint32_t i=0; i<images.size(); i++)
    {
      VkImageViewCreateInfo view_create_info = {};
      view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      view_create_info.pNext = nullptr;
      view_create_info.flags = 0;
      view_create_info.image = images[i];
      view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
      view_create_info.format = meta.format.format;
      view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      view_create_info.subresourceRange.baseMipLevel = 0;
      view_create_info.subresourceRange.levelCount = 1;
      view_create_info.subresourceRange.baseArrayLayer = 0;
      view_create_info.subresourceRange.layerCount = 1;

      auto result = vkCreateImageView(device, &view_create_info, nullptr, &views[i]);
      if(result != VK_SUCCESS)
      {
        std::cout << "The image view could not be created. Error code: " << result << std::endl;
      }
    }

    return views;
  }

  VkCommandPool create_command_pool(std::uint32_t index, VkDevice device)
  {
    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.pNext = nullptr;
    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_create_info.queueFamilyIndex = index;

    VkCommandPool pool = VK_NULL_HANDLE;
    auto result = vkCreateCommandPool(device, &command_pool_create_info, nullptr, &pool);
    if(result != VK_SUCCESS)
    {
      std::cout << "The command pool could not be created. Error Code: " << result << std::endl;
      throw;
    }

    return pool;
  }

  VkCommandPool create_command_pool2(std::uint32_t index, VkDevice device)
  {
    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.pNext = nullptr;
    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    command_pool_create_info.queueFamilyIndex = index;

    VkCommandPool pool = VK_NULL_HANDLE;
    auto result = vkCreateCommandPool(device, &command_pool_create_info, nullptr, &pool);
    if(result != VK_SUCCESS)
    {
      std::cout << "The command pool could not be created. Error Code: " << result << std::endl;
      throw;
    }

    return pool;
  }

  Synchs create_synchs(std::uint32_t max_frames, VkDevice device)
  {
    Synchs synchs = {};
    synchs.image_available_semaphores.resize(max_frames);
    synchs.render_finished_semaphores.resize(max_frames);
    synchs.in_flight_fences.resize(max_frames);

    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = nullptr;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.pNext = nullptr;
    semaphore_create_info.flags = 0;

    for(std::uint32_t i=0; i<max_frames; i++)
    {
      vkCreateSemaphore(device, &semaphore_create_info, nullptr, &synchs.image_available_semaphores[i]);
      vkCreateSemaphore(device, &semaphore_create_info, nullptr, &synchs.render_finished_semaphores[i]);
      vkCreateFence(device, &fence_create_info, nullptr, &synchs.in_flight_fences[i]);
    }

    return synchs;
  }
}