#include "../include/shader.hpp"
#include "../include/macros.hpp"

VkShaderModule read_shader(std::string& path, VkDevice device)
{
  #ifdef VK_DEBUG
    std::string header = "C:/Users/logan/OneDrive/Desktop/Empire/Shaders";
  #else
    std::string header = "Shaders";
  #endif
  std::string full_path = header + "/" + path;

  std::ifstream file(full_path, std::ios::ate | std::ios::binary);
  if(!file.is_open())
  {
    std::cout << "The shader file faild to open." << std::endl;
    throw;
  }

  size_t file_size = (size_t) file.tellg();
  std::vector<char> buffer(file_size);

  file.seekg(0);
  file.read(buffer.data(), file_size);
  file.close();

  VkShaderModuleCreateInfo shader_module_create_info = {};
  shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_module_create_info.pNext = nullptr;
  shader_module_create_info.flags = 0;
  shader_module_create_info.codeSize = buffer.size();
  shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

  VkShaderModule shader_module = VK_NULL_HANDLE;
  auto result = vkCreateShaderModule(device, &shader_module_create_info, nullptr, &shader_module);
  if(result != VK_SUCCESS)
  {
    std::cout << "The shader module could not be created. Error code: " << result << std::endl;
    throw;
  }

  return shader_module;
}