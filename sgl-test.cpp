/// sgl-test.h
/// Demo for testing sgl
/// author: Ulrike Hager

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <chrono>

#include <GL/glew.h>

#include <SDL2/SDL.h>
//#include <SDL_opengl.h>
#include <SDL_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sgl-helper.h"
#include "sglWindow.h"

const int width = 1024;
const int height = 768;


void run()
{
  sglWindow window("SGL Test", width, height);
  glClearColor(0.08f, 0.3f, 0.04f, 1.0f);
  GLfloat camera_radius = sqrt( 12*12 + 10*10 );  // x^2+z^2

  std::vector<glm::vec3> vertices, normals;
  std::vector<glm::vec2> uvs;
  load_blender_obj("resources/mushroom.obj", vertices, uvs, normals);
  GLuint texture = load_texture("resources/mushroom.png");

  GLfloat floor_vertices[] = {
    -6.0f, 0.0f, -6.0f,
    6.0f, 0.0f, -6.0f,
    -6.0f, 0.0f,  6.0f,

    6.0f, 0.0f, -6.0f,
    -6.0f, 0.0f,  6.0f,
    6.0f, 0.0f,  6.0f,
  };
  
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray (vao);

  std::vector<GLuint> vertex_buffers(3);
  glGenBuffers (vertex_buffers.size(), &vertex_buffers[0]);

  glBindBuffer (GL_ARRAY_BUFFER, vertex_buffers[0]);
  glBufferData (GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

  glBindBuffer (GL_ARRAY_BUFFER, vertex_buffers[1]);
  glBufferData (GL_ARRAY_BUFFER,  uvs.size()*sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  glBindBuffer (GL_ARRAY_BUFFER, vertex_buffers[2]);
  glBufferData (GL_ARRAY_BUFFER,  sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);
  
  GLuint floor_shader = program_from_shaderfiles( "basic_vertex_shader.glsl" , "basic_fragment_shader.glsl" );
  GLuint texture_shader = program_from_shaderfiles( "texture_vs.glsl" , "texture_fs.glsl" );

  GLuint model_uniform = bind_uniform(texture_shader, "model");
  GLuint view_uniform = bind_uniform(texture_shader, "view");
  GLuint projection_uniform = bind_uniform(texture_shader, "projection");

  GLuint position_attrib = bind_attribute(texture_shader, "vertex_position");
  GLuint uv_attrib = bind_attribute(texture_shader, "vertex_uv");
  GLuint texture_sampler = bind_uniform( texture_shader, "texture_sampler");
  GLuint colour_intensity = bind_uniform(texture_shader, "colour_intensity");

  GLuint floor_colour_uniform = bind_uniform( floor_shader, "vertex_colour" );
  GLuint floor_position_attrib = bind_attribute(floor_shader, "vertex_position");
  GLuint floor_model_uniform = bind_uniform(floor_shader, "model");
  GLuint floor_view_uniform = bind_uniform(floor_shader, "view");
  GLuint floor_projection_uniform = bind_uniform(floor_shader, "projection");

  glm::mat4 projection_matrix = glm::perspective(glm::radians(40.0f), (float) width / (float)height, 0.1f, 50.0f);
  glm::mat4 model_matrix = glm::translate( glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, -1.0f));    // glm::mat4(1.0f);  // identity, model at origin
  glm::mat4 translate_second = glm::translate( glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, 3.5f));
  glm::mat4 rotate_second = glm::rotate( glm::mat4(1.0f), 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 scale_second = glm::scale( glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.7f) );
  glm::mat4 model_matrix_second = translate_second * scale_second * rotate_second; 
  glm::mat4 model_matrix_floor = glm::mat4(1.0f);  // identity, model at origin

  SDL_Event event;
  bool quit = false;
  
  auto start_time = std::chrono::high_resolution_clock::now();
  while (!quit) {
    while( SDL_PollEvent( &event ) ) {
      if (event.type == SDL_QUIT) quit = true;
      else if (event.type == SDL_KEYDOWN ) {
	switch ( event.key.keysym.sym ) {
	case SDLK_ESCAPE:
	  quit = true;
	  break;
	}
      }
    }
      
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto current_time = std::chrono::high_resolution_clock::now();  
    float time = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - start_time).count() ;
    GLfloat x = cos(time/2.0) ;
    GLfloat z = sin(time/2.0);
    x *= camera_radius;
    z *= camera_radius;
    glm::mat4 view_matrix = glm::lookAt( glm::vec3(x,3,z), glm::vec3(0,3,0), glm::vec3(0,1,0) );


    /// Draw mushrooms ///
    glUseProgram(texture_shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(texture_sampler, 0);
    glUniform1f(colour_intensity, 1.0);

    glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view_matrix) );
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection_matrix) );
 
    glEnableVertexAttribArray ( position_attrib );
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffers[0]);
    glVertexAttribPointer ( position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0 );

    glEnableVertexAttribArray ( uv_attrib );
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffers[1]);
    glVertexAttribPointer ( uv_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0 );

    /// first
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_matrix) );
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    
    /// second
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_matrix_second) );
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()); 

    glDisableVertexAttribArray(position_attrib);
    glDisableVertexAttribArray(uv_attrib);

    /// Draw floor  ///
    glUseProgram(floor_shader);
    glUniformMatrix4fv(floor_view_uniform, 1, GL_FALSE, glm::value_ptr(view_matrix) );
    glUniformMatrix4fv(floor_projection_uniform, 1, GL_FALSE, glm::value_ptr(projection_matrix) );
    glUniformMatrix4fv(floor_model_uniform, 1, GL_FALSE, glm::value_ptr(model_matrix_floor) );
    glUniform3f( floor_colour_uniform, 0.1f, 0.02f, 0.1f );

    glEnableVertexAttribArray ( floor_position_attrib );
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffers[2]);
    glVertexAttribPointer( floor_position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  
    glEnable(GL_STENCIL_TEST); 
    glDepthMask(GL_FALSE);  
    glStencilFunc(GL_ALWAYS, 1, 0xFF); 
    glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE ); 
    glStencilMask(0xFF); 
    glClear(GL_STENCIL_BUFFER_BIT); 
    glDrawArrays(GL_TRIANGLES, 0, sizeof(floor_vertices) );
    glStencilFunc(GL_EQUAL, 1, 0xFF); 
    glStencilMask(0x00); 
    glDepthMask(GL_TRUE);

    glDisableVertexAttribArray(floor_position_attrib);

    ///  Draw reflections  ///
    glUseProgram(texture_shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(texture_sampler, 0);
    glUniform1f(colour_intensity, 0.05);

    glEnableVertexAttribArray ( position_attrib );
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffers[0]);
    glVertexAttribPointer ( position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0 );

    glEnableVertexAttribArray ( uv_attrib );
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffers[1]);
    glVertexAttribPointer ( uv_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    // first
    glm::mat4 refl_matrix = glm::scale( model_matrix, glm::vec3(1, -1, 1) );
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(refl_matrix) );
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    // second
    refl_matrix = glm::scale( model_matrix_second, glm::vec3(1, -1, 1) );
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr( refl_matrix) );
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisable(GL_STENCIL_TEST);

    window.swap();
  }
	
  glDeleteTextures(1, &texture);
  glDeleteBuffers(1, &vertex_buffers[0]);
  glDeleteVertexArrays(1, &vao);
  glDeleteProgram(texture_shader);
  glDeleteProgram(floor_shader);
}


int main( void )
{
 
  try {
    sdl_init();
    run();
  }
  catch (const std::exception& except) {
    std::cerr << except.what() << std::endl;
  }
  
  sdl_quit();
  return 0;
}

