#pragma once

#include "headers.hpp"

struct Glyph
{

};

struct Font
{
  float size;
  std::map<char, Glyph> glyphs;
};

struct Library
{
  std::vector<Font> fonts;

  VkDevice device;
  VmaAllocator allocator;
};

Library* create_library(float, VkDevice, VmaAllocator&);
void destroy_library(Library*);

void render_text(std::string, float, Library*);