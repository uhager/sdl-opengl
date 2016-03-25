#version 400

in vec3 vertex_position;
uniform vec3 vertex_colour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 transit_colour;

void main () {
     transit_colour = vertex_colour;	
     gl_Position = projection * view * model * vec4(vertex_position, 1.0) ;
};