#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_NO_SSE
#include "HandmadeMath.h"

#define GL_GLEXT_PROTOTYPES

#ifdef SOKOL_GLCORE33

 #include <GL/gl.h>
 #include <GL/glext.h>

//#include <SDL2/SDL_opengl.h>
//#include <SDL2/SDL_opengl_glext.h>
#endif




#ifdef SOKOL_GLES3
#include <GLES3/gl3platform.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif


#ifdef SOKOL_GLES2
#include <SDL2/SDL_opengles2.h>
#include <SDL2/SDL_opengles2_gl2ext.h>
//my GLES2 is missing this?? :/
//#define glVertexAttribDivisorEXT PFNGLVERTEXATTRIBDIVISOREXTPROC
#define glVertexAttribDivisorEXT //
#endif



#define  SOKOL_LOG(msg) printf(msg)
// SOKOL_DEBUG 0 //print errors

#include "sokol/sokol_gfx.h"



extern SDL_Window *window;
extern SDL_GLContext gl_ctx;


