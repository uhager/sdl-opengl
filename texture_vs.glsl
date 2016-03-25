#version 400

in vec3 vertex_position;
in vec2 vertex_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 transit_uv;

void main () {
     transit_uv = vertex_uv;	
     gl_Position = projection * view * model * vec4(vertex_position, 1.0) ;
};