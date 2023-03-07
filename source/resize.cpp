#include "../include/simulation.hpp"
#include "../include/vulkan_base.hpp"
#include "../include/vulkan_help.hpp"

void Simulation::resize()
{
  switch(m_state)
  {
    case MAIN_MENU:
      break;
    case CREATE_WORLD:
      break;
    case DESIGN_LAB:
      break;
    case START:
      break;
    case LOAD:
      break;
    case RUNNING:
      break;
  }

  vkDeviceWaitIdle(m_device);

  int width, height;
  SDL_GetWindowSize(m_window, &width, &height);
  m_width = (std::uint32_t)width;
  m_height = (std::uint32_t)height;

  m_registry.clear();
  for(auto view : m_views)
  {
    vkDestroyImageView(m_device, view, nullptr);
  }
  vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

  m_meta = vk::get_surface_meta(m_window, m_surface, m_physical_device);
  m_swapchain = vk::create_swapchain(m_surface, m_indecies, m_meta, m_device);
  m_images = vk::get_images(&m_meta, m_swapchain, m_device);
  m_views = vk::create_image_views(m_meta, m_images, m_device);

  switch(m_state)
  {
    case MAIN_MENU:
      set_main_menu();
      break;
    case CREATE_WORLD:
      set_create_world();
      break;
    case DESIGN_LAB:
      break;
    case START:
      break;
    case LOAD:
      break;
    case RUNNING:
      break;
  }
}