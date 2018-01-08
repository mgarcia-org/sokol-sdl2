
// #define SOKOL_GLCORE33
// #define SOKOL_GLES3
 #define SOKOL_GLES2

#define SOKOL_IMPL
#include "main.h"

SDL_Window *window = NULL;
SDL_GLContext gl_ctx;
const int WIDTH = 800;
const int HEIGHT = 600;
int active = 1;

/* a uniform block with a model-view-projection matrix */
typedef struct {
	hmm_mat4 mvp;
} params_t;

typedef struct {
	hmm_mat4 mvp;
} vs_params_t;

/* width/height must be 2^N */
enum {
	IMAGE_WIDTH = 64,
	IMAGE_HEIGHT = 64,
	LIVING = 0xFFFFFFFF,
	DEAD  = 0xFF000000
};
uint32_t pixels[IMAGE_WIDTH][IMAGE_HEIGHT];

void game_of_life_init();
void game_of_life_update();



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
	Uint8 *keys;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		printf("Unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}



#ifdef SOKOL_GLES2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

#ifdef SOKOL_GLES3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif // MOBILE //

#ifdef SOKOL_GLCORE33
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif



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




	/* init sokol_gfx */
	sg_setup(&(sg_desc){0});

	/* a 128x128 image with streaming-update strategy */
	sg_image img = sg_make_image(&(sg_image_desc){
		.width = IMAGE_WIDTH,
				.height = IMAGE_HEIGHT,
				.pixel_format = SG_PIXELFORMAT_RGBA8,
				.usage = SG_USAGE_STREAM,
				.min_filter = SG_FILTER_LINEAR,
				.mag_filter = SG_FILTER_LINEAR,
				.wrap_u = SG_WRAP_CLAMP_TO_EDGE,
				.wrap_v = SG_WRAP_CLAMP_TO_EDGE
	});

	/* a cube vertex- and index-buffer */
	float vertices[] = {
			/* pos                  color                       uvs */
			-1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
			1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
			1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
			-1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 0.0f, 1.0f,     0.0f, 1.0f,

			-1.0f, -1.0f,  1.0f,    0.0f, 1.0f, 0.0f, 1.0f,     0.0f, 0.0f,
			1.0f, -1.0f,  1.0f,    0.0f, 1.0f, 0.0f, 1.0f,     1.0f, 0.0f,
			1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f, 1.0f,     1.0f, 1.0f,
			-1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f, 1.0f,     0.0f, 1.0f,

			-1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f, 1.0f,     0.0f, 0.0f,
			-1.0f,  1.0f, -1.0f,    0.0f, 0.0f, 1.0f, 1.0f,     1.0f, 0.0f,
			-1.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f, 1.0f,     1.0f, 1.0f,
			-1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f, 1.0f,     0.0f, 1.0f,

			1.0f, -1.0f, -1.0f,    1.0f, 0.5f, 0.0f, 1.0f,     0.0f, 0.0f,
			1.0f,  1.0f, -1.0f,    1.0f, 0.5f, 0.0f, 1.0f,     1.0f, 0.0f,
			1.0f,  1.0f,  1.0f,    1.0f, 0.5f, 0.0f, 1.0f,     1.0f, 1.0f,
			1.0f, -1.0f,  1.0f,    1.0f, 0.5f, 0.0f, 1.0f,     0.0f, 1.0f,

			-1.0f, -1.0f, -1.0f,    0.0f, 0.5f, 1.0f, 1.0f,     0.0f, 0.0f,
			-1.0f, -1.0f,  1.0f,    0.0f, 0.5f, 1.0f, 1.0f,     1.0f, 0.0f,
			1.0f, -1.0f,  1.0f,    0.0f, 0.5f, 1.0f, 1.0f,     1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,    0.0f, 0.5f, 1.0f, 1.0f,     0.0f, 1.0f,

			-1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 0.5f, 1.0f,     0.0f, 0.0f,
			-1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.5f, 1.0f,     1.0f, 0.0f,
			1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.5f, 1.0f,     1.0f, 1.0f,
			1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 0.5f, 1.0f,     0.0f, 1.0f
	};
	uint16_t indices[] = {
			0, 1, 2,  0, 2, 3,
			6, 5, 4,  7, 6, 4,
			8, 9, 10,  8, 10, 11,
			14, 13, 12,  15, 14, 12,
			16, 17, 18,  16, 18, 19,
			22, 21, 20,  23, 22, 20
	};
	sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
		.size = sizeof(vertices),
				.content = vertices,
	});
	sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc){
		.type = SG_BUFFERTYPE_INDEXBUFFER,
				.size = sizeof(indices),
				.content = indices,
	});

	/* a shader to render textured cube
	    sg_shader shd = sg_make_shader(&(sg_shader_desc){
	        .vs.uniform_blocks[0] = {
	            .size = sizeof(vs_params_t),
	            .uniforms = {
	                [0] = { .name="mvp", .type=SG_UNIFORMTYPE_MAT4 }
	            }
	        },
	        .fs.images[0] = { .name="tex", .type=SG_IMAGETYPE_2D },
	        .vs.source =
	            "#version 330\n"
	            "uniform mat4 mvp;\n"
	            "in vec4 position;\n"
	            "in vec4 color0;\n"
	            "in vec2 texcoord0;\n"
	            "out vec2 uv;"
	            "out vec4 color;"
	            "void main() {\n"
	            "  gl_Position = mvp * position;\n"
	            "  uv = texcoord0;\n"
	            "  color = color0;\n"
	            "}\n",
	        .fs.source =
	            "#version 330\n"
	            "uniform sampler2D tex;\n"
	            "in vec4 color;\n"
	            "in vec2 uv;\n"
	            "out vec4 frag_color;\n"
	            "void main() {\n"
	            "  frag_color = texture(tex, uv) * color;\n"
	            "}\n"
	    });

	 */

	sg_shader shd = sg_make_shader(&(sg_shader_desc){
		.vs.uniform_blocks[0] = {
				.size = sizeof(vs_params_t),
				.uniforms = {
						[0] = { .name="mvp", .type=SG_UNIFORMTYPE_MAT4 }
				}
		},
		.fs.images[0] = { .name="tex", .type=SG_IMAGETYPE_2D },
		.vs.source =
				"uniform mat4 mvp;\n"
				"attribute vec4 position;\n"
				"attribute vec4 color0;\n"
				"attribute vec2 texcoord0;\n"
				"varying vec2 uv;"
				"varying vec4 color;"
				"void main() {\n"
				"  gl_Position = mvp * position;\n"
				"  uv = texcoord0;\n"
				"  color = color0;\n"
				"}\n",
				.fs.source =
						"precision mediump float;\n"
						"uniform sampler2D tex;\n"
						"varying vec4 color;\n"
						"varying vec2 uv;\n"
						"void main() {\n"
						"  gl_FragColor = texture2D(tex, uv) * color;\n"
						"}\n"
	});


	/* a pipeline-state-object for the textured cube */
	sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
		.vertex_layouts[0] = {
				.stride = 36,
				.attrs = {
						[0] = { .name="position",   .offset=0,  .format=SG_VERTEXFORMAT_FLOAT3 },
						[1] = { .name="color0",     .offset=12, .format=SG_VERTEXFORMAT_FLOAT4 },
						[2] = { .name="texcoord0",  .offset=28, .format=SG_VERTEXFORMAT_FLOAT2 }
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

	/* draw state with resource bindings */
	sg_draw_state draw_state = {
			.pipeline = pip,
			.vertex_buffers[0] = vbuf,
			.index_buffer = ibuf,
			.fs_images[0] = img
	};

	/* default pass action (clear to grey) */
	sg_pass_action pass_action = {0};

	/* view-projection matrix */
	hmm_mat4 proj = HMM_Perspective(60.0f, (float)WIDTH/(float)HEIGHT, 0.01f, 10.0f);
	hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 4.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
	hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);

	/* initial game-of-life seed state */
	game_of_life_init();

	vs_params_t vs_params;
	float rx = 0.0f, ry = 0.0f;



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

		/* model-view-projection matrix from rotated model matrix */
		rx += 0.1f; ry += 0.2f;
		hmm_mat4 rxm = HMM_Rotate(rx, HMM_Vec3(1.0f, 0.0f, 0.0f));
		hmm_mat4 rym = HMM_Rotate(ry, HMM_Vec3(0.0f, 1.0f, 0.0f));
		hmm_mat4 model = HMM_MultiplyMat4(rxm, rym);
		vs_params.mvp = HMM_MultiplyMat4(view_proj, model);

		/* update game-of-life state */
		game_of_life_update();

		/* update the dynamic image */
		sg_update_image(img, &(sg_image_content){
			.subimage[0][0] = {
					.ptr=pixels,
					.size=sizeof(pixels)
			}
		});

		/* get current window canvas size for the default pass */
		// int cur_width, cur_height;
		// glfwGetFramebufferSize(w, &cur_width, &cur_height);

		sg_begin_default_pass(&pass_action, cur_width, cur_height);
		sg_apply_draw_state(&draw_state);
		sg_apply_uniform_block(SG_SHADERSTAGE_VS, 0, &vs_params, sizeof(vs_params));
		sg_draw(0, 36, 1);
		sg_end_pass();
		sg_commit();
		//glfwSwapBuffers(w);
		//glfwPollEvents();


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



void game_of_life_init() {
	for (int y = 0; y < IMAGE_HEIGHT; y++) {
		for (int x = 0; x < IMAGE_WIDTH; x++) {
			if ((rand() & 255) > 230) {
				pixels[y][x] = LIVING;
			}
			else {
				pixels[y][x] = DEAD;
			}
		}
	}
}

void game_of_life_update() {
	static int update_count = 0;
	for (int y = 0; y < IMAGE_HEIGHT; y++) {
		for (int x = 0; x < IMAGE_WIDTH; x++) {
			int num_living_neighbours = 0;
			for (int ny = -1; ny < 2; ny++) {
				for (int nx = -1; nx < 2; nx++) {
					if ((nx == 0) && (ny == 0)) {
						continue;
					}
					if (pixels[(y+ny)&(IMAGE_HEIGHT-1)][(x+nx)&(IMAGE_WIDTH-1)] == LIVING) {
						num_living_neighbours++;
					}
				}
			}
			/* any live cell... */
			if (pixels[y][x] == LIVING) {
				if (num_living_neighbours < 2) {
					/* ... with fewer than 2 living neighbours dies, as if caused by underpopulation */
					pixels[y][x] = DEAD;
				}
				else if (num_living_neighbours > 3) {
					/* ... with more than 3 living neighbours dies, as if caused by overpopulation */
					pixels[y][x] = DEAD;
				}
			}
			else if (num_living_neighbours == 3) {
				/* any dead cell with exactly 3 living neighbours becomes a live cell, as if by reproduction */
				pixels[y][x] = LIVING;
			}
		}
	}
	if (update_count++ > 240) {
		game_of_life_init();
		update_count = 0;
	}
}
