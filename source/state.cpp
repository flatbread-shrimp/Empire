#include "../include/simulation.hpp"
#include "../include/ui.hpp"
#include "../include/background.hpp"

void Simulation::set_main_menu()
{
  vkDeviceWaitIdle(m_device);
  m_registry.clear();
  m_state = MAIN_MENU;

  Ui ui = {};
  Quad quad = {};
  Button button = {};

  ui::Color button_color = {};
  button_color.normal = { 8.0f/255.0f, 2.0f/255.0f, 189.0f/255.0f };
  button_color.gray = { 79.0f/255.0f, 79.0f/255.0f, 79.0f/255.0f };
  button_color.highlight = { 40.0f/255.0f, 35.0f/255.0f, 186.0f/255.0f };

  float width = (float)m_width;
  float height = (float)m_height;
  float button_count = 5.0f;

  float button_width = 0.25f*width;
  float button_height = 0.1f*height;
  float outer_gap = 0.15f*height;
  float inner_gap = (height - button_count*button_height - 2*outer_gap)/4.0f;

  float button_x = width/2.0f - button_width/2.0f;
  float button_y = outer_gap;

  auto background = m_registry.create();
  vk::Draw_Info background_draw_info = background::create_quad_draw_info(m_images.size(), m_device);
  vk::Compute_Info background_compute_info = background::create_texture_compute_info(m_images.size(), m_device);
  vk::Texture_Info background_texture_info = background::create_texture_info(m_images.size(), m_width, m_height, m_queues.graphics, m_graphics_pool, m_device, m_allocator, background_draw_info, background_compute_info);

  m_registry.emplace<Quad>(background, quad);
  m_registry.emplace<vk::Texture_Info>(background, background_texture_info);
  m_registry.emplace<vk::Draw_Info>(background, background_draw_info);
  m_registry.emplace<vk::Compute_Info>(background, background_compute_info);

  auto create_world = m_registry.create();
  vk::Buffer_Info create_world_buffer_info = ui::create_buffer_info(m_allocator);
  vk::Draw_Info create_world_draw_info = ui::create_draw_info(m_device);

  ui::Button create_world_button = {};
  create_world_button.x = button_x;
  create_world_button.y = button_y;
  create_world_button.width = button_width;
  create_world_button.height = button_height;
  create_world_button.locked = false;

  ui::fill_button_data(create_world_button, create_world_buffer_info, button_color, m_allocator);

  ui::Label create_world_label = { "Create World" };

  m_registry.emplace<Ui>(create_world, ui);
  m_registry.emplace<Button>(create_world, button);
  m_registry.emplace<vk::Draw_Info>(create_world, create_world_draw_info);
  m_registry.emplace<vk::Buffer_Info>(create_world, create_world_buffer_info);
  m_registry.emplace<ui::Button>(create_world, create_world_button);
  m_registry.emplace<ui::Color>(create_world, button_color);
  m_registry.emplace<ui::Label>(create_world, create_world_label);

  auto design_lab = m_registry.create();
  vk::Buffer_Info design_lab_buffer_info = ui::create_buffer_info(m_allocator);
  vk::Draw_Info design_lab_draw_info = ui::create_draw_info(m_device);

  button_y += button_height + inner_gap;
  ui::Button design_lab_button = {};
  design_lab_button.x = button_x;
  design_lab_button.y = button_y;
  design_lab_button.width = button_width;
  design_lab_button.height = button_height;
  design_lab_button.locked = true;

  ui::fill_button_data(design_lab_button, design_lab_buffer_info, button_color, m_allocator);

  ui::Label design_lab_label = { "Design Lab" };

  m_registry.emplace<Ui>(design_lab, ui);
  m_registry.emplace<Button>(design_lab, button);
  m_registry.emplace<vk::Draw_Info>(design_lab, design_lab_draw_info);
  m_registry.emplace<vk::Buffer_Info>(design_lab, design_lab_buffer_info);
  m_registry.emplace<ui::Button>(design_lab, design_lab_button);
  m_registry.emplace<ui::Color>(design_lab, button_color);
  m_registry.emplace<ui::Label>(design_lab, design_lab_label);

  auto start = m_registry.create();
  vk::Buffer_Info start_buffer_info = ui::create_buffer_info(m_allocator);
  vk::Draw_Info start_draw_info = ui::create_draw_info(m_device);

  button_y += button_height + inner_gap;
  ui::Button start_button = {};
  start_button.x = button_x;
  start_button.y = button_y;
  start_button.width = button_width;
  start_button.height = button_height;
  start_button.locked = true;

  ui::fill_button_data(start_button, start_buffer_info, button_color, m_allocator);

  ui::Label start_label = { "Start" };

  m_registry.emplace<Ui>(start, ui);
  m_registry.emplace<Button>(start, button);
  m_registry.emplace<vk::Draw_Info>(start, start_draw_info);
  m_registry.emplace<vk::Buffer_Info>(start, start_buffer_info);
  m_registry.emplace<ui::Button>(start, start_button);
  m_registry.emplace<ui::Color>(start, button_color);
  m_registry.emplace<ui::Label>(start, start_label);

  auto load = m_registry.create();
  vk::Buffer_Info load_buffer_info = ui::create_buffer_info(m_allocator);
  vk::Draw_Info load_draw_info = ui::create_draw_info(m_device);

  button_y += button_height + inner_gap;
  ui::Button load_button = {};
  load_button.x = button_x;
  load_button.y = button_y;
  load_button.width = button_width;
  load_button.height = button_height;
  load_button.locked = true;

  ui::fill_button_data(load_button, load_buffer_info, button_color, m_allocator);

  ui::Label load_label = { "Load" };

  m_registry.emplace<Ui>(load, ui);
  m_registry.emplace<Button>(load, button);
  m_registry.emplace<vk::Draw_Info>(load, load_draw_info);
  m_registry.emplace<vk::Buffer_Info>(load, load_buffer_info);
  m_registry.emplace<ui::Button>(load, load_button);
  m_registry.emplace<ui::Color>(load, button_color);
  m_registry.emplace<ui::Label>(load, load_label);

  auto quit = m_registry.create();
  vk::Buffer_Info quit_buffer_info = ui::create_buffer_info(m_allocator);
  vk::Draw_Info quit_draw_info = ui::create_draw_info(m_device);

  button_y += button_height + inner_gap;
  ui::Button quit_button = {};
  quit_button.x = button_x;
  quit_button.y = button_y;
  quit_button.width = button_width;
  quit_button.height = button_height;
  quit_button.locked = false;

  ui::fill_button_data(quit_button, quit_buffer_info, button_color, m_allocator);

  ui::Label quit_label = { "Quit" };

  m_registry.emplace<Ui>(quit, ui);
  m_registry.emplace<Button>(quit, button);
  m_registry.emplace<vk::Draw_Info>(quit, quit_draw_info);
  m_registry.emplace<vk::Buffer_Info>(quit, quit_buffer_info);
  m_registry.emplace<ui::Button>(quit, quit_button);
  m_registry.emplace<ui::Color>(quit, button_color);
  m_registry.emplace<ui::Label>(quit, quit_label);
}

void Simulation::set_create_world()
{
  vkDeviceWaitIdle(m_device);
  m_registry.clear();
  m_state = CREATE_WORLD;

  Quad quad = {};
  Panel panel = {};

  auto background = m_registry.create();
  vk::Draw_Info background_draw_info = background::create_quad_draw_info(m_images.size(), m_device);
  vk::Compute_Info background_compute_info = background::create_texture_compute_info(m_images.size(), m_device);
  vk::Texture_Info background_texture_info = background::create_texture_info(m_images.size(), m_width, m_height, m_queues.graphics, m_graphics_pool, m_device, m_allocator, background_draw_info, background_compute_info);

  m_registry.emplace<Quad>(background, quad);
  m_registry.emplace<vk::Texture_Info>(background, background_texture_info);
  m_registry.emplace<vk::Draw_Info>(background, background_draw_info);
  m_registry.emplace<vk::Compute_Info>(background, background_compute_info);

  auto left_panel = m_registry.create();

  vk::Draw_Info left_panel_draw_info = ui::create_draw_info(m_device);
  vk::Buffer_Info left_panel_buffer_info = ui::create_buffer_info(m_allocator);

  float width = (float)m_width;
  float height = (float)m_height;

  ui::Panel left_panel_struct = {};
  left_panel_struct.x = 0.0f;
  left_panel_struct.y = 0.0f;
  left_panel_struct.width = 0.175f*width;
  left_panel_struct.height = height;
  
  left_panel_struct.vx = 0.0f;
  left_panel_struct.vy = 0.0f;

  left_panel_struct.mode = ui::ACTIVE;
  left_panel_struct.side = ui::LEFT;
  left_panel_struct.binding = ui::BOUND;

  left_panel_struct.tab_count = 3;
  
  float tab_width = 0.25f*left_panel_struct.width;
  float tab_height = 0.16f*left_panel_struct.height;

    auto space_tab = m_registry.create();
    ui::Tab space_tab_struct = {};
    space_tab_struct.width = tab_width;
    space_tab_struct.height = tab_height;
    space_tab_struct.index = 0;
    space_tab_struct.parent = left_panel;

    auto space_mesh = ui::create_tab_mesh(width, height, space_tab_struct, left_panel_struct);
    space_tab_struct.x = space_mesh[0];
    space_tab_struct.y = space_mesh[1];

    left_panel_struct.active_index = 0;
    left_panel_struct.tabs.push_back(space_tab);

    ui::Color space_color = {};
    space_color.normal = {77.0f/255.0f, 9.0f/255.0f, 125.0f/255.0f};
    space_color.gray = {125.0f/255.0f, 125.0f/255.0f, 125.0f/255.0f};

    auto space_draw_info = ui::create_draw_info(m_device);
    auto space_buffer_info = ui::create_buffer_info(m_allocator);

    m_registry.emplace<ui::Tab>(space_tab, space_tab_struct);
    m_registry.emplace<ui::Color>(space_tab, space_color);
    m_registry.emplace<vk::Draw_Info>(space_tab, space_draw_info);
    m_registry.emplace<vk::Buffer_Info>(space_tab, space_buffer_info);

    ui::fill_tab_data(m_width, m_height, left_panel_struct, space_tab_struct, space_buffer_info, space_color, m_allocator);
    std::cout << std::endl;

    auto geography_tab = m_registry.create();
    ui::Tab geography_tab_struct = {};
    geography_tab_struct.width = tab_width;
    geography_tab_struct.height = tab_height;
    geography_tab_struct.index = 1;
    geography_tab_struct.parent = left_panel;

    auto geography_mesh = ui::create_tab_mesh(width, height, geography_tab_struct, left_panel_struct);
    geography_tab_struct.x = geography_mesh[0];
    geography_tab_struct.y = geography_mesh[1];

    ui::Color geography_color = {};
    geography_color.normal = {189.0f/255.0f, 94.0f/255.0f, 0.0f/255.0f};
    geography_color.gray = {125.0f/255.0f, 125.0f/255.0f, 125.0f/255.0f};

    auto geography_draw_info = ui::create_draw_info(m_device);
    auto geography_buffer_info = ui::create_buffer_info(m_allocator);

    m_registry.emplace<ui::Tab>(geography_tab, geography_tab_struct);
    m_registry.emplace<ui::Color>(geography_tab, geography_color);
    m_registry.emplace<vk::Draw_Info>(geography_tab, geography_draw_info);
    m_registry.emplace<vk::Buffer_Info>(geography_tab, geography_buffer_info);
  
    left_panel_struct.tabs.push_back(geography_tab);

    ui::fill_tab_data(m_width, m_height, left_panel_struct, geography_tab_struct, geography_buffer_info, geography_color, m_allocator);

    auto ecology_tab = m_registry.create();
    ui::Tab ecology_tab_struct = {};
    ecology_tab_struct.width = tab_width;
    ecology_tab_struct.height = tab_height;
    ecology_tab_struct.index = 2;
    ecology_tab_struct.parent = left_panel;

    auto ecology_mesh = ui::create_tab_mesh(width, height, ecology_tab_struct, left_panel_struct);
    ecology_tab_struct.x = ecology_mesh[0];
    ecology_tab_struct.y = ecology_mesh[1];

    ui::Color ecology_color = {};
    ecology_color.normal = {5.0f/255.0f, 115.0f/255.0f, 52.0f/255.0f};
    ecology_color.gray = {125.0f/255.0f, 125.0f/255.0f, 125.0f/255.0f};

    auto ecology_draw_info = ui::create_draw_info(m_device);
    auto ecology_buffer_info = ui::create_buffer_info(m_allocator);

    m_registry.emplace<ui::Tab>(ecology_tab, ecology_tab_struct);
    m_registry.emplace<ui::Color>(ecology_tab, ecology_color);
    m_registry.emplace<vk::Draw_Info>(ecology_tab, ecology_draw_info);
    m_registry.emplace<vk::Buffer_Info>(ecology_tab, ecology_buffer_info);

    left_panel_struct.tabs.push_back(ecology_tab);

    ui::fill_tab_data(m_width, m_height, left_panel_struct, ecology_tab_struct, ecology_buffer_info, ecology_color, m_allocator);

  m_registry.emplace<vk::Draw_Info>(left_panel, left_panel_draw_info);
  m_registry.emplace<vk::Buffer_Info>(left_panel, left_panel_buffer_info);
  m_registry.emplace<ui::Panel>(left_panel, left_panel_struct);
  m_registry.emplace<Panel>(left_panel, panel);

  auto color = m_registry.get<ui::Color>(left_panel_struct.tabs[left_panel_struct.active_index]);
  ui::fill_panel_data(left_panel_struct, left_panel_buffer_info, color.normal, m_allocator);

  auto right_panel = m_registry.create();

  auto right_panel_draw_info = ui::create_draw_info(m_device);
  auto right_panel_buffer_info = ui::create_buffer_info(m_allocator);

  ui::Panel right_panel_struct = {};
  right_panel_struct.x = width;
  right_panel_struct.y = 0.0f;
  right_panel_struct.width = 0.175f*width;
  right_panel_struct.height = height;

  right_panel_struct.vx = 0.0f;
  right_panel_struct.vy = 0.0f;

  right_panel_struct.mode = ui::PASIVE;
  right_panel_struct.side = ui::RIGHT;
  right_panel_struct.binding = ui::BOUND;

  right_panel_struct.tab_count = 1;

    auto political_tab = m_registry.create();
    ui::Tab political_tab_struct = {};
    political_tab_struct.width = tab_width;
    political_tab_struct.height = tab_height;
    political_tab_struct.index = 0;
    political_tab_struct.parent = right_panel;

    auto political_mesh = ui::create_tab_mesh(width, height, political_tab_struct, right_panel_struct);
    political_tab_struct.x = political_mesh[0];
    political_tab_struct.y = political_mesh[1];

    ui::Color political_tab_color = {};
    political_tab_color.normal = { 0.0f, 0.0f, 1.0f };
    political_tab_color.highlight = { 0.0f, 0.0f, 0.0f };
    political_tab_color.gray = { 0.0f, 0.0f, 0.0f };

    auto political_draw_info = ui::create_draw_info(m_device);
    auto political_buffer_info = ui::create_buffer_info(m_allocator);

    m_registry.emplace<ui::Tab>(political_tab, political_tab_struct);
    m_registry.emplace<ui::Color>(political_tab, political_tab_color);
    m_registry.emplace<vk::Draw_Info>(political_tab, political_draw_info);
    m_registry.emplace<vk::Buffer_Info>(political_tab, political_buffer_info);

    right_panel_struct.active_index = 0;
    right_panel_struct.tabs.push_back(political_tab);

    ui::fill_tab_data(m_width, m_height, right_panel_struct, political_tab_struct, political_buffer_info, political_tab_color, m_allocator);

  m_registry.emplace<Panel>(right_panel, panel);
  m_registry.emplace<ui::Panel>(right_panel, right_panel_struct);
  m_registry.emplace<vk::Draw_Info>(right_panel, right_panel_draw_info);
  m_registry.emplace<vk::Buffer_Info>(right_panel, right_panel_buffer_info);

  auto right_color = m_registry.get<ui::Color>(right_panel_struct.tabs[right_panel_struct.active_index]);
  ui::fill_panel_data(right_panel_struct, right_panel_buffer_info, color.normal, m_allocator);
}

void Simulation::set_design_lab()
{
  vkDeviceWaitIdle(m_device);
  m_registry.clear();
  m_state = DESIGN_LAB;
}

void Simulation::set_start()
{
  vkDeviceWaitIdle(m_device);
  m_registry.clear();
  m_state = START;
}

void Simulation::set_load()
{
  vkDeviceWaitIdle(m_device);
  m_registry.clear();
  m_state = LOAD;
}

void Simulation::set_running()
{
  vkDeviceWaitIdle(m_device);
  m_registry.clear();
  m_state = RUNNING;
}