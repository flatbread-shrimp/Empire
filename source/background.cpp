#include "../include/background.hpp"
#include "../include/shader.hpp"

namespace background
{
  vk::Draw_Info create_quad_draw_info(std::uint32_t image_count, VkDevice device)
  {
    vk::Draw_Info info = {};

    std::string vertex_path = "quad_vert.spv";
    VkShaderModule vertex_module = read_shader(vertex_path, device);

    VkPipelineShaderStageCreateInfo vertex_stage_create_info = {};
    vertex_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_stage_create_info.pNext = nullptr;
    vertex_stage_create_info.flags = 0;
    vertex_stage_create_info.module = vertex_module;
    vertex_stage_create_info.pName = "main";
    vertex_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_stage_create_info.pSpecializationInfo = nullptr;

    std::string fragment_path = "quad_frag.spv";
    VkShaderModule fragment_module = read_shader(fragment_path, device);

    VkPipelineShaderStageCreateInfo fragment_stage_create_info = {};
    fragment_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_stage_create_info.pNext = nullptr;
    fragment_stage_create_info.flags = 0;
    fragment_stage_create_info.module = fragment_module;
    fragment_stage_create_info.pName = "main";
    fragment_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_stage_create_info.pSpecializationInfo = nullptr;

    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos = { vertex_stage_create_info, fragment_stage_create_info };

    std::vector<VkDynamicState> dynamic_states = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
    dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_create_info.pNext = nullptr;
    dynamic_state_create_info.flags = 0;
    dynamic_state_create_info.dynamicStateCount = dynamic_states.size();
    dynamic_state_create_info.pDynamicStates = dynamic_states.data();

    VkPipelineVertexInputStateCreateInfo vertex_input_create_info = {};
    vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_create_info.pNext = nullptr;
    vertex_input_create_info.flags = 0;
    vertex_input_create_info.vertexBindingDescriptionCount = 0;
    vertex_input_create_info.pVertexBindingDescriptions = nullptr;
    vertex_input_create_info.vertexAttributeDescriptionCount = 0;
    vertex_input_create_info.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = {};
    input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_create_info.pNext = nullptr;
    input_assembly_create_info.flags = 0;
    input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
    viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_create_info.pNext = nullptr;
    viewport_state_create_info.flags = 0;
    viewport_state_create_info.scissorCount = 1;
    viewport_state_create_info.viewportCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer_state_create_info = {};
    rasterizer_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_state_create_info.pNext = nullptr;
    rasterizer_state_create_info.flags = 0;
    rasterizer_state_create_info.depthClampEnable = VK_FALSE;
    rasterizer_state_create_info.rasterizerDiscardEnable = VK_FALSE;
    rasterizer_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer_state_create_info.lineWidth = 1.0f;
    rasterizer_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer_state_create_info.depthBiasEnable = VK_FALSE;
    rasterizer_state_create_info.depthBiasConstantFactor = 0.0f;
    rasterizer_state_create_info.depthBiasClamp = 0.0f;
    rasterizer_state_create_info.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling_state_create_info = {};
    multisampling_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_state_create_info.pNext = nullptr;
    multisampling_state_create_info.flags = 0;
    multisampling_state_create_info.sampleShadingEnable = VK_FALSE;
    multisampling_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_state_create_info.minSampleShading = 1.0f;
    multisampling_state_create_info.pSampleMask = nullptr;
    multisampling_state_create_info.alphaToCoverageEnable = VK_FALSE;
    multisampling_state_create_info.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending_create_info = {};
    color_blending_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending_create_info.logicOpEnable = VK_FALSE;
    color_blending_create_info.logicOp = VK_LOGIC_OP_COPY;
    color_blending_create_info.attachmentCount = 1;
    color_blending_create_info.pAttachments = &color_blend_attachment;
    color_blending_create_info.blendConstants[0] = 0.0f;
    color_blending_create_info.blendConstants[1] = 0.0f;
    color_blending_create_info.blendConstants[2] = 0.0f;
    color_blending_create_info.blendConstants[3] = 0.0f;

    VkDescriptorSetLayoutBinding graphics_binding = {};
    graphics_binding.binding = 0;
    graphics_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    graphics_binding.descriptorCount = 1;
    graphics_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    graphics_binding.pImmutableSamplers = nullptr;

    info.descriptor_set_layouts.resize(image_count);
    for(std::uint32_t i = 0; i < image_count; i++)
    {
      VkDescriptorSetLayoutCreateInfo compute_set_layout_create_info = {};
      compute_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      compute_set_layout_create_info.pNext = nullptr;
      compute_set_layout_create_info.bindingCount = 1;
      compute_set_layout_create_info.pBindings = &graphics_binding;
    
      auto result = vkCreateDescriptorSetLayout(device, &compute_set_layout_create_info, nullptr, &info.descriptor_set_layouts[i]);
      if(result != VK_SUCCESS)
      {
        std::cout << "The quad descriptor set could not be created. Error Code: " << result << std::endl;
        throw;
      }

      VkDescriptorSetLayoutCreateInfo graphics_set_layout_create_info = {};
      graphics_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      graphics_set_layout_create_info.pNext = nullptr;
      graphics_set_layout_create_info.bindingCount = 1;
      graphics_set_layout_create_info.pBindings = &graphics_binding;
    
      result = vkCreateDescriptorSetLayout(device, &graphics_set_layout_create_info, nullptr, &info.descriptor_set_layouts[image_count + i]);
      if(result != VK_SUCCESS)
      {
        std::cout << "The quad descriptor set could not be created. Error Code: " << result << std::endl;
        throw;
      }
    }

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = nullptr;
    pipeline_layout_create_info.flags = 0;
    pipeline_layout_create_info.setLayoutCount = 1;
    pipeline_layout_create_info.pSetLayouts = &info.descriptor_set_layouts[0];
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = nullptr;

    auto result = vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &info.layout);
    if(result != VK_SUCCESS)
    {
      std::cout << "The background quads pipeline layout could not be created. Error Code: " << result << std::endl;
      throw;
    }

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
    graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_create_info.pNext = nullptr;
    graphics_pipeline_create_info.flags = 0;
    graphics_pipeline_create_info.stageCount = shader_stage_create_infos.size();
    graphics_pipeline_create_info.pStages = shader_stage_create_infos.data();
    graphics_pipeline_create_info.pVertexInputState = &vertex_input_create_info;
    graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
    graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
    graphics_pipeline_create_info.pRasterizationState = &rasterizer_state_create_info;
    graphics_pipeline_create_info.pMultisampleState = &multisampling_state_create_info;
    graphics_pipeline_create_info.pDepthStencilState = nullptr;
    graphics_pipeline_create_info.pColorBlendState = &color_blending_create_info;
    graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
    graphics_pipeline_create_info.layout = info.layout;
    graphics_pipeline_create_info.renderPass = VK_NULL_HANDLE;
    graphics_pipeline_create_info.subpass = 0;
    graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_create_info.basePipelineIndex = -1;

    result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &info.pipeline);
    if(result != VK_SUCCESS)
    {
      std::cout << "The background quads graphics pipeline could not be created. Error Code: " << result << std::endl;
      throw;
    }

    vkDestroyShaderModule(device, vertex_module, nullptr);
    vkDestroyShaderModule(device, fragment_module, nullptr);

    info.device = device;
    return info;
  }

  vk::Texture_Info create_texture_info(std::uint32_t image_count, std::uint32_t width, std::uint32_t height, VkQueue queue, VkCommandPool pool, VkDevice device, VmaAllocator allocator, vk::Draw_Info& draw_info, vk::Compute_Info& compute_info)
  {
    vk::Texture_Info info = {};
    info.memory.resize(image_count);
    info.textures.resize(image_count);
    info.views.resize(image_count);

    for(std::uint32_t i=0; i<image_count; i++)
    {
      VkImageCreateInfo image_create_info = {};
      image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      image_create_info.pNext = nullptr;
      image_create_info.flags = 0;
      image_create_info.extent.width = width;
      image_create_info.extent.height = height;
      image_create_info.extent.depth = 1;
      image_create_info.imageType = VK_IMAGE_TYPE_2D;
      image_create_info.arrayLayers = 1;
      image_create_info.format = VK_FORMAT_R32G32B32A32_SFLOAT;
      image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      image_create_info.mipLevels = 1;
      image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
      image_create_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

      VmaAllocationCreateInfo allocation_create_info = {};
      allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;

      auto result = vmaCreateImage(allocator, &image_create_info, &allocation_create_info, &info.textures[i], &info.memory[i], nullptr);
      if(result != VK_SUCCESS)
      {
        std::cout << "Error creating the image and allocating it's memory. Error Code: " << result << std::endl;
        throw;
      }

      VkImageViewCreateInfo view_create_info = {};
      view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      view_create_info.pNext = nullptr;
      view_create_info.flags = 0;
      view_create_info.image = info.textures[i];
      view_create_info.format = VK_FORMAT_R32G32B32A32_SFLOAT;
      view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
      view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      view_create_info.subresourceRange.baseMipLevel = 0;
      view_create_info.subresourceRange.levelCount = 1;
      view_create_info.subresourceRange.baseArrayLayer = 0;
      view_create_info.subresourceRange.layerCount = 1;

      result = vkCreateImageView(device, &view_create_info, nullptr, &info.views[i]);
      if(result != VK_SUCCESS)
      {
        std::cout << "The image view could not be created. Error Code: " << result << std::endl;
        throw;
      }
    }

    VkSamplerCreateInfo sampler_create_info = {};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.pNext = nullptr;
    sampler_create_info.flags = 0;
    sampler_create_info.magFilter = VK_FILTER_LINEAR;
    sampler_create_info.minFilter = VK_FILTER_LINEAR;
    sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    auto result = vkCreateSampler(device, &sampler_create_info, nullptr, &info.sampler);
    if(result != VK_SUCCESS)
    {
      std::cout << "Error creating sampler. Error Code: " << result << std::endl;
      throw;
    }

    info.pools.resize(2);
    VkDescriptorPoolSize compute_pool_size = {};
    compute_pool_size.descriptorCount = image_count;
    compute_pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

    VkDescriptorPoolCreateInfo compute_pool_create_info = {};
    compute_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    compute_pool_create_info.pNext = nullptr;
    compute_pool_create_info.flags = 0;
    compute_pool_create_info.poolSizeCount = 1;
    compute_pool_create_info.pPoolSizes = &compute_pool_size;
    compute_pool_create_info.maxSets = image_count;

    result = vkCreateDescriptorPool(device, &compute_pool_create_info, nullptr, &info.pools[0]);
    if(result != VK_SUCCESS)
    {
      std::cout << "Error creating descriptor pool. Error Code: " << result << std::endl;
      throw;
    }

    VkDescriptorPoolSize graphics_pool_size = {};
    graphics_pool_size.descriptorCount = image_count;
    graphics_pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    VkDescriptorPoolCreateInfo graphics_pool_create_info = {};
    graphics_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    graphics_pool_create_info.pNext = nullptr;
    graphics_pool_create_info.flags = 0;
    graphics_pool_create_info.poolSizeCount = 1;
    graphics_pool_create_info.pPoolSizes = &graphics_pool_size;
    graphics_pool_create_info.maxSets = image_count;

    result = vkCreateDescriptorPool(device, &graphics_pool_create_info, nullptr, &info.pools[1]);
    if(result != VK_SUCCESS)
    {
      std::cout << "Error creating descriptor pool. Error Code: " << result << std::endl;
      throw;
    }

    info.compute_sets.resize(image_count);
    VkDescriptorSetAllocateInfo compute_set_allocate_info = {};
    compute_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    compute_set_allocate_info.pNext = nullptr;
    compute_set_allocate_info.descriptorPool = info.pools[0];
    compute_set_allocate_info.descriptorSetCount = image_count;
    compute_set_allocate_info.pSetLayouts = compute_info.descriptor_set_layouts.data();

    result = vkAllocateDescriptorSets(device, &compute_set_allocate_info, info.compute_sets.data());
    if(result != VK_SUCCESS)
    {
      std::cout << "The descriptor set allocation faild. Error Code: " << result << std::endl;
      throw;
    }

    info.graphics_sets.resize(image_count);
    VkDescriptorSetAllocateInfo graphics_set_allocate_info = {};
    graphics_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    graphics_set_allocate_info.pNext = nullptr;
    graphics_set_allocate_info.descriptorPool = info.pools[1];
    graphics_set_allocate_info.descriptorSetCount = image_count;
    graphics_set_allocate_info.pSetLayouts = draw_info.descriptor_set_layouts.data();

    result = vkAllocateDescriptorSets(device, &graphics_set_allocate_info, info.graphics_sets.data());
    if(result != VK_SUCCESS)
    {
      std::cout << "The descriptor set allocation faild. Error Code: " << result << std::endl;
      throw;
    }

    for(std::uint32_t i=0; i<image_count; i++)
    {
      VkDescriptorImageInfo compute_image_info = {};
      compute_image_info.sampler = info.sampler;
      compute_image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
      compute_image_info.imageView = info.views[i];

      VkWriteDescriptorSet compute_write = {};
      compute_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      compute_write.pNext = nullptr;
      compute_write.descriptorCount = 1;
      compute_write.dstArrayElement = 0;
      compute_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      compute_write.dstBinding = 0;
      compute_write.dstSet = info.compute_sets[i];
      compute_write.pImageInfo = &compute_image_info;
      compute_write.pBufferInfo = nullptr;
      compute_write.pTexelBufferView = nullptr;

      vkUpdateDescriptorSets(device, 1, &compute_write, 0, nullptr);

      VkDescriptorImageInfo graphics_image_info = {};
      graphics_image_info.sampler = info.sampler;
      graphics_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      graphics_image_info.imageView = info.views[i];

      VkWriteDescriptorSet graphics_write = {};
      graphics_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      graphics_write.pNext = nullptr;
      graphics_write.descriptorCount = 1;
      graphics_write.dstArrayElement = 0;
      graphics_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      graphics_write.dstBinding = 0;
      graphics_write.dstSet = info.graphics_sets[i];
      graphics_write.pImageInfo = &graphics_image_info;
      graphics_write.pBufferInfo = nullptr;
      graphics_write.pTexelBufferView = nullptr;

      vkUpdateDescriptorSets(device, 1, &graphics_write, 0, nullptr);

      transfer_image_between_layouts(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i, info, queue, pool, device);
    }

    info.allocator = allocator;
    info.device = device;
    return info;
  }

  vk::Compute_Info create_texture_compute_info(std::uint32_t image_count, VkDevice device)
  {
    vk::Compute_Info info = {};

    std::string compute_path = "render_comp.spv";
    VkShaderModule compute_module = read_shader(compute_path, device);

    VkDescriptorSetLayoutBinding storage_binding = {};
    storage_binding.binding = 0;
    storage_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    storage_binding.descriptorCount = 1;
    storage_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    storage_binding.pImmutableSamplers = nullptr;

    info.descriptor_set_layouts.resize(image_count);
    for(std::uint32_t i=0; i<image_count; i++)
    {
      VkDescriptorSetLayoutCreateInfo set_layout_create_info = {};
      set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      set_layout_create_info.pNext = nullptr;
      set_layout_create_info.flags = 0;
      set_layout_create_info.bindingCount = 1;
      set_layout_create_info.pBindings = &storage_binding;

      auto result = vkCreateDescriptorSetLayout(device, &set_layout_create_info, nullptr, &info.descriptor_set_layouts[i]);
      if(result != VK_SUCCESS)
      {
        std::cout << "The descriptor set layout could not be created. Error Code: " << result << std::endl;
        throw;
      }
    }

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = nullptr;
    pipeline_layout_create_info.flags = 0;
    pipeline_layout_create_info.pPushConstantRanges = nullptr;
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.setLayoutCount = info.descriptor_set_layouts.size();
    pipeline_layout_create_info.pSetLayouts = info.descriptor_set_layouts.data();

    auto result  = vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &info.layout);
    if(result != VK_SUCCESS)
    { 
      std::cout << "Error creating pipeline layout. Error code: " << result << std::endl;
      throw;
    }

    VkPipelineShaderStageCreateInfo compute_shader_stage_info = {};
    compute_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    compute_shader_stage_info.pNext = nullptr;
    compute_shader_stage_info.flags = 0;
    compute_shader_stage_info.pName = "main";
    compute_shader_stage_info.pSpecializationInfo = nullptr;
    compute_shader_stage_info.module = compute_module;
    compute_shader_stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;

    VkComputePipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_create_info.pNext = nullptr;
    pipeline_create_info.flags = 0;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex = 0;
    pipeline_create_info.layout = info.layout;
    pipeline_create_info.stage = compute_shader_stage_info;

    result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &info.pipeline);
    if(result != VK_SUCCESS)
    {
      std::cout << "The texture compute pipeline could not be created. Error Code: " << result << std::endl; 
      throw;
    }

    vkDestroyShaderModule(device, compute_module, nullptr);

    info.device = device;
    return info;
  }

  void transfer_image_between_layouts(VkImageLayout _old, VkImageLayout _new, std::uint32_t index, vk::Texture_Info& info, VkQueue queue, VkCommandPool pool, VkDevice device)
  {
    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = nullptr;
    fence_create_info.flags = 0;

    VkFence fence = VK_NULL_HANDLE;
    auto result = vkCreateFence(device, &fence_create_info, nullptr, &fence);
    if(result != VK_SUCCESS)
    {
      std::cout << "The fence could not be created. Error Code: " << result << std::endl;
      throw;
    }

    VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.pNext = nullptr;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = 1;
    command_buffer_allocate_info.commandPool = pool;

    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    result = vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffer);
    if(result != VK_SUCCESS)
    {
      std::cout << "The command buffer could not be allocated. Error Code: " << result << std::endl;
      throw;
    }

    VkCommandBufferBeginInfo command_buffer_begin_info = {};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = nullptr;
    command_buffer_begin_info.flags = 0;
    command_buffer_begin_info.pInheritanceInfo = nullptr;

    result = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
    if(result != VK_SUCCESS)
    {
      std::cout << "The command bufffer could not be started. Error Code: " << result << std::endl;
      throw;
    }

    VkImageMemoryBarrier image_barrier = {};
    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_barrier.pNext = nullptr;
    image_barrier.image = info.textures[index];
    image_barrier.oldLayout = _old;
    image_barrier.newLayout = _new;
    image_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    image_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    image_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_barrier.subresourceRange.baseMipLevel = 0;
    image_barrier.subresourceRange.levelCount = 1;
    image_barrier.subresourceRange.baseArrayLayer = 0;
    image_barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0,nullptr, 1, &image_barrier);

    vkEndCommandBuffer(command_buffer);

    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_TRANSFER_BIT };
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = nullptr;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    submit_info.waitSemaphoreCount = 0;
    submit_info.pWaitSemaphores = nullptr;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.signalSemaphoreCount = 0;
    submit_info.pSignalSemaphores = nullptr;

    result = vkQueueSubmit(queue, 1, &submit_info, fence);
    if(result != VK_SUCCESS)
    {
      std::cout << "The queue submition faild. Error Code: " << result << std::endl;
      throw;
    }

    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkFreeCommandBuffers(device, pool, 1, &command_buffer);
    vkDestroyFence(device, fence, nullptr);
  }
}