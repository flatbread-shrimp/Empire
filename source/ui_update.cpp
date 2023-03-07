#include "../include/ui_update.hpp"

float check_x_bounds(ui::panel_mode mode, ui::screen_side target, ui::screen_side direction, float width, float panel_width, float x0, bool& check)
{
  float x = x0;
  if(mode == ui::PASIVE && target == ui::LEFT && direction == ui::LEFT)
  {
    if(x < -panel_width)
    {
      x = -panel_width;
      check = true;
    }
  }
  else if(mode == ui::ACTIVE && target == ui::LEFT && direction == ui::LEFT)
  {
    if(x < 0.0f)
    {
      x = 0.0f;
      check = true;
    }
  }
  else if(mode == ui::PASIVE && target == ui::RIGHT && direction == ui::LEFT)
  {
    std::cout << "This collection of inputs is not allowed. Error Code: 1" << std::endl;
    throw;
  }
  else if(mode == ui::ACTIVE && target == ui::RIGHT && direction == ui::LEFT)
  {
    if(x < width - panel_width)
    {
      x = width - panel_width;
      check = true;
    }
  }
  else if(mode == ui::PASIVE && target == ui::LEFT && direction == ui::RIGHT)
  {
    std::cout << "This collection of inputs is not allowed. Error Code: 2" << std::endl;
    throw;
  }
  else if(mode == ui::ACTIVE && target == ui::LEFT && direction == ui::RIGHT)
  {
    if(x > 0.0f)
    {
      x = 0.0f;
      check = true;
    }
  }
  else if(mode == ui::PASIVE && target == ui::RIGHT && direction == ui::RIGHT)
  {
    if(x > width)
    {
      x = width;
      check = true;
    }
  }
  else if(mode == ui::ACTIVE && target == ui::RIGHT && direction == ui::RIGHT)
  {
    if(x > width - panel_width)
    {
      x = width - panel_width;
      check = true;
    }
  }
  else
  {
    std::cout << "Invalid combination of inputs to check x bounds!" << std::endl;
    throw;
  }
  return x;
}
float check_y_bounds(ui::panel_mode mode, ui::screen_side target, ui::screen_side direction, float height, float panel_height, float y0, bool& check)
{
  float y = y0;
  return y;
}
void detect_merge(entt::entity* partner, bool* decteded, entt::entity panel, entt::registry& registry)
{
  auto panel_struct = registry.get<ui::Panel>(panel);
  ui::screen_side motion_target = panel_struct.transition_side;
  ui::screen_side motion_direction = panel_struct.motion_direction;

  for(auto test_panel : registry.view<Panel>())
  {
    auto test_struct = registry.get<ui::Panel>(test_panel);
    if(test_struct.side == motion_target && test_struct.side == motion_direction && test_panel != panel)
    {
      *decteded = true;
      *partner = test_panel;
      break;
    }
  }
};

int ui::update::panel(float dt, float width, float height, entt::entity panel_element, entt::registry& registry, VkDevice device, VmaAllocator allocator)
{
  auto& panel_struct = registry.get<ui::Panel>(panel_element);
  if(panel_struct.mode == MOTION)
  {
    float x, y;
    x = panel_struct.x + dt * panel_struct.vx;
    y = panel_struct.y + dt * panel_struct.vy;

    bool faild_x_check = false;
    bool faild_y_check = false;
    x = check_x_bounds(panel_struct.transition_target, panel_struct.transition_side, panel_struct.motion_direction, width, panel_struct.width, x, faild_x_check);
    y = check_y_bounds(panel_struct.transition_target, panel_struct.transition_side, panel_struct.motion_direction, height, panel_struct.height, y, faild_y_check);

    bool merge_detected = false;
    entt::entity other_panel;
    if(faild_x_check || faild_y_check)
    {
      detect_merge(&other_panel, &merge_detected, panel_element, registry);
    }

    if(faild_x_check)
    {
      switch(panel_struct.transition_side)
      {
        case ui::LEFT:
          if(!merge_detected)
          {
            panel_struct.motion_direction = ui::NONE;
            panel_struct.transition_side = ui::NONE;
            panel_struct.vx = 0.0f;
            panel_struct.vy = 0.0f;
            panel_struct.mode = panel_struct.transition_target;
            panel_struct.side = ui::LEFT;
          }
          else
          {
            ui::combine_tab_with_panel(width, height, panel_struct.tabs[0], panel_element, other_panel, registry, device, allocator);
            return 1;
          }
          break;
        case ui::RIGHT:
          if(!merge_detected)
          {
            panel_struct.motion_direction = ui::NONE;
            panel_struct.transition_side = ui::NONE;
            panel_struct.vx = 0.0f;
            panel_struct.vy = 0.0f;
            panel_struct.mode = panel_struct.transition_target;
            panel_struct.side = ui::RIGHT;
          }
          else
          { 
            ui::combine_tab_with_panel(width, height, panel_struct.tabs[0], panel_element, other_panel, registry, device, allocator);
            return 1;
          }
          break;
        default:
          std::cout << "ERROR: Unknown transition side. Shutting down." << std::endl;
          break;
      }
    }
    if(faild_y_check)
    {
      switch(panel_struct.transition_side)
      {
        case ui::TOP:
          panel_struct;
          panel_struct;
          panel_struct;
          panel_struct;
          panel_struct;
          panel_struct;
          break;
        case ui::BOTTOM:
          panel_struct;
          panel_struct;
          panel_struct;
          panel_struct;
          panel_struct;
          panel_struct;
          break;
        default:
          std::cout << "ERROR: Unknown transition side. Shutting down." << std::endl;
          break;
      }
    }

    panel_struct.x = x;
    panel_struct.y = y;
    registry.patch<ui::Panel>(panel_element);

    auto buffer_info = registry.get<vk::Buffer_Info>(panel_element);
    auto color = registry.get<ui::Color>(panel_struct.tabs[panel_struct.active_index]);

    ui::fill_panel_data(panel_struct, buffer_info, color.normal, allocator);
  }
  return 0;
}

void ui::update::tab(float width, float height, entt::entity tab_element, entt::entity panel_element, entt::registry& registry, VmaAllocator allocator)
{
  auto& panel_struct = registry.get<ui::Panel>(panel_element);
  auto& tab_struct = registry.get<ui::Tab>(tab_element);

  if(panel_struct.mode == MOTION)
  {
    auto mesh = ui::create_tab_mesh(width, height, tab_struct, panel_struct);
    tab_struct.x = mesh[0];
    tab_struct.y = mesh[1];

    registry.patch<ui::Tab>(tab_element);

    auto buffer_info = registry.get<vk::Buffer_Info>(tab_element);
    auto color = registry.get<Color>(tab_element);
    ui::fill_tab_data(width, height, panel_struct, tab_struct, buffer_info, color, allocator);
  }
}

void ui::update::element(entt::entity element, entt::entity tab, entt::entity panel, entt::registry& registry)
{

}