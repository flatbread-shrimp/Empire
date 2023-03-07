#include "../include/font.hpp"
#include "../include/shader.hpp"

Library* create_library(float height, VkDevice device, VmaAllocator& allocator)
{
  Library* library = new Library;

  float max_size, avrage_size, min_size;
  min_size = 0.025f*height;
  avrage_size = 0.05f*height;
  max_size = 0.075f*height;
  


  library->device = device;
  library->allocator = allocator;
  return library;
}

void destroy_library(Library* library)
{
  delete library;
}

void render_text(std::string, float, Library*)
{

}