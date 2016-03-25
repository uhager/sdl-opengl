#version 400

in vec2 transit_uv;
uniform sampler2D texture_sampler;
uniform float colour_intensity;

out vec4 out_colour;

void main () {
  vec3 temp_colour = texture(texture_sampler, transit_uv).rgb;
  out_colour = vec4( temp_colour.r * colour_intensity, temp_colour.g * colour_intensity, temp_colour.b * colour_intensity, 1.0);
};