#version 330 core

layout (location = 0) in vec2 vtx_position;
layout (location = 1) in vec3 vtx_tex_coord;
layout (location = 2) in vec4 vtx_color;
layout (location = 3) in vec4 vtx_grd_color;

layout (location = 4) in vec3 vtx_color_offset_is_positive;
layout (location = 5) in vec3 vtx_color_offset;

out vec3 frg_tex_coord;
out vec4 frg_color;
out vec4 frg_grd_color;
out vec3 frg_color_offset_is_positive;
out vec3 frg_color_offset;

uniform mat4 proj_matrix;

void main() {
    gl_Position     = proj_matrix * vec4(vtx_position, 0.0, 1.0);
    frg_tex_coord   = vec3(vtx_tex_coord);
    frg_color       = vtx_color;
    frg_grd_color   = vtx_grd_color;
    frg_color_offset_is_positive = vtx_color_offset_is_positive;
    frg_color_offset= vtx_color_offset;
}