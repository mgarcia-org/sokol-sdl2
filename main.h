#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_NO_SSE
#include "HandmadeMath.h"

#define GL_GLEXT_PROTOTYPES


#ifdef OGLES3
#include <GLES3/gl3platform.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>


#define SOKOL_GLES3

#else
#include <SDL2/SDL_opengles2.h>
#include <SDL2/SDL_opengles2_gl2ext.h>


#define SOKOL_GLES2

#endif

//#define glVertexAttribDivisorEXT PFNGLVERTEXATTRIBDIVISOREXTPROC

#define glVertexAttribDivisorEXT //


#define SOKOL_DEBUG 0 //print errors

#include "sokol/sokol_gfx.h"



extern SDL_Window *window;
extern SDL_GLContext gl_ctx;


