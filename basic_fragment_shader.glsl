#version 400

in vec3 transit_colour;
out vec4 out_colour;

void main () {
  out_colour = vec4( transit_colour, 1.0 );
};