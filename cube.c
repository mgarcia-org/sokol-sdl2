
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
	sg_desc desc = {0};
	sg_setup(&desc);
	assert(sg_isvalid());

	/* cube vertex buffer */
	float vertices[] = {
			-1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
			1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
			1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
			-1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,

			-1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
			1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
			1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
			-1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,

			-1.0, -1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
			-1.0,  1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
			-1.0,  1.0,  1.0,   0.0, 0.0, 1.0, 1.0,
			-1.0, -1.0,  1.0,   0.0, 0.0, 1.0, 1.0,

			1.0, -1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
			1.0,  1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
			1.0,  1.0,  1.0,   1.0, 0.5, 0.0, 1.0,
			1.0, -1.0,  1.0,   1.0, 0.5, 0.0, 1.0,

			-1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,
			-1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
			1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
			1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,

			-1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0,
			-1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
			1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
			1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0
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
#if 0
/* create shader */
	sg_shader shd = sg_make_shader(&(sg_shader_desc) {
		.vs.uniform_blocks[0] = {
				.size = sizeof(params_t),
				.uniforms = {
						[0] = { .name="mvp", .type=SG_UNIFORMTYPE_MAT4 }
				}
		},
		.vs.source =
				"#version 330\n"
				"uniform mat4 mvp;\n"
				"in vec4 position;\n"
				"in vec4 color0;\n"
				"out vec4 color;\n"
				"void main() {\n"
				"  gl_Position = mvp * position;\n"
				"  color = color0;\n"
				"}\n",
				.fs.source =
						"#version 330\n"
						"in vec4 color;\n"
						"out vec4 frag_color;\n"
						"void main() {\n"
						"  frag_color = color;\n"
						"}\n"
	});
#endif

	sg_shader shd = sg_make_shader(&(sg_shader_desc) {
		.vs.uniform_blocks[0] = {
				.size = sizeof(params_t),
				.uniforms = {
						[0] = { .name="mvp", .type=SG_UNIFORMTYPE_MAT4 }
				}
		},
		.vs.source =
				"uniform mat4 mvp;\n"
				"attribute vec4 position;\n"
				"attribute vec4 color0;\n"
				"varying vec4 color;\n"
				"void main() {\n"
				"  gl_Position = mvp * position;\n"
				"  color = color0;\n"
				"}\n",
				.fs.source =
						"precision mediump float;\n"
						"varying vec4 color;\n"
						"void main() {\n"
						"  gl_FragColor = color;\n"
						"}\n"
	});


	/* create pipeline object */
	sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
		.vertex_layouts[0] = {
				.stride = 28,
				.attrs = {
						[0] = { .name="position", .offset=0, .format=SG_VERTEXFORMAT_FLOAT3 },
						[1] = { .name="color0", .offset=12, .format=SG_VERTEXFORMAT_FLOAT4 }
				}
		},
		.shader = shd,
		.index_type = SG_INDEXTYPE_UINT16,
		.depth_stencil = {
				.depth_compare_func = SG_COMPAREFUNC_LESS_EQUAL,
				.depth_write_enabled = true,
		},
		.rasterizer.cull_mode = SG_CULLMODE_BACK,
	});

	/* draw state struct with resource bindings */
	sg_draw_state draw_state = {
			.pipeline = pip,
			.vertex_buffers[0] = vbuf,
			.index_buffer = ibuf
	};

	/* default pass action */
	sg_pass_action pass_action = { 0 };

	/* view-projection matrix */
	hmm_mat4 proj = HMM_Perspective(60.0f, (float)WIDTH/(float)HEIGHT, 0.01f, 10.0f);
	hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 6.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
	hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);

	params_t vs_params;
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

		/* rotated model matrix */
		rx += 1.0f; ry += 2.0f;
		hmm_mat4 rxm = HMM_Rotate(rx, HMM_Vec3(1.0f, 0.0f, 0.0f));
		hmm_mat4 rym = HMM_Rotate(ry, HMM_Vec3(0.0f, 1.0f, 0.0f));
		hmm_mat4 model = HMM_MultiplyMat4(rxm, rym);

		/* model-view-projection matrix for vertex shader */
		vs_params.mvp = HMM_MultiplyMat4(view_proj, model);


		//glfwGetFramebufferSize(w, &cur_width, &cur_height);
		sg_begin_default_pass(&pass_action, cur_width, cur_height);
		sg_apply_draw_state(&draw_state);
		sg_apply_uniform_block(SG_SHADERSTAGE_VS, 0, &vs_params, sizeof(vs_params));
		sg_draw(0, 36, 1);
		sg_end_pass();
		sg_commit();


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
