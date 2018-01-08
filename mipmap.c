
#define SOKOL_IMPL
#include "main.h"

SDL_Window *window = NULL;
SDL_GLContext gl_ctx;
const int WIDTH = 800;
const int HEIGHT = 600;
const int MSAA_SAMPLES = 4;

int active = 1;


typedef struct {
	hmm_mat4 mvp;
} vs_params_t;

struct {
	uint32_t mip0[65536];   /* 256x256 */
	uint32_t mip1[16384];   /* 128x128 */
	uint32_t mip2[4096];    /* 64*64 */
	uint32_t mip3[1024];    /* 32*32 */
	uint32_t mip4[256];     /* 16*16 */
	uint32_t mip5[64];      /* 8*8 */
	uint32_t mip6[16];      /* 4*4 */
	uint32_t mip7[4];       /* 2*2 */
	uint32_t mip8[1];       /* 1*2 */
} pixels;

uint32_t mip_colors[9] = {
		0xFF0000FF,     /* red */
		0xFF00FF00,     /* green */
		0xFFFF0000,     /* blue */
		0xFFFF00FF,     /* magenta */
		0xFFFFFF00,     /* cyan */
		0xFF00FFFF,     /* yellow */
		0xFFFF00A0,     /* violet */
		0xFFFFA0FF,     /* orange */
		0xFFA000FF,     /* purple */
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
			640,
			480,
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
	sg_setup(&(sg_desc){0});

	/* a plane vertex buffer */
	float vertices[] = {
			-1.0, -1.0, 0.0,  0.0, 0.0,
			+1.0, -1.0, 0.0,  1.0, 0.0,
			-1.0, +1.0, 0.0,  0.0, 1.0,
			+1.0, +1.0, 0.0,  1.0, 1.0,
	};
	sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
		.size = sizeof(vertices),
				.content = vertices
	});

	/* initialize mipmap content, different colors and checkboard pattern */
	sg_image_content img_content;
	uint32_t* ptr = pixels.mip0;
	bool even_odd = false;
	for (int mip_index = 0; mip_index <= 8; mip_index++) {
		const int dim = 1<<(8-mip_index);
		img_content.subimage[0][mip_index].ptr = ptr;
		img_content.subimage[0][mip_index].size = dim * dim * 4;
		for (int y = 0; y < dim; y++) {
			for (int x = 0; x < dim; x++) {
				*ptr++ = even_odd ? mip_colors[mip_index] : 0xFF000000;
				even_odd = !even_odd;
			}
			even_odd = !even_odd;
		}
	}
	/* the first 4 images are just different min-filters, the last
       4 images are different anistropy levels */
	sg_image img[12];
	sg_image_desc img_desc = {
			.width = 256,
			.height = 256,
			.num_mipmaps = 9,
			.pixel_format = SG_PIXELFORMAT_RGBA8,
			.mag_filter = SG_FILTER_LINEAR,
			.content = img_content
	};
	sg_filter min_filter[] = {
			SG_FILTER_NEAREST_MIPMAP_NEAREST,
			SG_FILTER_LINEAR_MIPMAP_NEAREST,
			SG_FILTER_NEAREST_MIPMAP_LINEAR,
			SG_FILTER_LINEAR_MIPMAP_LINEAR,
	};
	for (int i = 0; i < 4; i++) {
		img_desc.min_filter = min_filter[i];
		img[i] = sg_make_image(&img_desc);
	}
	img_desc.min_lod = 2.0f;
	img_desc.max_lod = 4.0f;
	for (int i = 4; i < 8; i++) {
		img_desc.min_filter = min_filter[i-4];
		img[i] = sg_make_image(&img_desc);
	}
	img_desc.min_lod = 0.0f;
	img_desc.max_lod = 0.0f;    /* for max_lod, zero-initialized means "FLT_MAX" */
	for (int i = 8; i < 12; i++) {
		img_desc.max_anisotropy = 1<<(i-7);
		img[i] = sg_make_image(&img_desc);
	}

	/* shader
	sg_shader shd = sg_make_shader(&(sg_shader_desc){
		.vs = {
				.uniform_blocks[0] = {
						.size = sizeof(vs_params_t),
						.uniforms = {
								[0] = { .name="mvp", .type=SG_UNIFORMTYPE_MAT4 }
						}
				},
				.source =
						"#version 330\n"
						"uniform mat4 mvp;\n"
						"in vec4 position;\n"
						"in vec2 texcoord0;\n"
						"out vec2 uv;\n"
						"void main() {\n"
						"  gl_Position = mvp * position;\n"
						"  uv = texcoord0;\n"
						"}\n"
		},
				.fs = {
						.images[0] = { .name="tex", .type = SG_IMAGETYPE_2D },
						.source =
								"#version 330\n"
								"uniform sampler2D tex;"
								"in vec2 uv;\n"
								"out vec4 frag_color;\n"
								"void main() {\n"
								"  frag_color = texture(tex, uv);\n"
								"}\n"
		}
	});
	 */

	sg_shader shd = sg_make_shader(&(sg_shader_desc){
		.vs = {
				.uniform_blocks[0] = {
						.size = sizeof(vs_params_t),
						.uniforms = {
								[0] = { .name="mvp", .type=SG_UNIFORMTYPE_MAT4 }
						}
				},
				.source =
						"uniform mat4 mvp;\n"
						"attribute vec4 position;\n"
						"attribute vec2 texcoord0;\n"
						"varying vec2 uv;\n"
						"void main() {\n"
						"  gl_Position = mvp * position;\n"
						"  uv = texcoord0;\n"
						"}\n"
		},
				.fs = {
						.images[0] = { .name="tex", .type = SG_IMAGETYPE_2D },
						.source =
								"precision mediump float;"
								"uniform sampler2D tex;"
								"varying vec2 uv;\n"
								"void main() {\n"
								"  gl_FragColor = texture2D(tex, uv);\n"
								"}\n"
		}
	});


	/* pipeline state */
	sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc) {
		.vertex_layouts[0] = {
				.stride = 20,
				.attrs = {
						[0] = { .name="position", .offset=0,  .format=SG_VERTEXFORMAT_FLOAT3 },
						[1] = { .name="texcoord0", .offset=12, .format=SG_VERTEXFORMAT_FLOAT2 }
				}
		},
		.shader = shd,
		.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
		.rasterizer.sample_count = MSAA_SAMPLES
	});

	/* view-projection matrix */
	hmm_mat4 proj = HMM_Perspective(90.0f, (float)WIDTH/(float)HEIGHT, 0.01f, 10.0f);
	hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 0.0f, 5.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
	hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);

	vs_params_t vs_params;
	float r = 0.0f;



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

		r += 0.1f;
		hmm_mat4 rm = HMM_Rotate(r, HMM_Vec3(1.0f, 0.0f, 0.0f));

		sg_draw_state draw_state = {
				.pipeline = pip,
				.vertex_buffers[0] = vbuf
		};
		// int cur_width, cur_height;
		// glfwGetFramebufferSize(w, &cur_width, &cur_height);
		sg_begin_default_pass(&(sg_pass_action){0}, cur_width, cur_height);
		for (int i = 0; i < 12; i++) {
			const float x = ((float)(i & 3) - 1.5f) * 2.0f;
			const float y = ((float)(i / 4) - 1.0f) * -2.0f;
			hmm_mat4 model = HMM_MultiplyMat4(HMM_Translate(HMM_Vec3(x, y, 0.0f)), rm);
			vs_params.mvp = HMM_MultiplyMat4(view_proj, model);

			draw_state.fs_images[0] = img[i];
			sg_apply_draw_state(&draw_state);
			sg_apply_uniform_block(SG_SHADERSTAGE_VS, 0, &vs_params, sizeof(vs_params));
			sg_draw(0, 4, 1);
		}
		sg_end_pass();
		sg_commit();
		// glfwSwapBuffers(w);
		// glfwPollEvents();

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
