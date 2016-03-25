/// sglWindow.h
/// Open an SDL2 window for use with OpenGL
/// author: Ulrike Hager

#ifndef SGL_WINDOW
#define SGL_WINDOW

#include <memory>
#include <string>

#include <SDL2/SDL.h>

struct DeleteWindow
{
  void operator()(SDL_Window* win) const{
    if (win){
      SDL_DestroyWindow(win);
      win = nullptr;
    }
  }
};


struct DeleteContext
{
  void operator()(SDL_GLContext* con) const {
    if (con) {
      SDL_GL_DeleteContext(con);
    }
  }
};


class sglWindow
{
 public:
  sglWindow(std::string name, int width, int height);
  sglWindow(const sglWindow& toCopy) = delete;
  sglWindow& operator=(const sglWindow& toCopy) = delete;
  void swap() {
    if (window_)
      SDL_GL_SwapWindow(window_.get());
  } 

  uint32_t width() {return width_;}
  uint32_t height() {return height_;}

 private:
  //! The order here is important to make sure context and window are destroyed in the correct order.
  std::unique_ptr<SDL_GLContext, DeleteContext> gl_context_ = nullptr;
  std::unique_ptr<SDL_Window, DeleteWindow> window_ = nullptr;
  uint32_t width_ = 600;
  uint32_t height_ = 400;

};


#endif //  SGL_WINDOW
