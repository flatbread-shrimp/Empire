#include "../include/simulation.hpp"
#include "../include/ui.hpp"
#include "../include/vulkan_dynamic.hpp"

void Simulation::handel_main_menu(SDL_Event& event)
{
  auto ui_view = m_registry.view<ui::Button>();
  for(auto entity : ui_view)
  {
    auto button = m_registry.get<ui::Button>(entity);
    auto color = m_registry.get<ui::Color>(entity);
    auto buffers = m_registry.get<vk::Buffer_Info>(entity);
    auto label = m_registry.get<ui::Label>(entity);

    /* --- change the button colors --- */
    if(ui::inside(m_mouse_x, m_mouse_y, button.x, button.y, button.width, button.height) && !button.selected && !button.locked)
    {
      ui::highlight_color(color, buffers, m_allocator);
      m_registry.patch<ui::Button>(entity, [](auto &button){ button.selected = true; });
    }
    else if(!ui::inside(m_mouse_x, m_mouse_y, button.x, button.y, button.width, button.height) && button.selected && !button.locked)
    {
      ui::unhighlight_color(color, buffers, m_allocator);
      m_registry.patch<ui::Button>(entity, [](auto &button){ button.selected = false; });
    }
    
    /* --- handel the click event --- */
    if(ui::inside(m_mouse_x, m_mouse_y, button.x, button.y, button.width, button.height))
    {
      if(event.type == SDL_MOUSEBUTTONDOWN && label.text == "Quit")
      {
        m_is_running = false;
      }
      else if(event.type == SDL_MOUSEBUTTONUP && label.text == "Create World")
      {
        set_create_world();
      }
    }
  }
}

void Simulation::handel_create_world(SDL_Event& event)
{
  // hanel click haneling here
  auto click_view = m_registry.view<Click>();
  for(auto click_element : click_view)
  {
    auto click_struct = m_registry.get<Click>(click_element);

    switch(event.button.button)
    {
      case SDL_BUTTON_LEFT:
        if(click_struct.down)
        {
          auto panel_view = m_registry.view<Panel>();
          for(auto panel_element : panel_view)
          {
            auto& panel_struct = m_registry.get<ui::Panel>(panel_element);
            auto tabs = panel_struct.tabs;

            for(auto tab_element : tabs)
            {
              auto& tab_struct = m_registry.get<ui::Tab>(tab_element);
              auto elements = tab_struct.elements;

              if(ui::inside(click_struct.down_x, click_struct.down_y, tab_struct.x, tab_struct.y, tab_struct.width, tab_struct.height))
              {
                panel_struct.active_index = tab_struct.index;
                m_registry.patch<ui::Panel>(panel_element);

                auto color = m_registry.get<ui::Color>(tab_element);
                auto buffer_info = m_registry.get<vk::Buffer_Info>(panel_element);
                ui::unhighlight_color(color, buffer_info, m_allocator);
              }
            }
          }
        }
        break;
      case SDL_BUTTON_RIGHT:
        if(click_struct.down)
        {
          auto panel_view = m_registry.view<Panel>();
          for(auto panel_element : panel_view)
          {
            auto& panel_struct = m_registry.get<ui::Panel>(panel_element);
            auto tabs = panel_struct.tabs;

            for(auto tab_element : tabs)
            {
              auto& tab_struct = m_registry.get<ui::Tab>(tab_element);

              if(ui::inside(click_struct.down_x, click_struct.down_y, tab_struct.x, tab_struct.y, tab_struct.width, tab_struct.height))
              {
                tab_struct.held = true;
                m_registry.patch<ui::Tab>(tab_element);
              }
            }
          }
        }
        else
        {
          float dx = click_struct.up_x - click_struct.down_x;
          float dy = click_struct.up_y - click_struct.down_y;

          dx = 1600.0f * dx / ((float)m_width + dx);
          dy = 1600.0f * dy / ((float)m_height + dy);

          auto panel_view = m_registry.view<Panel>();
          for(auto panel_element : panel_view)
          {
            auto& panel_struct = m_registry.get<ui::Panel>(panel_element);
            auto tabs = panel_struct.tabs;

            for(auto tab_element : tabs)
            {
              auto& tab_struct = m_registry.get<ui::Tab>(tab_element);

              if(tab_struct.held)
              {
                switch(panel_struct.side)
                {
                  case ui::LEFT:
                    if(dx < 0.0f)
                    {
                      ui::set_panel_to_passive(dx, panel_element, m_registry);
                    }
                    else if(dx > 0.0f && panel_struct.mode == ui::PASIVE)
                    {
                      ui::set_panel_to_active(dx, panel_element, m_registry);
                    }
                    else
                    {
                      ui::set_tab_to_motion((float)m_width, (float)m_height, dx, true, tab_element, m_registry, m_device, m_allocator);
                    }
                    break;
                  case ui::RIGHT:
                    if(dx > 0.0f )
                    {
                      ui::set_panel_to_passive(dx, panel_element, m_registry);
                    }
                    else if(dx < 0.0f && panel_struct.mode == ui::PASIVE)
                    {
                      ui::set_panel_to_active(dx, panel_element, m_registry);
                    }
                    else
                    {
                      ui::set_tab_to_motion((float)m_width, (float)m_height, dx, true, tab_element, m_registry, m_device, m_allocator);
                    }
                    break;
                  case ui::TOP:
                    if(dy < 0.0f )
                    {
                      ui::set_panel_to_passive(dx, panel_element, m_registry);
                    }
                    else if(dy > 0.0f && panel_struct.mode == ui::PASIVE)
                    {
                      ui::set_panel_to_active(dx, panel_element, m_registry);
                    }
                    else
                    {
                      ui::set_tab_to_motion((float)m_width, (float)m_height, dy, false, tab_element, m_registry, m_device, m_allocator);
                    }
                    break;
                  case ui::BOTTOM:
                    if(dy > 0.0f )
                    {
                      ui::set_panel_to_passive(dx, panel_element, m_registry);
                    }
                    else if(dy < 0.0f && panel_struct.mode == ui::PASIVE)
                    {
                      ui::set_panel_to_active(dx, panel_element, m_registry);
                    }
                    else
                    {
                      ui::set_tab_to_motion((float)m_width, (float)m_height, dy, false, tab_element, m_registry, m_device, m_allocator);
                    }
                    break;
                  case ui::NONE:
                    break;
                }

                tab_struct.held = false;
                m_registry.patch<ui::Tab>(tab_element);
                
                break;
              }
            }
          }
        }
        break;
    }
  }

  auto wheel_view = m_registry.view<Wheel>();
  for(auto wheel_element : wheel_view)
  {
    
  }
}

void Simulation::handel_design_lab(SDL_Event& event)
{
  
}

void Simulation::handel_start(SDL_Event& event)
{

}

void Simulation::handel_load(SDL_Event& event)
{

}

void Simulation::handel_running(SDL_Event& event)
{

}