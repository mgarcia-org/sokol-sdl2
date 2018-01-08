
#define SOKOL_IMPL
#include "main.h"

SDL_Window *window = NULL;
SDL_GLContext gl_ctx;

int active = 1;

typedef struct {
    hmm_mat4 mvp;
    hmm_vec2 offset0;
    hmm_vec2 offset1;
    hmm_vec2 offset2;
} params_t;

enum {
    WIDTH = 800,
    HEIGHT = 600,
    IMG_LAYERS = 3,
    IMG_WIDTH = 16,
    IMG_HEIGHT = 16
};



#if __WINDOWS__

extern int __argc;
extern char **__argv;
int mainReal(int argc, char **argv);

// Based on Denis K response
// See: https://msdn.microsoft.com/library/dn727674.aspx
// This adds Windows specific entrypoint to clasic startpoint of your app:
// int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return mainReal(__argc, __argv);
}


int mainReal(int argc, char **argv)

#else
int	main(int argc, char **argv)
#endif
{



	//compiles for me but doesn't run:
	//GL version: OpenGL ES 3.0 Mesa 11.2.0
	// array textures not supported by GL context
	// nor does the webGL version work for me: https://floooh.github.io/sokol-html5/asmjs/arraytex-emsc.html
	// maybe related: https://stackoverflow.com/questions/23539853/are-1d-textures-supported-in-webgl-yet
	// yet the GFML demo works fine



	Uint8 *keys;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		printf("Unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}



#if 1    // Some #define of yours that says it's a mobile build. //
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif // MOBILE //


	SDL_GL_LoadLibrary( NULL );


	printf(glGetString(GL_EXTENSIONS) );




	window = SDL_CreateWindow(
			"Sokol",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WIDTH,
			HEIGHT,
			//SDL_WINDOW_OPENGL ||
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);



	/* Create our opengl context and attach it to our window */
	gl_ctx = SDL_GL_CreateContext(window);


	printf("GL version: %s", glGetString(GL_VERSION) );

	/* This makes our buffer swap syncronized with the monitor's vertical refresh */
	SDL_GL_SetSwapInterval(-1);

	/* Clear our buffer with a red background */
	glClearColor ( 1.0, 0.0, 0.0, 1.0 );
	glClear ( GL_COLOR_BUFFER_BIT );



    /* setup sokol_gfx */
    sg_desc desc = {0};
    sg_setup(&desc);
    assert(sg_isvalid());

    /* a 16x16 array texture with 3 layers and a checkerboard pattern */
    static uint32_t pixels[IMG_LAYERS][IMG_HEIGHT][IMG_WIDTH];
    for (int layer=0, even_odd=0; layer<IMG_LAYERS; layer++) {
        for (int y = 0; y < IMG_HEIGHT; y++, even_odd++) {
            for (int x = 0; x < IMG_WIDTH; x++, even_odd++) {
                if (even_odd & 1) {
                    switch (layer) {
                        case 0: pixels[layer][y][x] = 0x000000FF; break;
                        case 1: pixels[layer][y][x] = 0x0000FF00; break;
                        case 2: pixels[layer][y][x] = 0x00FF0000; break;
                    }
                }
                else {
                    pixels[layer][y][x] = 0;
                }
            }
        }
    }
    sg_image img = sg_make_image(&(sg_image_desc) {
        .type = SG_IMAGETYPE_ARRAY,
        .width = IMG_WIDTH,
        .height = IMG_HEIGHT,
        .layers = IMG_LAYERS,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .content.subimage[0][0] = {
            .ptr = pixels,
            .size = sizeof(pixels)
        }
    });

    /* cube vertex buffer */
    float vertices[] = {
        /* pos                  uvs */
        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,    1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f,

        -1.0f, -1.0f,  1.0f,    0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,    1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 1.0f,

        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,    1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,    1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    0.0f, 1.0f,

         1.0f, -1.0f, -1.0f,    0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,    1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 1.0f,
         1.0f, -1.0f,  1.0f,    0.0f, 1.0f,

        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,    1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,    1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,    0.0f, 1.0f,

        -1.0f,  1.0f, -1.0f,    0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,    1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 1.0f,
         1.0f,  1.0f, -1.0f,    0.0f, 1.0f
    };
    sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
        .content = vertices,
    });

    /* create an index buffer for the cube */
    uint16_t indices[] = {
        0, 1, 2,  0, 2, 3,
        6, 5, 4,  7, 6, 4,
        8, 9, 10,  8, 10, 11,
        14, 13, 12,  15, 14, 12,
        16, 17, 18,  16, 18, 19,
        22, 21, 20,  23, 22, 20
    };
    sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .size = sizeof(indices),
        .content = indices,
    });

    /* shader to sample from array texture
    sg_shader shd = sg_make_shader(&(sg_shader_desc){
        .vs.uniform_blocks[0] = {
            .size = sizeof(params_t),
            .uniforms = {
                [0] = { .name="mvp",     .type=SG_UNIFORMTYPE_MAT4 },
                [1] = { .name="offset0", .type=SG_UNIFORMTYPE_FLOAT2 },
                [2] = { .name="offset1", .type=SG_UNIFORMTYPE_FLOAT2 },
                [3] = { .name="offset2", .type=SG_UNIFORMTYPE_FLOAT2 }
            }
        },
        .fs.images[0] = { .name="tex", .type=SG_IMAGETYPE_ARRAY },
        .vs.source =
            "#version 330\n"
            "uniform mat4 mvp;\n"
            "uniform vec2 offset0;\n"
            "uniform vec2 offset1;\n"
            "uniform vec2 offset2;\n"
            "in vec4 position;\n"
            "in vec2 texcoord0;\n"
            "out vec3 uv0;\n"
            "out vec3 uv1;\n"
            "out vec3 uv2;\n"
            "void main() {\n"
            "  gl_Position = mvp * position;\n"
            "  uv0 = vec3(texcoord0 + offset0, 0.0);\n"
            "  uv1 = vec3(texcoord0 + offset1, 1.0);\n"
            "  uv2 = vec3(texcoord0 + offset2, 2.0);\n"
            "}\n",
        .fs.source =
            "#version 330\n"
            "uniform sampler2DArray tex;\n"
            "in vec3 uv0;\n"
            "in vec3 uv1;\n"
            "in vec3 uv2;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "  vec4 c0 = texture(tex, uv0);\n"
            "  vec4 c1 = texture(tex, uv1);\n"
            "  vec4 c2 = texture(tex, uv2);\n"
            "  frag_color = vec4(c0.xyz + c1.xyz + c2.xyz, 1.0);\n"
            "}\n"
    });
 */


    sg_shader shd = sg_make_shader(&(sg_shader_desc){
           .vs.uniform_blocks[0] = {
               .size = sizeof(params_t),
               .uniforms = {
                   [0] = { .name="mvp",     .type=SG_UNIFORMTYPE_MAT4 },
                   [1] = { .name="offset0", .type=SG_UNIFORMTYPE_FLOAT2 },
                   [2] = { .name="offset1", .type=SG_UNIFORMTYPE_FLOAT2 },
                   [3] = { .name="offset2", .type=SG_UNIFORMTYPE_FLOAT2 }
               }
           },
           .fs.images[0] = { .name="tex", .type=SG_IMAGETYPE_ARRAY },
		   .vs.source =
		              "#version 300 es\n"
		              "uniform mat4 mvp;\n"
		              "uniform vec2 offset0;\n"
		              "uniform vec2 offset1;\n"
		              "uniform vec2 offset2;\n"
		              "in vec4 position;\n"
		              "in vec2 texcoord0;\n"
		              "out vec3 uv0;\n"
		              "out vec3 uv1;\n"
		              "out vec3 uv2;\n"
		              "void main() {\n"
		              "  gl_Position = mvp * position;\n"
		              "  uv0 = vec3(texcoord0 + offset0, 0.0);\n"
		              "  uv1 = vec3(texcoord0 + offset1, 1.0);\n"
		              "  uv2 = vec3(texcoord0 + offset2, 2.0);\n"
		              "}\n",
		          .fs.source =
		              "#version 300 es\n"
		              "precision mediump float;\n"
		              "precision lowp sampler2DArray;\n"
		              "uniform sampler2DArray tex;\n"
		              "in vec3 uv0;\n"
		              "in vec3 uv1;\n"
		              "in vec3 uv2;\n"
		              "out vec4 frag_color;\n"
		              "void main() {\n"
		              "  vec4 c0 = texture(tex, uv0);\n"
		              "  vec4 c1 = texture(tex, uv1);\n"
		              "  vec4 c2 = texture(tex, uv2);\n"
		              "  frag_color = vec4(c0.xyz + c1.xyz + c2.xyz, 1.0);\n"
		              "}\n"
       });



    /* create pipeline object */
    sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
        .vertex_layouts[0] = {
            .stride = 20,
            .attrs = {
                [0] = { .name="position", .offset=0, .format=SG_VERTEXFORMAT_FLOAT3 },
                [1] = { .name="texcoord0", .offset=12, .format=SG_VERTEXFORMAT_FLOAT2 }
            }
        },
        .shader = shd,
        .index_type = SG_INDEXTYPE_UINT16,
        .depth_stencil = {
            .depth_compare_func = SG_COMPAREFUNC_LESS_EQUAL,
            .depth_write_enabled = true
        },
        .rasterizer.cull_mode = SG_CULLMODE_BACK
    });

    /* draw state */
    sg_draw_state draw_state = {
        .pipeline = pip,
        .vertex_buffers[0] = vbuf,
        .index_buffer = ibuf,
        .fs_images[0] = img
    };

    /* default pass action */
    sg_pass_action pass_action = {
        .colors[0] = { .action=SG_ACTION_CLEAR, .val={0.0f, 0.0f, 0.0f, 1.0f} }
    };

    /* view-projection matrix */
    hmm_mat4 proj = HMM_Perspective(60.0f, (float)WIDTH/(float)HEIGHT, 0.01f, 10.0f);
    hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 6.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
    hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);

    params_t vs_params;
    float rx = 0.0f, ry = 0.0f;
    int frame_index = 0;








	SDL_Event sdl_event;

	int cur_width, cur_height;



	SDL_GetWindowSize(window, &cur_width, &cur_height);

	const Uint8 *keyboard_state  = SDL_GetKeyboardState(NULL);

	/* draw loop */
	while(active)
	{


		while( SDL_PollEvent( &sdl_event ) != 0 )
		{
			if( sdl_event.type == SDL_QUIT )
				active = 0;
		}

		if ( keyboard_state[SDL_SCANCODE_ESCAPE ]  ||  keyboard_state[SDL_QUIT ]  )
		{
			active = 0;
		}

		 /* rotated model matrix */
		        rx += 0.25f; ry += 0.5f;
		        hmm_mat4 rxm = HMM_Rotate(rx, HMM_Vec3(1.0f, 0.0f, 0.0f));
		        hmm_mat4 rym = HMM_Rotate(ry, HMM_Vec3(0.0f, 1.0f, 0.0f));
		        hmm_mat4 model = HMM_MultiplyMat4(rxm, rym);

		        /* model-view-projection matrix for vertex shader */
		        vs_params.mvp = HMM_MultiplyMat4(view_proj, model);
		        /* uv offsets */
		        float offset = (float)frame_index * 0.0001f;
		        vs_params.offset0 = HMM_Vec2(-offset, offset);
		        vs_params.offset1 = HMM_Vec2(offset, -offset);
		        vs_params.offset2 = HMM_Vec2(0.0f, 0.0f);

		       // int cur_width, cur_height;
		        //glfwGetFramebufferSize(w, &cur_width, &cur_height);
		        sg_begin_default_pass(&pass_action, cur_width, cur_height);
		        sg_apply_draw_state(&draw_state);
		        sg_apply_uniform_block(SG_SHADERSTAGE_VS, 0, &vs_params, sizeof(vs_params));
		        sg_draw(0, 36, 1);
		        sg_end_pass();
		        sg_commit();
		        //glfwSwapBuffers(w);
		        //glfwPollEvents();
		        frame_index++;


		SDL_GL_SwapWindow(window);

		// glfwSwapBuffers(w);
		// glfwPollEvents();
	}

	/* cleanup */

	//glfwTerminate();


	sg_shutdown();
	SDL_DestroyWindow(window);
	SDL_Quit();


}


