#version 460

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec3 frag_color;

layout(push_constant) uniform Push {
  float width;
  float height;
} push;

void main() 
{
  mat2 coord_transform = mat2(push.width, 0.0, 0.0, push.height);
  vec2 position = coord_transform*in_position - vec2(1.0, 1.0);

  gl_Position = vec4(position, 0.0, 1.0);
  frag_color = in_color;
}