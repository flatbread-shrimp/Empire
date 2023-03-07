#include "../include/ui.hpp"
#include "../include/shader.hpp"
#include "../include/data.hpp"

void emplace_panel(entt::entity entity, entt::registry& registry)
{
  Panel panel = {};
  registry.emplace<Panel>(entity, panel);
}

bool match_sides(ui::Panel& panel_one, ui::Panel& panel_two)
{
  bool found = false;
  if((panel_one.side == ui::LEFT && panel_two.side == ui::RIGHT) || (panel_one.side == ui::RIGHT && panel_two.side == ui::LEFT))
  {
    found = true;
  }
  else if((panel_one.side == ui::TOP && panel_two.side == ui::BOTTOM) || (panel_one.side == ui::BOTTOM && panel_two.side == ui::TOP))
  {
    found = true;
  }

  return found;
}

vk::Buffer_Info ui::create_buffer_info(VmaAllocator allocator)
{
  vk::Buffer_Info info = {};

  VkBufferCreateInfo position_buffer_create_info = {};
  position_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  position_buffer_create_info.size = sizeof(float)*2*4;
  position_buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo position_allocation_create_info = {};
  position_allocation_create_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
  position_allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;

  VkBuffer position_buffer;
  VmaAllocation position_allocation;
  auto result = vmaCreateBuffer(allocator, &position_buffer_create_info, &position_allocation_create_info, &position_buffer, &position_allocation, nullptr);
  if(result != VK_SUCCESS)
  {
    std::cout << "The button buffer could not be created. Error code: " << result  << std::endl;
    throw;
  }

  info.buffers.push_back(position_buffer);
  info.allocations.push_back(position_allocation);

  VkBufferCreateInfo color_buffer_create_info = {};
  color_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  color_buffer_create_info.size = sizeof(float)*3;
  color_buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo color_allocation_create_info = {};
  color_allocation_create_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
  color_allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;

  VkBuffer color_buffer;
  VmaAllocation color_allocation;
  result = vmaCreateBuffer(allocator, &color_buffer_create_info, &color_allocation_create_info, &color_buffer, &color_allocation, nullptr);
  if(result != VK_SUCCESS)
  {
    std::cout << "The buttons color buffer could not be created. Error code: " << result << std::endl;
    throw;
  }

  info.buffers.push_back(color_buffer);
  info.allocations.push_back(color_allocation);

  VkBufferCreateInfo index_buffer_create_info = {};
  index_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  index_buffer_create_info.size = sizeof(std::uint32_t)*6;
  index_buffer_create_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo index_allocation_create_info = {};
  index_allocation_create_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
  index_allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;

  VkBuffer index_buffer;
  VmaAllocation index_allocation;
  result = vmaCreateBuffer(allocator, &index_buffer_create_info, &index_allocation_create_info, &index_buffer, &index_allocation, nullptr);
  if(result != VK_SUCCESS)
  {
    std::cout << "The buttons index buffer could not be created. Error code: " << result << std::endl;
    throw;
  }

  info.buffers.push_back(index_buffer);
  info.allocations.push_back(index_allocation);

  info.allocator = allocator;
  return info;
}

vk::Draw_Info ui::create_draw_info(VkDevice device)
{
  vk::Draw_Info info = {};
    
  std::string vertex_path = "ui_vert.spv";
  auto vertex_module = read_shader(vertex_path, device);

  VkPipelineShaderStageCreateInfo vertex_stage_info = {};
  vertex_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertex_stage_info.pNext = nullptr;
  vertex_stage_info.flags = 0;
  vertex_stage_info.module = vertex_module;
  vertex_stage_info.pName = "main";
  vertex_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertex_stage_info.pSpecializationInfo = nullptr;

  std::string fragment_path = "ui_frag.spv";
  auto fragment_module = read_shader(fragment_path, device);

  VkPipelineShaderStageCreateInfo fragment_stage_info = {};
  fragment_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragment_stage_info.pNext = nullptr;
  fragment_stage_info.flags = 0;
  fragment_stage_info.module = fragment_module;
  fragment_stage_info.pName = "main";
  fragment_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragment_stage_info.pSpecializationInfo = nullptr;

  std::vector<VkPipelineShaderStageCreateInfo> shader_infos = {vertex_stage_info, fragment_stage_info};

  std::vector<VkDynamicState> dynamic_states = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamic_state_info = {};
  dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_info.pNext = nullptr;
  dynamic_state_info.flags = 0;
  dynamic_state_info.dynamicStateCount = dynamic_states.size();
  dynamic_state_info.pDynamicStates = dynamic_states.data();

  VkVertexInputBindingDescription position_binding = {};
  position_binding.binding = 0;
  position_binding.stride = sizeof(float)*2;
  position_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  VkVertexInputBindingDescription color_binding = {};
  color_binding.binding = 1;
  color_binding.stride = sizeof(float)*3;
  color_binding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

  std::vector<VkVertexInputBindingDescription> bindings = { position_binding, color_binding };

  VkVertexInputAttributeDescription position_attribute = {};
  position_attribute.binding = 0;
  position_attribute.location = 0;
  position_attribute.format = VK_FORMAT_R32G32_SFLOAT;
  position_attribute.offset = 0;

  VkVertexInputAttributeDescription color_attribute = {};
  color_attribute.binding = 1;
  color_attribute.location = 1;
  color_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
  color_attribute.offset = 0;

  std::vector<VkVertexInputAttributeDescription> attributes = { position_attribute, color_attribute };

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.pNext = nullptr;
  vertex_input_info.flags = 0;
  vertex_input_info.vertexBindingDescriptionCount = bindings.size();
  vertex_input_info.pVertexBindingDescriptions = bindings.data();
  vertex_input_info.vertexAttributeDescriptionCount = attributes.size();
  vertex_input_info.pVertexAttributeDescriptions = attributes.data();

  VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
  input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_info.pNext = nullptr;
  input_assembly_info.flags = 0;
  input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_info.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewport_state_info = {};
  viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_info.pNext = nullptr;
  viewport_state_info.flags = 0;
  viewport_state_info.viewportCount = 1;
  viewport_state_info.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer_info = {};
  rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer_info.pNext = nullptr;
  rasterizer_info.flags = 0;
  rasterizer_info.depthClampEnable = VK_FALSE;
  rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
  rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer_info.lineWidth = 1.0f;
  rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer_info.depthBiasEnable = VK_FALSE;
  rasterizer_info.depthBiasConstantFactor = 0.0f;
  rasterizer_info.depthBiasClamp = 0.0f;
  rasterizer_info.depthBiasSlopeFactor = 0.0f;

  VkPipelineMultisampleStateCreateInfo multisampling_info = {};
  multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_info.pNext = nullptr;
  multisampling_info.flags = 0;
  multisampling_info.sampleShadingEnable = VK_FALSE;
  multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling_info.minSampleShading = 1.0f;
  multisampling_info.pSampleMask = nullptr;
  multisampling_info.alphaToCoverageEnable = VK_FALSE;
  multisampling_info.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_TRUE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo color_blending_info = {};
  color_blending_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending_info.pNext = nullptr;
  color_blending_info.flags = 0;
  color_blending_info.logicOpEnable = VK_FALSE;
  color_blending_info.logicOp = VK_LOGIC_OP_COPY;
  color_blending_info.attachmentCount = 1;
  color_blending_info.pAttachments = &color_blend_attachment;
  color_blending_info.blendConstants[0] = 0.0f;
  color_blending_info.blendConstants[1] = 0.0f;
  color_blending_info.blendConstants[2] = 0.0f;
  color_blending_info.blendConstants[3] = 0.0f;

  VkPushConstantRange transform_range = {};
  transform_range.offset = 0;
  transform_range.size = 2*sizeof(float);
  transform_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.pNext = nullptr;
  pipeline_layout_create_info.flags = 0;
  pipeline_layout_create_info.setLayoutCount = 0;
  pipeline_layout_create_info.pSetLayouts = nullptr;
  pipeline_layout_create_info.pushConstantRangeCount = 1;
  pipeline_layout_create_info.pPushConstantRanges = &transform_range;

  auto result = vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &info.layout);
  if(result != VK_SUCCESS)
  {
    std::cout << "The button pipeline layout could not be created. Error code: " << result << std::endl;
    throw;
  }

  VkGraphicsPipelineCreateInfo pipeline_create_info = {};
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.pNext = nullptr;
  pipeline_create_info.flags = 0;
  pipeline_create_info.stageCount = shader_infos.size();
  pipeline_create_info.pStages = shader_infos.data();
  pipeline_create_info.pVertexInputState = &vertex_input_info;
  pipeline_create_info.pInputAssemblyState = &input_assembly_info;
  pipeline_create_info.pViewportState = &viewport_state_info;
  pipeline_create_info.pRasterizationState = &rasterizer_info;
  pipeline_create_info.pMultisampleState = &multisampling_info;
  pipeline_create_info.pDepthStencilState = nullptr;
  pipeline_create_info.pColorBlendState = &color_blending_info;
  pipeline_create_info.pDynamicState = &dynamic_state_info;
  pipeline_create_info.layout = info.layout;
  pipeline_create_info.renderPass = VK_NULL_HANDLE;
  pipeline_create_info.subpass = 0;
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_create_info.basePipelineIndex = -1;

  result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &info.pipeline);
  if(result != VK_SUCCESS)
  {
    std::cout << "The buttons pipelines could not be created. Error code: " << result << std::endl;
    throw;
  }

  vkDestroyShaderModule(device, vertex_module, nullptr);
  vkDestroyShaderModule(device, fragment_module, nullptr);

  info.device = device;
  return info;
}

std::array<float, 8> ui::create_mesh(float x, float y, float width, float height)
{
  std::array<float, 8> mesh = {
    x,                y,
    x + width, y,
    x,                y + height,
    x + width, y + height
  };

  return mesh;
}

void ui::fill_button_data(Button& button, vk::Buffer_Info& info, Color& color, VmaAllocator& allocator)
{
  std::array<std::uint32_t, 6> indices = {
    0, 3, 2, 
    0, 1, 3
  };

  auto positions = create_mesh(button.x, button.y, button.width, button.height);

  void* location;
  vmaMapMemory(allocator, info.allocations[0], &location);
  memcpy(location, positions.data(), positions.size()*sizeof(float));
  vmaUnmapMemory(allocator, info.allocations[0]);

  if(button.locked)
  {
    vmaMapMemory(allocator, info.allocations[1], &location);
    memcpy(location, color.gray.data(), color.gray.size()*sizeof(float));
    vmaUnmapMemory(allocator, info.allocations[1]);
  }
  else
  {
    vmaMapMemory(allocator, info.allocations[1], &location);
    memcpy(location, color.normal.data(), color.normal.size()*sizeof(float));
    vmaUnmapMemory(allocator, info.allocations[1]);
  }

  vmaMapMemory(allocator, info.allocations[2], &location);
  memcpy(location, indices.data(), indices.size()*sizeof(std::uint32_t));
  vmaUnmapMemory(allocator, info.allocations[2]);
}

void ui::fill_panel_data(Panel& panel, vk::Buffer_Info& buffers, std::array<float, 3>& color, VmaAllocator& allocator)
{
  std::array<std::uint32_t, 6> indices = {
    0, 3, 2, 
    0, 1, 3
  };

  auto positions = create_mesh(panel.x, panel.y, panel.width, panel.height);

  void* location;
  vmaMapMemory(allocator, buffers.allocations[0], &location);
  memcpy(location, positions.data(), sizeof(float)*positions.size());
  vmaUnmapMemory(allocator, buffers.allocations[0]);

  vmaMapMemory(allocator, buffers.allocations[1], &location);
  memcpy(location, color.data(), sizeof(float)*color.size());
  vmaUnmapMemory(allocator, buffers.allocations[1]);

  vmaMapMemory(allocator, buffers.allocations[2], &location);
  memcpy(location, indices.data(), sizeof(float)*indices.size());
  vmaUnmapMemory(allocator, buffers.allocations[2]);
}

void ui::fill_tab_data(float width, float height, Panel& panel, Tab& tab, vk::Buffer_Info& buffers, Color& color, VmaAllocator& allocator)
{
  std::array<std::uint32_t, 6> indices = {
    0, 3, 2, 
    0, 1, 3
  };

  auto positions = create_tab_mesh(width, height, tab, panel);

  void* location;
  vmaMapMemory(allocator, buffers.allocations[0], &location);
  memcpy(location, positions.data(), sizeof(float)*positions.size());
  vmaUnmapMemory(allocator, buffers.allocations[0]);

  vmaMapMemory(allocator, buffers.allocations[1], &location);
  memcpy(location, color.normal.data(), sizeof(float)*color.normal.size());
  vmaUnmapMemory(allocator, buffers.allocations[1]);

  vmaMapMemory(allocator, buffers.allocations[2], &location);
  memcpy(location, indices.data(), sizeof(float)*indices.size());
  vmaUnmapMemory(allocator, buffers.allocations[2]);
}

void ui::highlight_color(Color& color, vk::Buffer_Info& buffers, VmaAllocator& allocator)
{
  void* location;
  vmaMapMemory(allocator, buffers.allocations[1], &location);
  memcpy(location, color.highlight.data(), color.highlight.size()*sizeof(float));
  vmaUnmapMemory(allocator, buffers.allocations[1]);
}

void ui::unhighlight_color(Color& color, vk::Buffer_Info& buffers, VmaAllocator& allocator)
{
  void* location;
  vmaMapMemory(allocator, buffers.allocations[1], &location);
  memcpy(location, color.normal.data(), color.normal.size()*sizeof(float));
  vmaUnmapMemory(allocator, buffers.allocations[1]);
}

void ui::gray_color(Color& color, vk::Buffer_Info& buffers, VmaAllocator& allocator)
{
  void* location;
  vmaMapMemory(allocator, buffers.allocations[1], &location);
  memcpy(location, color.gray.data(), color.gray.size()*sizeof(float));
  vmaUnmapMemory(allocator, buffers.allocations[1]);
}

void ui::ungray_color(Color& color, vk::Buffer_Info& buffers, VmaAllocator& allocator)
{
  void* location;
  vmaMapMemory(allocator, buffers.allocations[1], &location);
  memcpy(location, color.normal.data(), color.normal.size()*sizeof(float));
  vmaUnmapMemory(allocator, buffers.allocations[1]);
}

bool ui::inside(float px, float py, float x, float y, float width, float height)
{
  bool x_check = px > x && px < x + width;
  bool y_check = py > y && py < y + height;

  return x_check && y_check;
}

std::array<float, 8> ui::create_tab_mesh(float width, float height, Tab& tab, Panel& panel)
{
  float x, y;
  switch(panel.side)
  {
    case LEFT:
      x = panel.x + panel.width;
      y = panel.height/2.0f + ((float)tab.index - (float)panel.tab_count/2.0f)*tab.height;
      break;

    case RIGHT:
      x = panel.x - tab.width;
      y = panel.height/2.0f + ((float)tab.index - (float)panel.tab_count/2.0f)*tab.height;
      break;

    case TOP:
      x = panel.width/2.0f + ((float)tab.index - (float)panel.tab_count/2.0f)*tab.width;
      y = panel.y + tab.height;
      break;

    case BOTTOM:
      x = panel.width/2.0f + ((float)tab.index - (float)panel.tab_count/2.0f)*tab.width;
      y = panel.y - tab.height;
      break;

    default:
      std::cout << "We fell through folks!" << std::endl;
  }

  std::array<float, 8> mesh = {
    x,             y,
    x + tab.width, y,
    x,             y + tab.height,
    x + tab.width, y + tab.height
  };
  return mesh;
}

void ui::set_panel_to_passive(float& v, entt::entity& panel_element, entt::registry& registry)
{
  auto& panel_struct = registry.get<ui::Panel>(panel_element);
  switch(panel_struct.side)
  {
    case LEFT:
      panel_struct.vx = v;
      panel_struct.vy = 0.0f;
      panel_struct.mode = MOTION;
      panel_struct.motion_direction = LEFT;
      panel_struct.transition_target = PASIVE;
      panel_struct.transition_side = LEFT;
      break;
    case RIGHT:
      panel_struct.vx = v;
      panel_struct.vy = 0.0f;
      panel_struct.mode = MOTION;
      panel_struct.motion_direction = RIGHT;
      panel_struct.transition_target = PASIVE;
      panel_struct.transition_side = RIGHT;
      break;
    case TOP:
      panel_struct.vx = 0.0f;
      panel_struct.vy = v;
      panel_struct.mode = MOTION;
      panel_struct.motion_direction = TOP;
      panel_struct.transition_target = PASIVE;
      panel_struct.transition_side = TOP;
      break;
    case BOTTOM:
      panel_struct.vx = 0.0f;
      panel_struct.vy = v;
      panel_struct.mode = MOTION;
      panel_struct.motion_direction = BOTTOM;
      panel_struct.transition_target = PASIVE;
      panel_struct.transition_side = BOTTOM;
      break;
    case NONE:
      break;
  }

  registry.patch<ui::Panel>(panel_element);
}

void ui::set_panel_to_active(float& v, entt::entity& panel_element, entt::registry& registry)
{
  auto& panel_struct = registry.get<ui::Panel>(panel_element);
  switch(panel_struct.side)
  {
    case LEFT:
      panel_struct.vx = v;
      panel_struct.vy = 0.0f;
      panel_struct.mode = MOTION;
      panel_struct.motion_direction = RIGHT;
      panel_struct.transition_target = ACTIVE;
      panel_struct.transition_side = LEFT;
      break;
    case RIGHT:
      panel_struct.vx = v;
      panel_struct.vy = 0.0f;
      panel_struct.mode = MOTION;
      panel_struct.motion_direction = LEFT;
      panel_struct.transition_target = ACTIVE;
      panel_struct.transition_side = RIGHT;
      break;
    case TOP:
      panel_struct.vx = 0.0f;
      panel_struct.vy = v;
      panel_struct.mode = MOTION;
      panel_struct.motion_direction = BOTTOM;
      panel_struct.transition_target = ACTIVE;
      panel_struct.transition_side = TOP;
      break;
    case BOTTOM:
      panel_struct.vx = 0.0f;
      panel_struct.vy = v;
      panel_struct.mode = MOTION;
      panel_struct.motion_direction = TOP;
      panel_struct.transition_target = ACTIVE;
      panel_struct.transition_side = BOTTOM;
      break;
    case NONE:
      break;
  }

  registry.patch<ui::Panel>(panel_element);
}

void ui::set_tab_to_motion(float width, float height, float v, bool horisontal, entt::entity& tab_element, entt::registry& registry, VkDevice device, VmaAllocator allocator)
{
  auto& tab_struct = registry.get<ui::Tab>(tab_element);

  auto new_panel_element = registry.create();
  ui::Panel new_panel_struct = {};

  auto& old_panel_struct = registry.get<ui::Panel>(tab_struct.parent);

  new_panel_struct.x = old_panel_struct.x;
  new_panel_struct.y = old_panel_struct.y;
  if(horisontal)
  {
    new_panel_struct.vx = v;
    new_panel_struct.vy = 0.0f;
  }
  else
  {
    new_panel_struct.vx = 0.0f;
    new_panel_struct.vy = v;
  }

  new_panel_struct.active_index = 0;
  new_panel_struct.tab_count = 1;
  new_panel_struct.tabs.push_back(tab_element);

  auto panel_view = registry.view<Panel>();
  for(auto test_panel_element : panel_view)
  {
    auto test_panel_struct = registry.get<ui::Panel>(test_panel_element);
    if(match_sides(old_panel_struct, test_panel_struct))
    {
      new_panel_struct.transition_target = test_panel_struct.mode;
      break;
    }
  }
  new_panel_struct.mode = MOTION;
  new_panel_struct.side = NONE;

  switch(old_panel_struct.side)
  {
    case LEFT:
      new_panel_struct.side = LEFT;
      new_panel_struct.transition_side = RIGHT;
      new_panel_struct.motion_direction = RIGHT;
      break;
    case RIGHT:
      new_panel_struct.side = RIGHT;
      new_panel_struct.transition_side = LEFT;
      new_panel_struct.motion_direction = LEFT;
      break;
    case TOP:
      new_panel_struct.side = TOP;
      new_panel_struct.transition_side = BOTTOM;
      new_panel_struct.motion_direction = BOTTOM;
      break;
    case BOTTOM:
      new_panel_struct.side = BOTTOM;
      new_panel_struct.transition_side = TOP;
      new_panel_struct.motion_direction = TOP;
      break;
  }

  new_panel_struct.binding = FREE;

  new_panel_struct.width = old_panel_struct.width;
  new_panel_struct.height = old_panel_struct.height;
  new_panel_struct.x = old_panel_struct.x;
  new_panel_struct.y = old_panel_struct.y;

  auto draw_info = ui::create_draw_info(device);
  auto buffer_info = ui::create_buffer_info(allocator);

  emplace_panel(new_panel_element, registry);
  registry.emplace<ui::Panel>(new_panel_element, new_panel_struct);
  registry.emplace<vk::Draw_Info>(new_panel_element, draw_info);
  registry.emplace<vk::Buffer_Info>(new_panel_element, buffer_info);

  std::uint32_t critical_index = UINT32_MAX;
  for(auto it = old_panel_struct.tabs.end() - 1; it >= old_panel_struct.tabs.begin(); it--)
  {
    if(*it == tab_element)
    {
      if(old_panel_struct.active_index == tab_struct.index && old_panel_struct.active_index != 0)
      {
        old_panel_struct.active_index -= 1;
      }
      else if(old_panel_struct.active_index == tab_struct.index)
      {
        old_panel_struct.active_index += 1;
      }

      critical_index = registry.get<ui::Tab>(*it).index;

      old_panel_struct.tab_count -= 1;
      old_panel_struct.tabs.erase(it);
    }
  }

  for(auto old_panel_tab_element : old_panel_struct.tabs)
  {
    auto& old_panel_tab_struct = registry.get<ui::Tab>(old_panel_tab_element);
    if(old_panel_tab_struct.index > critical_index)
    {
      old_panel_tab_struct.index -= 1;
    }
    registry.patch<ui::Tab>(old_panel_tab_element);
  }

  if(old_panel_struct.tab_count != 0)
  {
    registry.patch<ui::Panel>(tab_struct.parent);

    auto old_panel_buffers = registry.get<vk::Buffer_Info>(tab_struct.parent);
    auto old_panel_color = registry.get<ui::Color>(old_panel_struct.tabs[old_panel_struct.active_index]).normal;

    ui::fill_panel_data(old_panel_struct, old_panel_buffers, old_panel_color, allocator);
    for(auto old_panel_tab_element : old_panel_struct.tabs)
    {
      auto old_panel_tab_struct = registry.get<ui::Tab>(old_panel_tab_element);
      auto old_panel_tab_buffers = registry.get<vk::Buffer_Info>(old_panel_tab_element);
      auto old_panel_tab_color = registry.get<ui::Color>(old_panel_tab_element);

      ui::fill_tab_data(width, height, old_panel_struct, old_panel_tab_struct, old_panel_tab_buffers, old_panel_tab_color, allocator);
    }
  }
  else
  {
    vkDeviceWaitIdle(device);
    registry.destroy(tab_struct.parent);
  }

  tab_struct.index = 0;
  tab_struct.parent = new_panel_element;
  registry.patch<ui::Tab>(tab_element);

  auto color = registry.get<ui::Color>(tab_element);
  auto tab_buffers = registry.get<vk::Buffer_Info>(tab_element);

  ui::fill_panel_data(new_panel_struct, buffer_info, color.normal, allocator);
  ui::fill_tab_data(width, height, new_panel_struct, tab_struct, tab_buffers, color, allocator);
}

void ui::combine_tab_with_panel(float width, float height, entt::entity& tab, entt::entity& tab_panel, entt::entity& panel, entt::registry& registry, VkDevice device, VmaAllocator allocator)
{
  vkDeviceWaitIdle(device);
  auto& panel_struct = registry.get<ui::Panel>(panel);
  
  panel_struct.tab_count += 1;
  panel_struct.tabs.push_back(tab);

  registry.patch<ui::Panel>(panel);
  registry.destroy(tab_panel);

  auto& tab_struct = registry.get<ui::Tab>(tab);

  std::uint32_t max_index = 0;
  for(auto tab_element : panel_struct.tabs)
  {
    auto index = registry.get<ui::Tab>(tab_element).index;
    if(index > max_index)
    {
      max_index = index;
    }
  }
  tab_struct.index = max_index + 1;
  tab_struct.parent = panel;

  registry.patch<ui::Tab>(tab);

  auto panel_buffers = registry.get<vk::Buffer_Info>(panel);
  auto panel_color = registry.get<ui::Color>(panel_struct.tabs[panel_struct.active_index]).normal;
  ui::fill_panel_data(panel_struct, panel_buffers, panel_color, allocator);
  
  for(auto tab_element : panel_struct.tabs)
  {
    auto& panel_tab_struct = registry.get<ui::Tab>(tab_element);
    
    auto mesh = ui::create_tab_mesh(width, height, panel_tab_struct, panel_struct);
    panel_tab_struct.x = mesh[0];
    panel_tab_struct.y = mesh[1];
    
    registry.patch<ui::Tab>(tab_element);

    auto panel_tab_buffers = registry.get<vk::Buffer_Info>(tab_element);
    auto panel_tab_color = registry.get<ui::Color>(tab_element);
    ui::fill_tab_data(width, height, panel_struct, panel_tab_struct, panel_tab_buffers, panel_tab_color, allocator);
  }
}