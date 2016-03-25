/// sgl-helper.cpp
/// Collection of functions for common OpenGL + SDL2 tasks
/// author: Ulrike Hager

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <chrono>

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "sgl-helper.h"

GLuint load_shader(const std::string& file, GLenum type){
  GLint compile_result = GL_FALSE;
  int log_length = 0;
  
  std::ifstream input( file ,std::ifstream::in);
  if ( !input.is_open() ) 
    throw std::runtime_error("[shader_program] Couldn't open file " + file );
  
  std::string line = "", code = "";
  while ( getline(input,line) ) {
    code += "\n" + line;
  }
  input.close();

  GLuint shaderID = glCreateShader( type );

  const char* temp = code.c_str();
  glShaderSource (shaderID, 1, &temp, nullptr);
  glCompileShader (shaderID);

  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compile_result);
  glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &log_length);
  if ( log_length > 0 ){
    std::vector<char> error(log_length+1);
    glGetShaderInfoLog(shaderID, log_length, nullptr, &error[0]);
    std::cerr << &error[0] << std::endl;
  }

  return shaderID;
}


void check_program_compilation(GLuint program)
{
  GLint compile_result = GL_FALSE;
  int log_length = 0;
  
  glGetProgramiv(program, GL_LINK_STATUS, &compile_result);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
  if ( log_length > 0 ){
    std::vector<char> error(log_length+1);
    glGetProgramInfoLog(program, log_length, nullptr, &error[0]);
    std::cerr << &error[0] << std::endl;
  }
}


GLuint program_from_shaders(std::vector<GLuint> shaders)
{
  GLuint shader_program = glCreateProgram ();

  for (auto shader: shaders) {
    glAttachShader (shader_program, shader);
  }
 
  glLinkProgram (shader_program);
  check_program_compilation( shader_program );

  for (auto shader: shaders) {
    glDetachShader(shader_program, shader);
    glDeleteShader(shader);
  }

  return shader_program;
}


GLuint program_from_shaders(GLuint vertex_shader, GLuint fragment_shader)
{
  std::vector<GLuint> shaders{vertex_shader, fragment_shader};
  return program_from_shaders(shaders);
}


GLuint program_from_shaderfiles(const std::string& vertex_shader_file, const std::string& fragment_shader_file)
{
  std::vector<GLuint> shaders;
  std::vector<std::pair<std::string, GLenum> > shader_list = {{vertex_shader_file, GL_VERTEX_SHADER }, {fragment_shader_file, GL_FRAGMENT_SHADER}};

  for (auto shader: shader_list)   {
    GLuint shaderID = load_shader( shader.first, shader.second );
    shaders.push_back(shaderID);
  }
  return program_from_shaders(shaders);
}


GLuint bind_attribute(GLuint program, const std::string& attribute)
{
  GLint attr_id = glGetAttribLocation(program, attribute.c_str());
  if (attr_id < 0) {
    throw std::runtime_error( "Could not bind attribute " + attribute );
  }
  return (GLuint)attr_id;
}


GLuint bind_uniform(GLuint program, const std::string& uniform)
{
  GLint unif_id = glGetUniformLocation(program, uniform.c_str());
  if (unif_id < 0) {
    throw std::runtime_error( "Could not bind uniform " + uniform );
  }
  return (GLuint)unif_id;
}


////////////////////
//  blender obj   //
////////////////////
std::vector<uint32_t> read_faces(std::istringstream& stream)
{
  std::vector<uint32_t> result;
  std::string set;
  stream >> set;
  std::string item = "";
  std::istringstream iss(set);
  while ( getline(iss, item, '/' ) ) {
    result.push_back(std::stoul(item));
  }
  return result;
}


void load_blender_obj_ifstream(const std::string& file, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals)
{
  std::vector<uint32_t> indices_vertex, indices_uv, indices_normals;
  std::vector<glm::vec3> temp_vertices;
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;

  auto start_time = std::chrono::high_resolution_clock::now();

  std::ifstream input( file ,std::ifstream::in);
  if ( !input.is_open() ) 
    throw std::runtime_error("[load_blender_obj] Couldn't open file " + file );

  std::string line = "", token = "";;
  while ( getline(input,line) ) {
    std::istringstream stream(line);
    stream >> token;
    if ( token == "v" ) {    //vertices
      glm::vec3 vertex;
      stream >> vertex.x >> vertex.y >> vertex.z;
      temp_vertices.push_back(vertex);
    }
    else if ( token == "vt" ) {   // texture uv coordinates
      glm::vec2 uv;
      stream >> uv.x >> uv.y ;
      uv.y *= -1;
      temp_uvs.push_back(uv);
    }
    else if ( token == "vn" ) {  // normals
      glm::vec3 normal;
      stream >> normal.x >> normal.y >> normal.z;
      temp_normals.push_back(normal);
    }
    else if ( token == "f" ) {   // faces, connects indices
      for ( uint32_t i = 0 ; i < 3 ; ++i ) {
	std::vector<uint32_t> face = read_faces(stream);
	indices_vertex.push_back( face.at(0));
	indices_uv.push_back( face.at(1));
	indices_normals.push_back( face.at(2));
      }
    }  
  }
  input.close();

  auto current_time = std::chrono::high_resolution_clock::now();  
  float time = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - start_time).count() ;
  std::cout << "time to parse obj file using ifstream: " << time << "\n";

  for( uint32_t i=0; i < indices_vertex.size(); ++i ) {
    uint32_t index = indices_vertex.at(i) -1 ;
    glm::vec3 vertex = temp_vertices.at( index );
    vertices.push_back( vertex ) ;
    //    std::cout << "index: " << index << " - " << vertex.x << ", " << vertex.y << ", "<< vertex.z << "\n";
    index = indices_uv[i] - 1 ;
    uvs.push_back( temp_uvs.at(index) ) ;
    //    std::cout << "index: " << index << " - " << temp_uvs.at(index).x << ", " << temp_uvs.at(index).y  << "\n";
    index = indices_normals[i] - 1 ;
    normals.push_back( temp_normals.at(index) ) ;
  }
}


void load_blender_obj_fscan(const std::string& file, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals)
{
  // Compare performance to ifstream.
  // Adapted from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
  
  std::vector<uint32_t> indices_vertex, indices_uv, indices_normals;
  std::vector<glm::vec3> temp_vertices;
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;

  auto start_time = std::chrono::high_resolution_clock::now();

  FILE * input = fopen(file.c_str(), "r");
  if ( !input ) 
    throw std::runtime_error("[load_blender_obj] Couldn't open file " + file );

  while( 1 ){
    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(input, "%s", lineHeader);
    if (res == EOF)
      break; 
    if ( strcmp( lineHeader, "v" ) == 0 ){
      glm::vec3 vertex;
      fscanf(input, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
      temp_vertices.push_back(vertex);
    }
    else if ( strcmp( lineHeader, "vt" ) == 0 ){
      glm::vec2 uv;
      fscanf(input, "%f %f\n", &uv.x, &uv.y );
      uv.y *= -1;
      temp_uvs.push_back(uv);
    }
    else if ( strcmp( lineHeader, "vn" ) == 0 ){
      glm::vec3 normal;
      fscanf(input, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
      temp_normals.push_back(normal);
    }
    else if ( strcmp( lineHeader, "f" ) == 0 ){
      std::string vertex1, vertex2, vertex3;
      unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
      int matches = fscanf(input, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
      if (matches != 9){
	throw std::runtime_error("File can't be read by parser. Try exporting with other options");
      }
      indices_vertex.push_back(vertexIndex[0]);
      indices_vertex.push_back(vertexIndex[1]);
      indices_vertex.push_back(vertexIndex[2]);
      indices_uv.push_back(uvIndex[0]);
      indices_uv.push_back(uvIndex[1]);
      indices_uv.push_back(uvIndex[2]);
      indices_normals.push_back(normalIndex[0]);
      indices_normals.push_back(normalIndex[1]);
      indices_normals.push_back(normalIndex[2]);
    }
    else {
      char buffer[1024];
      fgets(buffer, 1024, input);
    }
  }
  fclose(input);
  
  auto current_time = std::chrono::high_resolution_clock::now();  
  float time = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - start_time).count() ;
  std::cout << "time to parse obj file using fscan: " << time << "\n";

  for( uint32_t i=0; i < indices_vertex.size(); ++i ) {
    uint32_t index = indices_vertex.at(i) -1 ;
    glm::vec3 vertex = temp_vertices.at( index );
    vertices.push_back( vertex ) ;
    //    std::cout << "index: " << index << " - " << vertex.x << ", " << vertex.y << ", "<< vertex.z << "\n";
    index = indices_uv[i] - 1 ;
    uvs.push_back( temp_uvs.at(index) ) ;
    //    std::cout << "index: " << index << " - " << temp_uvs.at(index).x << ", " << temp_uvs.at(index).y  << "\n";
    index = indices_normals[i] - 1 ;
    normals.push_back( temp_normals.at(index) ) ;
  }
 
}


void load_blender_obj(const std::string& file, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals)
{
  load_blender_obj_fscan(file, vertices, uvs, normals);
}


////////////////////
////    SDL2    ////
////////////////////
GLuint load_texture(std::string file)
{
  GLuint texture_id = 0;
  SDL_Surface* surf = IMG_Load(file.c_str());
  if (surf == nullptr) 
    throw std::runtime_error( "IMG_Load: " + std::string( SDL_GetError() ) );

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surf);
  return texture_id;
}


void sdl_init()
{
 if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER ) < 0 ) {
    std::cerr << "SDL could not initialize! SDL_Error: " <<  SDL_GetError() << std::endl;
    exit(1);
  }
  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
    std::cout << "IMG_Init " << SDL_GetError();
    SDL_Quit();
    exit(1);
  }
  if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "2" ) ) {
    std::cerr << "Couldn't enable anisotropic texture filtering, trying linear.\n" ;
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
      std::cerr << "Couldn't enable linear texture filtering, using nearest.\n" ;
    }
  }

  if (TTF_Init() != 0){
    std::cerr << "[SbWindow::initialize] Error in TTF_init: " << SDL_GetError() << std::endl;
    SDL_Quit();
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  if ( SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) ) {
    std::string message = sdl_error("Failed to set attribute");
    throw std::runtime_error( message );
  }
  if ( SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0) ) {
    std::string message = sdl_error("Failed to set attribute");
    throw std::runtime_error( message );
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);  // 8-bit stencil buffer

}


void sdl_quit()
{
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
}

std::string sdl_error(std::string text)
{
  std::stringstream strstr;
  strstr << text << ": " << SDL_GetError();
  return strstr.str();
}

