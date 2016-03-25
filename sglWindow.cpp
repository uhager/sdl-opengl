/// sglWindow.cpp
/// Open an SDL2 window for use with OpenGL
/// author: Ulrike Hager

#include <iostream>
#include <memory>
#include <string>
#include <sstream>

#include <GL/glew.h>

#include <SDL2/SDL.h>
//#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "sgl-helper.h"
#include "sglWindow.h"


sglWindow::sglWindow(std::string name, int width, int height)
  : width_(width), height_(height)
{
  
  window_ = std::unique_ptr<SDL_Window, DeleteWindow>( SDL_CreateWindow( name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)
					 , DeleteWindow() );
  if( window_ == nullptr ){
    std::cerr << "Could not create window. SDL_Error: " <<  SDL_GetError()  << std::endl;
    exit(1);
  }
  
  SDL_GLContext con = SDL_GL_CreateContext(window_.get());
  if ( !con ) {
    std::string error = sdl_error("Failed to create GL context");
    throw std::runtime_error( error );
  }
  gl_context_ = std::unique_ptr<SDL_GLContext, DeleteContext>(&con, DeleteContext() );
  
  const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
  const GLubyte* version = glGetString (GL_VERSION); // version as a string
  std::cout << "Renderer: " << renderer << std::endl;
  std::cout << "OpenGL version supported "<< version << std::endl;
 
  glewExperimental = GL_TRUE;
  GLenum glewini = glewInit();
  
  if ( glewini != GLEW_OK) {
    std::stringstream strstr;
    strstr << "Failed to initialize GLEW: " << glewGetErrorString(glewini) ;
    throw std::runtime_error( strstr.str()  ) ;
  }

  SDL_GL_SetSwapInterval(1);
  glClearColor(0.08f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  swap();
  
  glEnable (GL_DEPTH_TEST); 
  glDepthFunc (GL_LESS); 
}


