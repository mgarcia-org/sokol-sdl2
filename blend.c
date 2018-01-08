
#define SOKOL_IMPL
#include "main.h"

SDL_Window *window = NULL;
SDL_GLContext gl_ctx;

int active = 1;

/* a uniform block with a model-view-projection matrix */
typedef struct {
	hmm_mat4 mvp;
} params_t;

typedef struct {
	hmm_mat4 mvp;
} vs_params_t;


// constants (VS doesn't like "const int" for array size)
enum {
	WIDTH = 640,
	HEIGHT = 480,
	IMG_WIDTH = 32,
	IMG_HEIGHT = 32,
};
const int MSAA_SAMPLES = 4;


typedef struct {
	float tick;
} fs_params_t;

enum { NUM_BLEND_FACTORS = 15 };
sg_pipeline pips[NUM_BLEND_FACTORS][NUM_BLEND_FACTORS];



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





	/* setup sokol_gfx (need to increase pipeline pool size) */
	sg_desc desc = {
			.pipeline_pool_size = NUM_BLEND_FACTORS * NUM_BLEND_FACTORS + 1
	};
	sg_setup(&desc);

	/* a quad vertex buffer */
	float vertices[] = {
			/* pos               color */
			-1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 0.5f,
			+1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.5f,
			-1.0f, +1.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.5f,
			+1.0f, +1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.5f
	};
	sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
		.size = sizeof(vertices),
				.content = vertices
	});

	/* a shader for the fullscreen background quad
	sg_shader bg_shd = sg_make_shader(&(sg_shader_desc){
		.vs.source =
				"#version 330\n"
				"in vec2 position;\n"
				"void main() {\n"
				"  gl_Position = vec4(position, 0.5, 1.0);\n"
				"}\n",
				.fs = {
						.uniform_blocks[0] = {
								.size = sizeof(fs_params_t),
								.uniforms = {
										[0] = { .name="tick", .type=SG_UNIFORMTYPE_FLOAT }
								}
						},
						.source =
								"#version 330\n"
								"uniform float tick;\n"
								"out vec4 frag_color;\n"
								"void main() {\n"
								"  vec2 xy = fract((gl_FragCoord.xy-vec2(tick)) / 50.0);\n"
								"  frag_color = vec4(vec3(xy.x*xy.y), 1.0);\n"
								"}\n"
		}
	});

	 */
	/* a shader for the blended quads
	sg_shader quad_shd = sg_make_shader(&(sg_shader_desc){
		.vs.uniform_blocks[0] = {
				.size = sizeof(vs_params_t),
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
						"}"
	});

	 */

	sg_shader bg_shd = sg_make_shader(&(sg_shader_desc){
		.vs.source =
				"attribute vec2 position;\n"
				"void main() {\n"
				"  gl_Position = vec4(position, 0.5, 1.0);\n"
				"}\n",
				.fs = {
						.uniform_blocks[0] = {
								.size = sizeof(fs_params_t),
								.uniforms = {
										[0] = { .name="tick", .type=SG_UNIFORMTYPE_FLOAT }
								}
						},
						.source =
								"precision mediump float;\n"
								"uniform float tick;\n"
								"void main() {\n"
								"  vec2 xy = fract((gl_FragCoord.xy-vec2(tick)) / 50.0);\n"
								"  gl_FragColor = vec4(vec3(xy.x*xy.y), 1.0);\n"
								"}\n"
		}
	});




	/* a pipeline state object for rendering the background quad */
	sg_pipeline bg_pip = sg_make_pipeline(&(sg_pipeline_desc){
		.vertex_layouts[0] = {
				.stride = 28,
				.attrs = {
						[0] = { .name="position", .offset=0, .format=SG_VERTEXFORMAT_FLOAT2 }
				}
		},
		.shader = bg_shd,
		.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
	});


	sg_shader quad_shd = sg_make_shader(&(sg_shader_desc){
		.vs.uniform_blocks[0] = {
				.size = sizeof(vs_params_t),
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
				"  gl_Position = mvp * position;"
				"  color = color0;\n"
				"}\n",
				.fs.source =
						"precision mediump float;\n"
						"varying vec4 color;\n"
						"void main() {\n"
						"  gl_FragColor = color;\n"
						"}"
	});

	/* one pipeline object per blend-factor combination */
	sg_pipeline_desc pip_desc = {
			.vertex_layouts[0] = {
					.stride = 28,
					.attrs = {
							[0] = { .name="position", .offset=0, .format=SG_VERTEXFORMAT_FLOAT3 },
							[1] = { .name="color0", .offset=12, .format=SG_VERTEXFORMAT_FLOAT4 }
					}
			},
			.shader = quad_shd,
			.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
			.blend = {
					.enabled = true,
					.blend_color = { 1.0f, 0.0f, 0.0f, 1.0f },
			},
	};
	for (int src = 0; src < NUM_BLEND_FACTORS; src++) {
		for (int dst = 0; dst < NUM_BLEND_FACTORS; dst++) {
			const sg_blend_factor src_blend = (sg_blend_factor) (src+1);
			const sg_blend_factor dst_blend = (sg_blend_factor) (dst+1);
			pip_desc.blend.src_factor_rgb = src_blend;
			pip_desc.blend.dst_factor_rgb = dst_blend;
			pip_desc.blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
			pip_desc.blend.dst_factor_alpha = SG_BLENDFACTOR_ZERO;
			pips[src][dst] = sg_make_pipeline(&pip_desc);
			assert(pips[src][dst].id != SG_INVALID_ID);
		}
	}

	/* a pass action which does not clear, since the entire screen is overwritten anyway */
	sg_pass_action pass_action = {
			.colors[0].action = SG_ACTION_DONTCARE ,
			.depth.action = SG_ACTION_DONTCARE,
			.stencil.action = SG_ACTION_DONTCARE
	};

	/* view-projection matrix */
	hmm_mat4 proj = HMM_Perspective(90.0f, (float)WIDTH/(float)HEIGHT, 0.01f, 100.0f);
	hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 0.0f, 25.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
	hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);

	/* a draw state with resource bindings */
	sg_draw_state draw_state = {
			.vertex_buffers[0] = vbuf
	};

	vs_params_t vs_params;
	fs_params_t fs_params;
	float r = 0.0f;
	fs_params.tick = 0.0f;







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

		// int cur_width, cur_height;
		// glfwGetFramebufferSize(w, &cur_width, &cur_height);
		sg_begin_default_pass(&pass_action, cur_width, cur_height);

		/* draw a background quad */
		draw_state.pipeline = bg_pip;
		sg_apply_draw_state(&draw_state);
		sg_apply_uniform_block(SG_SHADERSTAGE_FS, 0, &fs_params, sizeof(fs_params));
		sg_draw(0, 4, 1);

		/* draw the blended quads */
		float r0 = r;
		for (int src = 0; src < NUM_BLEND_FACTORS; src++) {
			for (int dst = 0; dst < NUM_BLEND_FACTORS; dst++, r0+=0.6f) {
				/* compute new model-view-proj matrix */
				hmm_mat4 rm = HMM_Rotate(r0, HMM_Vec3(0.0f, 1.0f, 0.0f));
				const float x = ((float)(dst - NUM_BLEND_FACTORS/2)) * 3.0f;
				const float y = ((float)(src - NUM_BLEND_FACTORS/2)) * 2.2f;
				hmm_mat4 model = HMM_MultiplyMat4(HMM_Translate(HMM_Vec3(x, y, 0.0f)), rm);
				vs_params.mvp = HMM_MultiplyMat4(view_proj, model);

				draw_state.pipeline = pips[src][dst];
				sg_apply_draw_state(&draw_state);
				sg_apply_uniform_block(SG_SHADERSTAGE_VS, 0, &vs_params, sizeof(vs_params));
				sg_draw(0, 4, 1);
			}
		}
		sg_end_pass();
		sg_commit();
		//glfwSwapBuffers(w);
		//glfwPollEvents();

		SDL_GL_SwapWindow(window);

		r += 0.6f;
		fs_params.tick += 1.0f;




		// glfwSwapBuffers(w);
		// glfwPollEvents();
	}

	/* cleanup */

	//glfwTerminate();


	sg_shutdown();
	SDL_DestroyWindow(window);
	SDL_Quit();


}


