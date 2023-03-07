#version 460

layout(location = 0) out vec2 coordinates;

vec2 positions[6] = {
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),

    vec2(-1.0,  1.0),
    vec2( 1.0, -1.0),
    vec2( 1.0,  1.0)
};

vec2 tex_coords[6] = {
    vec2(0, 1),
    vec2(1, 1),
    vec2(0, 0),
    
    vec2(0, 0),
    vec2(1, 1),
    vec2(1, 0)
};

void main() 
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    coordinates = tex_coords[gl_VertexIndex];
}