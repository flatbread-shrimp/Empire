#include "../include/simulation.hpp"
#include "../include/ui.hpp"
#include "../include/font_render.hpp"

VkCommandBuffer Simulation::record_command_buffer(std::uint32_t index)
{ 
  VkCommandBuffer command = m_buffers[index];
  vkResetCommandBuffer(command, 0);

  VkCommandBufferBeginInfo command_begin_info = {};
  command_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_begin_info.pNext = nullptr;
  command_begin_info.flags = 0;
  command_begin_info.pInheritanceInfo = nullptr;

  auto result = vkBeginCommandBuffer(command, &command_begin_info);
  if(result != VK_SUCCESS)
  {
    std::cout << "The quads command buffer could not be started. Error Code: " << result << std::endl;
    throw;
  }

  VkImageMemoryBarrier input_image_barrier = {};
  input_image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  input_image_barrier.pNext = nullptr;
  input_image_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  input_image_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  input_image_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  input_image_barrier.image = m_images[index];
  input_image_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  input_image_barrier.subresourceRange.baseMipLevel = 0;
  input_image_barrier.subresourceRange.levelCount = 1;
  input_image_barrier.subresourceRange.baseArrayLayer = 0;
  input_image_barrier.subresourceRange.layerCount = 1;

  vkCmdPipelineBarrier(command, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &input_image_barrier);

  VkImage texture_image = VK_NULL_HANDLE;

  auto background_view = m_registry.view<Quad>();
  for(auto background_element : background_view)
  {
    auto background_compute = m_registry.get<vk::Compute_Info>(background_element);
    auto background_texture = m_registry.get<vk::Texture_Info>(background_element);
    texture_image = background_texture.textures[index];

    background::transfer_image_between_layouts(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, index, background_texture, m_queues.graphics, m_graphics_pool, m_device);

    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_COMPUTE, background_compute.pipeline);

    vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_COMPUTE, background_compute.layout, 0, 1, &background_texture.compute_sets[index], 0, nullptr);

    //std::uint32_t group_x = ceil( (float)m_width / 32.0f );
    //std::uint32_t group_y = ceil( (float)m_height / 2.0f );

    vkCmdDispatch(command, m_width, m_height, 1);
  }

  VkImageMemoryBarrier texture_barrier = {};
  texture_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  texture_barrier.pNext = nullptr;
  texture_barrier.image = texture_image;
  texture_barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
  texture_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  texture_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  texture_barrier.subresourceRange.baseMipLevel = 0;
  texture_barrier.subresourceRange.levelCount = 1;
  texture_barrier.subresourceRange.baseArrayLayer = 0;
  texture_barrier.subresourceRange.layerCount = 1;

  vkCmdPipelineBarrier(command, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 0, nullptr, 1, &texture_barrier);

  VkClearValue clear_value = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};
  VkRenderingAttachmentInfo rendering_attachment = {};
  rendering_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO; 
  rendering_attachment.pNext = nullptr;
  rendering_attachment.imageView = m_views[index];
  rendering_attachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
  rendering_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  rendering_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  rendering_attachment.clearValue = clear_value;

  VkRenderingInfo rendering_info = {};
  rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  rendering_info.pNext = nullptr;
  rendering_info.flags = 0;
  rendering_info.colorAttachmentCount = 1;
  rendering_info.pColorAttachments = &rendering_attachment;
  rendering_info.layerCount = 1;
  rendering_info.renderArea.offset = {0, 0};
  rendering_info.renderArea.extent = {m_width, m_height};

  vkCmdBeginRendering(command, &rendering_info);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_meta.extent.width);
    viewport.height = static_cast<float>(m_meta.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = m_meta.extent;
    vkCmdSetScissor(command, 0, 1, &scissor);

    /* --- draw the background quad --- */
    background_view = m_registry.view<Quad>();
    for(auto background_entity : background_view)
    {
      auto draw_info = m_registry.get<vk::Draw_Info>(background_entity);
      auto pipeline = draw_info.pipeline;
      auto pipeline_layout = draw_info.layout;
      auto descriptor_sets = draw_info.descriptor_set_layouts;

      auto texture = m_registry.get<vk::Texture_Info>(background_entity);

      vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &texture.graphics_sets[index], 0, nullptr);

      vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

      vkCmdDraw(command, 6, 1, 0, 0);
    }

    /* --- draw the panels --- */
    std::vector<entt::entity> pasives = {};
    std::vector<entt::entity> actives = {};
    std::vector<entt::entity> movers = {};

    auto panel_view = m_registry.view<Panel>();
    for(auto panel_entity : panel_view)
    {
      auto panel = m_registry.get<ui::Panel>(panel_entity);
      if(panel.mode == ui::MOTION)
      {
        movers.push_back(panel_entity);
      }
      else if(panel.mode == ui::ACTIVE)
      {
        actives.push_back(panel_entity);
      }
      else
      {
        pasives.push_back(panel_entity);
      }
    }

    for(auto panel_entity : movers)
    {
      auto panel = m_registry.get<ui::Panel>(panel_entity);
      auto tabs = panel.tabs;

      auto panel_draw = m_registry.get<vk::Draw_Info>(panel_entity);
      auto layout = panel_draw.layout;
      auto pipeline = panel_draw.pipeline;

      auto buffer_info = m_registry.get<vk::Buffer_Info>(panel_entity);
      auto position_buffer = buffer_info.buffers[0];
      auto color_buffer = buffer_info.buffers[1];
      auto index_buffer = buffer_info.buffers[2];

      vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

      VkBuffer vertex_buffers[] = { position_buffer, color_buffer };
      VkDeviceSize offsets[] = { 0, 0 };
      vkCmdBindVertexBuffers(command, 0, 2, vertex_buffers, offsets);

      float width = (float)m_width;
      float height = (float)m_height;
      float constants[] = { 2.0f/width, 2.0f/height };
      vkCmdPushConstants(command, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, 2*sizeof(float), constants);

      vkCmdBindIndexBuffer(command, index_buffer, 0, VK_INDEX_TYPE_UINT32);

      vkCmdDrawIndexed(command, 6, 1, 0, 0, 0);

      for(auto tab_entity : tabs)
      {
        auto tab_draw = m_registry.get<vk::Draw_Info>(tab_entity);
        auto tab_layout = tab_draw.layout;
        auto tab_pipeline = tab_draw.pipeline;

        auto tab_buffers = m_registry.get<vk::Buffer_Info>(tab_entity);
        auto tab_position_buffer = tab_buffers.buffers[0];
        auto tab_color_buffer = tab_buffers.buffers[1];
        auto tab_index_buffer = tab_buffers.buffers[2];

        vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, tab_pipeline);

        VkBuffer vertex_buffers[] = { tab_position_buffer, tab_color_buffer };
        VkDeviceSize offsets[] = { 0, 0 };
        vkCmdBindVertexBuffers(command, 0, 2, vertex_buffers, offsets);

        float width = (float)m_width;
        float height = (float)m_height;
        float constants[] = { 2.0f/width, 2.0f/height };
        vkCmdPushConstants(command, tab_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, 2*sizeof(float), constants);

        vkCmdBindIndexBuffer(command, tab_index_buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(command, 6, 1, 0, 0, 0);
      }
    }

    for(auto panel_entity : pasives)
    {
      auto panel = m_registry.get<ui::Panel>(panel_entity);
      auto tabs = panel.tabs;

      for(auto tab_entity : tabs)
      {
        auto tab_draw = m_registry.get<vk::Draw_Info>(tab_entity);
        auto tab_layout = tab_draw.layout;
        auto tab_pipeline = tab_draw.pipeline;

        auto tab_buffers = m_registry.get<vk::Buffer_Info>(tab_entity);
        auto tab_position_buffer = tab_buffers.buffers[0];
        auto tab_color_buffer = tab_buffers.buffers[1];
        auto tab_index_buffer = tab_buffers.buffers[2];

        vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, tab_pipeline);

        VkBuffer vertex_buffers[] = { tab_position_buffer, tab_color_buffer };
        VkDeviceSize offsets[] = { 0, 0 };
        vkCmdBindVertexBuffers(command, 0, 2, vertex_buffers, offsets);

        float width = (float)m_width;
        float height = (float)m_height;
        float constants[] = { 2.0f/width, 2.0f/height };
        vkCmdPushConstants(command, tab_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, 2*sizeof(float), constants);

        vkCmdBindIndexBuffer(command, tab_index_buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(command, 6, 1, 0, 0, 0);
      }
    }

    for(auto panel_entity : actives)
    {
      auto panel = m_registry.get<ui::Panel>(panel_entity);
      auto tabs = panel.tabs;

      auto panel_draw = m_registry.get<vk::Draw_Info>(panel_entity);
      auto layout = panel_draw.layout;
      auto pipeline = panel_draw.pipeline;

      auto buffer_info = m_registry.get<vk::Buffer_Info>(panel_entity);
      auto position_buffer = buffer_info.buffers[0];
      auto color_buffer = buffer_info.buffers[1];
      auto index_buffer = buffer_info.buffers[2];

      vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

      VkBuffer vertex_buffers[] = { position_buffer, color_buffer };
      VkDeviceSize offsets[] = { 0, 0 };
      vkCmdBindVertexBuffers(command, 0, 2, vertex_buffers, offsets);

      float width = (float)m_width;
      float height = (float)m_height;      
      float constants[] = { 2.0f/width, 2.0f/height };
      vkCmdPushConstants(command, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, 2*sizeof(float), constants);

      vkCmdBindIndexBuffer(command, index_buffer, 0, VK_INDEX_TYPE_UINT32);

      vkCmdDrawIndexed(command, 6, 1, 0, 0, 0);

      for(auto tab_entity : tabs)
      {
        auto tab_draw = m_registry.get<vk::Draw_Info>(tab_entity);
        auto tab_layout = tab_draw.layout;
        auto tab_pipeline = tab_draw.pipeline;

        auto tab_buffers = m_registry.get<vk::Buffer_Info>(tab_entity);
        auto tab_position_buffer = tab_buffers.buffers[0];
        auto tab_color_buffer = tab_buffers.buffers[1];
        auto tab_index_buffer = tab_buffers.buffers[2];

        vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, tab_pipeline);

        VkBuffer vertex_buffers[] = { tab_position_buffer, tab_color_buffer };
        VkDeviceSize offsets[] = { 0, 0 };
        vkCmdBindVertexBuffers(command, 0, 2, vertex_buffers, offsets);

        float width = (float)m_width;
        float height = (float)m_height;
        float constants[] = { 2.0f/width, 2.0f/height };
        vkCmdPushConstants(command, tab_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, 2*sizeof(float), constants);

        vkCmdBindIndexBuffer(command, tab_index_buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(command, 6, 1, 0, 0, 0);
      }
    }

    /* --- draw the buttons --- */
    auto button_view = m_registry.view<Button>();
    for(auto button_entity : button_view)
    {
      auto draw_info = m_registry.get<vk::Draw_Info>(button_entity);
      auto layout = draw_info.layout;
      auto pipeline = draw_info.pipeline;

      auto buffer_info = m_registry.get<vk::Buffer_Info>(button_entity);
      auto position_buffer = buffer_info.buffers[0];
      auto color_buffer = buffer_info.buffers[1];
      auto index_buffer = buffer_info.buffers[2];

      vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

      VkBuffer vertex_buffers[] = { position_buffer, color_buffer };
      VkDeviceSize offsets[] = { 0, 0 };
      vkCmdBindVertexBuffers(command, 0, 2, vertex_buffers, offsets);

      float width = (float)m_width;
      float height = (float)m_height;      
      float constants[] = { 2.0f/width, 2.0f/height };
      vkCmdPushConstants(command, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, 2*sizeof(float), constants);

      vkCmdBindIndexBuffer(command, index_buffer, 0, VK_INDEX_TYPE_UINT32);

      vkCmdDrawIndexed(command, 6, 1, 0, 0, 0);

      std::array<float, 3> color = {1.0f, 1.0f, 1.0f};
      auto button = m_registry.get<ui::Button>(button_entity);
      auto label = m_registry.get<ui::Label>(button_entity);
    }

  vkCmdEndRendering(command);

  VkImageMemoryBarrier output_image_barrier = {};
  output_image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  output_image_barrier.pNext = nullptr;
  output_image_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  output_image_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  output_image_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  output_image_barrier.image = m_images[index];
  output_image_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  output_image_barrier.subresourceRange.baseMipLevel = 0;
  output_image_barrier.subresourceRange.levelCount = 1;
  output_image_barrier.subresourceRange.baseArrayLayer = 0;
  output_image_barrier.subresourceRange.layerCount = 1;

  vkCmdPipelineBarrier(command, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &output_image_barrier);

  result = vkEndCommandBuffer(command);
  if(result != VK_SUCCESS)
  {
    std::cout << "The quads command buffer could not be ended. Error Code: " << result << std::endl;
    throw;
  }

  return command;
}