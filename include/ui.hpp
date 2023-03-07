#pragma once

#include "vulkan_dynamic.hpp"
#include "headers.hpp"

namespace ui
{
  struct Button
  {
    float x, y;
    float width, height;
    bool locked;
    bool selected;
  };

  struct Color
  {
    std::array<float, 3> normal;
    std::array<float, 3> gray;
    std::array<float, 3> highlight;
  };

  struct Label
  {
    std::string text;
  };

  struct List_Select
  {

  };
  struct Input
  {

  };
  struct Slider
  {

  };
  
  struct Tab
  {
    float x, y;
    float width, height;

    bool selected;
    bool held;

    float vx, vy;
    bool aligning;
    bool seting;

    std::uint32_t index;
    std::vector<entt::entity> elements;
    entt::entity parent;
  };
  
  enum side_binding{ BOUND, FREE };
  enum screen_side{ LEFT, RIGHT, TOP, BOTTOM, NONE };
  enum panel_mode{ ACTIVE, PASIVE, MOTION };
  struct Panel
  {
    float x, y;
    float width, height;

    float vx, vy;

    side_binding binding;
    screen_side side;
    screen_side motion_direction;
    screen_side transition_side;
    panel_mode mode;
    panel_mode transition_target;

    std::uint32_t tab_count;
    std::uint32_t active_index;
    std::vector<entt::entity> tabs;
  };

  /* --- below the button functions will be found --- */
  vk::Buffer_Info create_buffer_info(VmaAllocator);
  vk::Draw_Info create_draw_info(VkDevice);

  std::array<float, 8> create_mesh(float, float, float, float);
  void fill_button_data(Button&, vk::Buffer_Info&, Color&, VmaAllocator&);
  void fill_panel_data(Panel&, vk::Buffer_Info&, std::array<float, 3>&, VmaAllocator&);
  void fill_tab_data(float, float, Panel&, Tab&, vk::Buffer_Info&, Color&, VmaAllocator&);
  
  void highlight_color(Color&, vk::Buffer_Info&, VmaAllocator&);
  void unhighlight_color(Color&, vk::Buffer_Info&, VmaAllocator&);
  void gray_color(Color&, vk::Buffer_Info&, VmaAllocator&);
  void ungray_color(Color&, vk::Buffer_Info&, VmaAllocator&);

  bool inside(float, float, float, float, float, float);

  std::array<float, 8> create_tab_mesh(float, float, Tab&, Panel&);
  void set_panel_to_passive(float&, entt::entity&, entt::registry&);
  void set_panel_to_active(float&, entt::entity&, entt::registry&);
  void set_tab_to_motion(float, float, float, bool, entt::entity&, entt::registry&, VkDevice, VmaAllocator);
  void combine_tab_with_panel(float, float, entt::entity&, entt::entity&, entt::entity&, entt::registry&, VkDevice, VmaAllocator);
}