/// sgl-helper.h
/// Collection of functions for common OpenGL + SDL2 tasks
/// author: Ulrike Hager

#ifndef SGL_HELPER
#define SGL_HELPER

#include <vector>

#include <GL/glew.h>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


////////////////////
///   shaders   ////
////////////////////
GLuint load_shader(const std::string& file, GLenum type);
void check_program_compilation(GLuint program);
GLuint program_from_shaders(std::vector<GLuint> shaders);
GLuint program_from_shaders(GLuint vertex_shader, GLuint fragment_shader);
GLuint program_from_shaderfiles(const std::string& vertex_shader_file, const std::string& fragment_shader_file);
GLuint bind_attribute(GLuint program, const std::string& attribute);
GLuint bind_uniform(GLuint program, const std::string& uniform);

////////////////////
//  blender obj   //
////////////////////
void load_blender_obj(const std::string& file, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals);
void load_blender_obj_fscan(const std::string& file, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals);
void load_blender_obj_ifstream(const std::string& file, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals);

////////////////////
////    SDL2    ////
////////////////////
GLuint load_texture(std::string file);
void sdl_init();
void sdl_quit();
std::string sdl_error(std::string text);


#endif //  SGL_HELPER
