#pragma once

#include "headers.hpp"
#include "data.hpp"

#include "vulkan_dynamic.hpp"
#include "background.hpp"
#include "font.hpp"

enum state{ MAIN_MENU, CREATE_WORLD, DESIGN_LAB, START, LOAD, RUNNING };
class Simulation
{
  public:
    Simulation(std::uint32_t width, std::uint32_t height);
    ~Simulation();

    void poll_events();
    void update();
    void render();

    bool should_close();

    void print_ui();
  
  private:
    std::uint32_t m_width, m_height;
    std::string m_title;
    SDL_Window* m_window;

    float m_anisotropy;

    state m_state;

    bool m_resized;
    bool m_is_running;

    float m_mouse_x, m_mouse_y;

    VkInstance m_instance;
    VkPhysicalDevice m_physical_device;
    Indecies m_indecies;
    VkDebugUtilsMessengerEXT m_messenger;

    VkSurfaceKHR m_surface;

    VkDevice m_device;
    Queues m_queues;

    Surface_Meta m_meta;
    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_views;

    VkCommandPool m_compute_pool;
    VkCommandPool m_graphics_pool;
    VkCommandPool m_transfer_pool;

    std::vector<VkCommandBuffer> m_buffers;

    Synchs m_synch;
    std::uint32_t m_current_frame;
    std::uint32_t m_max_frames_in_flight;

    VmaAllocator m_allocator;
    entt::registry m_registry;

    std::chrono::duration<double> m_frame_time;
    std::chrono::time_point<std::chrono::system_clock> m_last_point, m_this_point;

    void set_main_menu();
    void set_create_world();
    void set_design_lab();
    void set_start();
    void set_load();
    void set_running();

    void resize();
    VkCommandBuffer record_command_buffer(std::uint32_t);

    void handel_main_menu(SDL_Event& event);
    void handel_create_world(SDL_Event& event);
    void handel_design_lab(SDL_Event& event);
    void handel_start(SDL_Event& event);
    void handel_load(SDL_Event& event);
    void handel_running(SDL_Event& event);
};