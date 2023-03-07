#version 460

layout(location = 0) in vec2 raw_position;
layout(location = 1) in vec2 raw_texture_coords;
layout(location = 2) in vec3 raw_color;

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_texture_coords;

layout(push_constant) uniform Push {
  float width;
  float height;
} push;

void main()
{
  mat2 transform = mat2(push.width, 0.0, 0.0, push.height);
  gl_Position = vec4(transform*raw_position, 0.0, 1.0);

  out_color = raw_color;
  out_texture_coords = raw_texture_coords;
}