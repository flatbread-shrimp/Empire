#version 460

layout(location = 0) in vec2 coordinates;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform sampler2D color_map;

void main()
{
    out_color = texture(color_map, coordinates);
}