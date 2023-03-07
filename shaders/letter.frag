#version 460

layout(location = 0) in vec3 out_color;
layout(location = 1) in vec2 out_texture_coords;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D texture_sampler; 

void main()
{
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(texture_sampler, out_texture_coords).r);
  color = vec4(out_color, 1.0)*sampled;
}