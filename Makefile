## sdl-opengl Makefile
## author: Ulrike Hager

CXX = g++
CXXFLAGS = -std=c++11 -fPIC -Wall -O2
LIBS = -lGLEW -lGL
DEBUG_FLAGS = -g -DDEBUG 
INCLUDES = -I$(HOME)/usr/include/
SDL_INCLUDES = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf

OBJS = sglWindow.o sgl-helper.o
EX_OBJS = sgl-test.o
ALL = libsgl.so sgl-test

all: $(ALL)
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all

.PHONY: clean

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDL_INCLUDES) $(INCLUDES) -o $@ -c $<

sgl-test: libsgl.so $(EX_OBJS)
	$(CXX) $(CXXFLAGS) $(EX_OBJS) $(LIBS)  $(SDL_LIBS) -L. -lsgl -o $@

libsgl.so: $(OBJS)
	$(CXX) -shared -o  $@ $(OBJS) $(LIBS) $(SDL_LIBS) 

clean:
	rm -f $(OBJS) $(EX_OBJS) $(ALL)
