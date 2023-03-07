#include "../include/simulation.hpp"
#include "../include/vulkan_base.hpp"
#include "../include/vulkan_help.hpp"
#include "../include/macros.hpp"
#include "../include/ui.hpp"
#include "../include/ui_update.hpp"

Simulation::Simulation(std::uint32_t width, std::uint32_t height)
{
  m_width = width; m_height = height;
  m_title = "Emprie";

  m_resized = false;
  m_is_running = true;

  m_max_frames_in_flight = 3;
  m_current_frame = 0;

  SDL_Init(SDL_INIT_EVERYTHING);

  Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN;
  m_window = SDL_CreateWindow(m_title.c_str(), undefined, undefined, m_width, m_height, flags);

  m_instance = vk::create_instance(m_window);
  m_physical_device = vk::select_device(vk::DISCRETE, m_instance);

  VkPhysicalDeviceProperties device_props;
  vkGetPhysicalDeviceProperties(m_physical_device, &device_props);
  m_anisotropy = device_props.limits.maxSamplerAnisotropy;

  #ifdef VK_DEBUG
    m_messenger = vk::create_messenger(m_instance);
  #endif

  SDL_Vulkan_CreateSurface(m_window, m_instance, &m_surface);

  vk::find_queue_indecies(&m_indecies, m_surface, m_physical_device);

  float priority = 1.0f;
  auto queue_infos = vk::create_queue_infos(m_indecies, &priority);
  m_device = vk::create_device(queue_infos, m_physical_device);
  m_queues = vk::get_device_queues(m_indecies, m_device);

  m_meta = vk::get_surface_meta(m_window, m_surface, m_physical_device);
  m_swapchain = vk::create_swapchain(m_surface, m_indecies, m_meta, m_device);
  m_images = vk::get_images(&m_meta, m_swapchain, m_device);
  m_views = vk::create_image_views(m_meta, m_images, m_device);

  VmaAllocatorCreateInfo allocator_create_info = {};
  allocator_create_info.instance = m_instance;
  allocator_create_info.physicalDevice = m_physical_device;
  allocator_create_info.device = m_device;

  auto result = vmaCreateAllocator(&allocator_create_info, &m_allocator);
  if(result != VK_SUCCESS)
  {
    std::cout << "The allocator creation faild. Error Code: " << result << std::endl;
    throw;
  }

  m_compute_pool = vk::create_command_pool(m_indecies.compute, m_device);
  m_graphics_pool = vk::create_command_pool(m_indecies.graphics, m_device);
  m_transfer_pool = vk::create_command_pool2(m_indecies.transfer, m_device);

  m_buffers.resize(m_meta.image_count);

  VkCommandBufferAllocateInfo allocate_info = {};
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.pNext = nullptr;
  allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocate_info.commandPool = m_graphics_pool;
  allocate_info.commandBufferCount = m_buffers.size();

  result = vkAllocateCommandBuffers(m_device, &allocate_info, m_buffers.data());
  if(result != VK_SUCCESS)
  {
    std::cout << "The allocation faild for the graphics buffers. Error Code: " << result << std::endl;
    throw;
  }

  m_synch = vk::create_synchs(m_max_frames_in_flight, m_device);

  m_registry = entt::registry();
  m_registry.on_destroy<vk::Draw_Info>().connect<vk::clean_draw_info>();
  m_registry.on_destroy<vk::Compute_Info>().connect<vk::clean_compute_info>();
  m_registry.on_destroy<vk::Buffer_Info>().connect<vk::clean_buffer_info>();
  m_registry.on_destroy<vk::Texture_Info>().connect<vk::clean_texture_info>();

  set_main_menu();

  TTF_Init();

  m_last_point = std::chrono::system_clock::now();
}
Simulation::~Simulation()
{
  vkDeviceWaitIdle(m_device);

  TTF_Quit();

  m_registry.clear();
  m_registry.on_destroy<vk::Draw_Info>().disconnect<vk::clean_draw_info>();
  m_registry.on_destroy<vk::Compute_Info>().disconnect<vk::clean_compute_info>();
  m_registry.on_destroy<vk::Buffer_Info>().disconnect<vk::clean_buffer_info>();
  m_registry.on_destroy<vk::Texture_Info>().disconnect<vk::clean_texture_info>();

  for(std::uint32_t i=0; i<m_max_frames_in_flight; i++)
  {
    vkDestroySemaphore(m_device, m_synch.image_available_semaphores[i], nullptr);
    vkDestroySemaphore(m_device, m_synch.render_finished_semaphores[i], nullptr);
    vkDestroyFence(m_device, m_synch.in_flight_fences[i], nullptr);
  }

  vkDestroyCommandPool(m_device, m_compute_pool, nullptr);
  vkDestroyCommandPool(m_device, m_graphics_pool, nullptr);
  vkDestroyCommandPool(m_device, m_transfer_pool, nullptr);

  vmaDestroyAllocator(m_allocator);

  for(auto view : m_views)
  {
    vkDestroyImageView(m_device, view, nullptr);
  }

  vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
  vkDestroyDevice(m_device, nullptr);

  vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

  #ifdef VK_DEBUG
    vk::destroy_messenger(m_messenger, m_instance);
  #endif

  vkDestroyInstance(m_instance, nullptr);
  SDL_Quit();
}

void Simulation::poll_events()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT) m_is_running = false;
    if(event.type == SDL_WINDOWEVENT_RESIZED) m_resized = true;

    int x, y;
    SDL_GetMouseState(&x, &y);
    m_mouse_x = (float)x;
    m_mouse_y = (float)y;

    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
      auto click = m_registry.create();

      Click click_struct = {};
      click_struct.down = true;
      click_struct.up = false;
      click_struct.down_x = m_mouse_x;
      click_struct.down_y = m_mouse_y;

      m_registry.emplace<Click>(click, click_struct);
    }
    else if(event.type == SDL_MOUSEBUTTONUP)
    {
      auto click_view = m_registry.view<Click>();
      for(auto click_element : click_view)
      {
        auto& click_struct = m_registry.get<Click>(click_element);

        click_struct.up = true;
        click_struct.down = false;
        click_struct.up_x = m_mouse_x;
        click_struct.up_y = m_mouse_y;

        m_registry.patch<Click>(click_element);
      }
    }

    if(event.type == SDL_MOUSEWHEEL)
    {
      auto wheel = m_registry.create();

      Wheel wheel_struct = {};
      wheel_struct.direction = event.wheel.direction;
      wheel_struct.dx = (float)event.wheel.x;
      wheel_struct.dy = (float)event.wheel.y;

      m_registry.emplace<Wheel>(wheel, wheel_struct);
    }

    switch(m_state)
    {
      case MAIN_MENU:
        handel_main_menu( event );
        break;
      case CREATE_WORLD:
        handel_create_world( event );
        break;
      case DESIGN_LAB:
        handel_design_lab( event );
        break;
      case START:
        handel_start( event );
        break;
      case LOAD:
        handel_load( event );
        break;
      case RUNNING:
        handel_running( event );
        break;
    }

    if(event.type == SDL_MOUSEBUTTONUP)
    {
      auto click_view = m_registry.view<Click>();
      for(auto click_element : click_view)
      {
        m_registry.destroy(click_element);
      }
    }
    if(event.type == SDL_MOUSEWHEEL)
    {
      auto wheel_view = m_registry.view<Wheel>();
      for(auto wheel_element : wheel_view)
      {
        m_registry.destroy(wheel_element);
      }
    }
  }
}

void Simulation::update()
{
  m_this_point = std::chrono::system_clock::now();
  m_frame_time = m_this_point - m_last_point;
  
  double elapsed_time = m_frame_time.count();
  float dt = (float)elapsed_time;
  
  // update the simulations ui
  auto panel_view = m_registry.view<Panel>();
  for(auto panel_element : panel_view)
  {
    int panel_res = ui::update::panel(dt, (float)m_width, (float)m_height, panel_element, m_registry, m_device, m_allocator);

    if(panel_res == 0)
    {
      auto& panel_struct = m_registry.get<ui::Panel>(panel_element);
      for(auto tab_element : panel_struct.tabs)
      {
        ui::update::tab((float)m_width, (float)m_height, tab_element, panel_element, m_registry, m_allocator);

        auto& tab_struct = m_registry.get<ui::Tab>(tab_element);
        for(auto element : tab_struct.elements)
        {
          ui::update::element(element, tab_element, panel_element, m_registry);
        }
      }
    }
  }

  // update the simulations body
}

void Simulation::render()
{
  /* --- start the rendering pass and get the image index --- */
  vkWaitForFences(m_device, 1, &m_synch.in_flight_fences[m_current_frame], VK_TRUE, UINT64_MAX);

  uint32_t image_index;
  auto result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_synch.image_available_semaphores[m_current_frame], VK_NULL_HANDLE, &image_index);
  if(result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    resize();
    return;
  } 
  else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
  {
    std::cout << "The swapchain image could not be gotten. Error code: " << result << std::endl; 
    throw;
  }

  vkResetFences(m_device, 1, &m_synch.in_flight_fences[m_current_frame]);
  auto command = record_command_buffer( image_index );

  VkPipelineStageFlags ui_wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  VkSubmitInfo ui_submit_info = {};
  ui_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  ui_submit_info.pNext = nullptr;
  ui_submit_info.pWaitDstStageMask = ui_wait_stages;
  ui_submit_info.waitSemaphoreCount = 1;
  ui_submit_info.pWaitSemaphores = &m_synch.image_available_semaphores[m_current_frame];
  ui_submit_info.signalSemaphoreCount = 1;
  ui_submit_info.pSignalSemaphores = &m_synch.render_finished_semaphores[m_current_frame];
  ui_submit_info.commandBufferCount = 1;
  ui_submit_info.pCommandBuffers = &command;

  result = vkQueueSubmit(m_queues.graphics, 1, &ui_submit_info, m_synch.in_flight_fences[m_current_frame]);
  if(result != VK_SUCCESS) {
    std::cout << "Failed the queue sumition. Error Code: " << result << std::endl;
    throw;
  }

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pNext = nullptr;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &m_synch.render_finished_semaphores[m_current_frame];
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &m_swapchain;
  present_info.pImageIndices = &image_index;

  result = vkQueuePresentKHR(m_queues.present, &present_info);
  if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
  {
    resize();
  } 
  else if (result != VK_SUCCESS) 
  {
    std::cout << "The presentation faild. Error code: " << result << std::endl;
    throw;
  }

  /* --- advance the frame to the next frame --- */
  m_current_frame = (m_current_frame + 1) % m_max_frames_in_flight;
  m_last_point = m_this_point;
}

bool Simulation::should_close()
{
  return !m_is_running;
}

void Simulation::print_ui()
{
  std::cout << std::endl;

  std::cout << "Simulation UI Header: " << std::endl;
  std::cout << "Panel Count:  " << m_registry.view<Panel>().size() << std::endl;
  std::cout << "Button Count: " << m_registry.view<Button>().size() << std::endl;

  std::cout << std::endl;
  uint32_t count = 0;
  for(auto panel : m_registry.view<Panel>())
  {
    auto panel_data = m_registry.get<ui::Panel>(panel);
    auto tabs = panel_data.tabs;

    std::cout << "Panel " << count << " Header: " << std::endl;
    std::cout << "Tab count: " << tabs.size() << std::endl;
    switch(panel_data.side)
    {
      case ui::LEFT:
        std::cout << "Panel Side: Left" << std::endl;
        break;
      case ui::RIGHT:
        std::cout << "Panel Side: Right" << std::endl;
        break;
      case ui::TOP:
        std::cout << "Panel Side: Top" << std::endl;
        break;
      case ui::BOTTOM:
        std::cout << "Panel Side: Bottom" << std::endl;
        break;
    }
    for(auto tab : tabs)
    {
      auto tab_data = m_registry.get<ui::Tab>(tab);
      std::cout << "This tab is at index: " << tab_data.index << std::endl;
    }

    std::cout << std::endl;
    count += 1;
  }

  std::cout << std::endl;
}