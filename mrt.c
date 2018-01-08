
 #define SOKOL_GLCORE33
// #define SOKOL_GLES3
 // #define SOKOL_GLES2


#define SOKOL_IMPL
#include "main.h"

SDL_Window *window = NULL;
SDL_GLContext gl_ctx;

int active = 1;
const int WIDTH = 640;
 const int HEIGHT = 480;

typedef struct {
	float x, y, z;
	float b;
} vertex_t;

typedef struct {
	hmm_mat4 mvp;
} offscreen_params_t;

typedef struct {
	hmm_vec2 offset;
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

	//ES 3 demo

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


	/* setup sokol_gfx */
	sg_desc desc = {0};
	sg_setup(&desc);
	assert(sg_isvalid());

	/* a render pass with 3 color attachment images, and a depth attachment image */
	const int offscreen_sample_count = sg_query_feature(SG_FEATURE_MSAA_RENDER_TARGETS) ? 4:1;
	sg_image_desc color_img_desc = {
			.render_target = true,
			.width = WIDTH,
			.height = HEIGHT,
			.min_filter = SG_FILTER_LINEAR,
			.mag_filter = SG_FILTER_LINEAR,
			.wrap_u = SG_WRAP_CLAMP_TO_EDGE,
			.wrap_v = SG_WRAP_CLAMP_TO_EDGE,
			.sample_count = offscreen_sample_count
	};
	sg_image_desc depth_img_desc = color_img_desc;
	depth_img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
	sg_pass_desc offscreen_pass_desc = {
			.color_attachments = {
					[0].image = sg_make_image(&color_img_desc),
					[1].image = sg_make_image(&color_img_desc),
					[2].image = sg_make_image(&color_img_desc)
			},
			.depth_stencil_attachment.image = sg_make_image(&depth_img_desc)
	};
	sg_pass offscreen_pass = sg_make_pass(&offscreen_pass_desc);

	/* a matching pass action with clear colors */
	sg_pass_action offscreen_pass_action = {
			.colors = {
					[0] = { .action = SG_ACTION_CLEAR, .val = { 0.25f, 0.0f, 0.0f, 1.0f } },
					[1] = { .action = SG_ACTION_CLEAR, .val = { 0.0f, 0.25f, 0.0f, 1.0f } },
					[2] = { .action = SG_ACTION_CLEAR, .val = { 0.0f, 0.0f, 0.25f, 1.0f } }
			}
	};

	/* cube vertex buffer */
	vertex_t cube_vertices[] = {
			/* pos + brightness */
			{ -1.0f, -1.0f, -1.0f,   1.0f },
			{  1.0f, -1.0f, -1.0f,   1.0f },
			{  1.0f,  1.0f, -1.0f,   1.0f },
			{ -1.0f,  1.0f, -1.0f,   1.0f },

			{ -1.0f, -1.0f,  1.0f,   0.8f },
			{  1.0f, -1.0f,  1.0f,   0.8f },
			{  1.0f,  1.0f,  1.0f,   0.8f },
			{ -1.0f,  1.0f,  1.0f,   0.8f },

			{ -1.0f, -1.0f, -1.0f,   0.6f },
			{ -1.0f,  1.0f, -1.0f,   0.6f },
			{ -1.0f,  1.0f,  1.0f,   0.6f },
			{ -1.0f, -1.0f,  1.0f,   0.6f },

			{ 1.0f, -1.0f, -1.0f,    0.4f },
			{ 1.0f,  1.0f, -1.0f,    0.4f },
			{ 1.0f,  1.0f,  1.0f,    0.4f },
			{ 1.0f, -1.0f,  1.0f,    0.4f },

			{ -1.0f, -1.0f, -1.0f,   0.5f },
			{ -1.0f, -1.0f,  1.0f,   0.5f },
			{  1.0f, -1.0f,  1.0f,   0.5f },
			{  1.0f, -1.0f, -1.0f,   0.5f },

			{ -1.0f,  1.0f, -1.0f,   0.7f },
			{ -1.0f,  1.0f,  1.0f,   0.7f },
			{  1.0f,  1.0f,  1.0f,   0.7f },
			{  1.0f,  1.0f, -1.0f,   0.7f },
	};
	sg_buffer cube_vbuf = sg_make_buffer(&(sg_buffer_desc){
		.size = sizeof(cube_vertices),
				.content = cube_vertices,
	});

	/* index buffer for the cube */
	uint16_t cube_indices[] = {
			0, 1, 2,  0, 2, 3,
			6, 5, 4,  7, 6, 4,
			8, 9, 10,  8, 10, 11,
			14, 13, 12,  15, 14, 12,
			16, 17, 18,  16, 18, 19,
			22, 21, 20,  23, 22, 20
	};
	sg_buffer cube_ibuf = sg_make_buffer(&(sg_buffer_desc){
		.type = SG_BUFFERTYPE_INDEXBUFFER,
				.size = sizeof(cube_indices),
				.content = cube_indices,
	});

	/* a shader to render the cube into offscreen MRT render targets
	sg_shader cube_shd = sg_make_shader(&(sg_shader_desc){
		.vs.uniform_blocks[0] = {
				.size = sizeof(offscreen_params_t),
				.uniforms = {
						[0] = { .name="mvp", .type=SG_UNIFORMTYPE_MAT4 }
				}
		},
		.vs.source =
				"#version 330\n"
				"uniform mat4 mvp;\n"
				"in vec4 position;\n"
				"in float bright0;\n"
				"out float bright;\n"
				"void main() {\n"
				"  gl_Position = mvp * position;\n"
				"  bright = bright0;\n"
				"}\n",
				.fs.source =
						"#version 330\n"
						"in float bright;\n"
						"layout(location=0) out vec4 frag_color_0;\n"
						"layout(location=1) out vec4 frag_color_1;\n"
						"layout(location=2) out vec4 frag_color_2;\n"
						"void main() {\n"
						"  frag_color_0 = vec4(bright, 0.0, 0.0, 1.0);\n"
						"  frag_color_1 = vec4(0.0, bright, 0.0, 1.0);\n"
						"  frag_color_2 = vec4(0.0, 0.0, bright, 1.0);\n"
						"}\n"
	});

 */


	sg_shader cube_shd = sg_make_shader(&(sg_shader_desc){
		.vs.uniform_blocks[0] = {
				.size = sizeof(offscreen_params_t),
				.uniforms = {
						[0] = { .name="mvp", .type=SG_UNIFORMTYPE_MAT4 }
				}
		},
		  .vs.source =
		            "#version 300 es\n"
		            "uniform mat4 mvp;\n"
		            "in vec4 position;\n"
		            "in float bright0;\n"
		            "out float bright;\n"
		            "void main() {\n"
		            "  gl_Position = mvp * position;\n"
		            "  bright = bright0;\n"
		            "}\n",
		        .fs.source =
		            "#version 300 es\n"
		            "precision mediump float;\n"
		            "in float bright;\n"
		            "layout(location=0) out vec4 frag_color_0;\n"
		            "layout(location=1) out vec4 frag_color_1;\n"
		            "layout(location=2) out vec4 frag_color_2;\n"
		            "void main() {\n"
		            "  frag_color_0 = vec4(bright, 0.0, 0.0, 1.0);\n"
		            "  frag_color_1 = vec4(0.0, bright, 0.0, 1.0);\n"
		            "  frag_color_2 = vec4(0.0, 0.0, bright, 1.0);\n"
		            "}\n"
	});

	/* pipeline object for the offscreen-rendered cube */
	sg_pipeline cube_pip = sg_make_pipeline(&(sg_pipeline_desc){
		.vertex_layouts[0] = {
				.stride = sizeof(vertex_t),
				.attrs = {
						[0] = { .name="position", .offset=offsetof(vertex_t,x), .format=SG_VERTEXFORMAT_FLOAT3 },
						[1] = { .name="bright0", .offset=offsetof(vertex_t,b), .format=SG_VERTEXFORMAT_FLOAT }
				}
		},
		.shader = cube_shd,
		.index_type = SG_INDEXTYPE_UINT16,
		.depth_stencil = {
				.depth_compare_func = SG_COMPAREFUNC_LESS_EQUAL,
				.depth_write_enabled = true
		},
		.blend = {
				.color_attachment_count = 3,
				.depth_format = SG_PIXELFORMAT_DEPTH
		},
		.rasterizer = {
				.cull_mode = SG_CULLMODE_BACK,
				.sample_count = offscreen_sample_count
		}
	});

	/* draw state for offscreen rendering */
	sg_draw_state offscreen_ds = {
			.pipeline = cube_pip,
			.vertex_buffers[0] = cube_vbuf,
			.index_buffer = cube_ibuf
	};

	/* a vertex buffer to render a fullscreen rectangle */
	/* -> FIXME: we should allow bufferless rendering */
	float quad_vertices[] = { 0.0f, 0.0f,  1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f };
	sg_buffer quad_buf = sg_make_buffer(&(sg_buffer_desc){
		.size = sizeof(quad_vertices),
				.content = quad_vertices,
	});

	/* a shader to render a fullscreen rectangle, which 'composes'
	       the 3 offscreen render target images onto the screen
	sg_shader fsq_shd = sg_make_shader(&(sg_shader_desc){
		.vs.uniform_blocks[0] = {
				.size = sizeof(params_t),
				.uniforms = {
						[0] = { .name="offset", .type=SG_UNIFORMTYPE_FLOAT2}
				}
		},
		.fs.images = {
				[0] = { .name="tex0", .type=SG_IMAGETYPE_2D },
				[1] = { .name="tex1", .type=SG_IMAGETYPE_2D },
				[2] = { .name="tex2", .type=SG_IMAGETYPE_2D }
		},
		.vs.source =
				"#version 330\n"
				"uniform vec2 offset;"
				"in vec2 pos;\n"
				"out vec2 uv0;\n"
				"out vec2 uv1;\n"
				"out vec2 uv2;\n"
				"void main() {\n"
				"  gl_Position = vec4(pos*2.0-1.0, 0.5, 1.0);\n"
				"  uv0 = pos + vec2(offset.x, 0.0);\n"
				"  uv1 = pos + vec2(0.0, offset.y);\n"
				"  uv2 = pos;\n"
				"}\n",
				.fs.source =
						"#version 330\n"
						"uniform sampler2D tex0;\n"
						"uniform sampler2D tex1;\n"
						"uniform sampler2D tex2;\n"
						"in vec2 uv0;\n"
						"in vec2 uv1;\n"
						"in vec2 uv2;\n"
						"out vec4 frag_color;\n"
						"void main() {\n"
						"  vec3 c0 = texture(tex0, uv0).xyz;\n"
						"  vec3 c1 = texture(tex1, uv1).xyz;\n"
						"  vec3 c2 = texture(tex2, uv2).xyz;\n"
						"  frag_color = vec4(c0 + c1 + c2, 1.0);\n"
						"}\n"
	});
 */


	sg_shader fsq_shd = sg_make_shader(&(sg_shader_desc){
		.vs.uniform_blocks[0] = {
				.size = sizeof(params_t),
				.uniforms = {
						[0] = { .name="offset", .type=SG_UNIFORMTYPE_FLOAT2}
				}
		},
		.fs.images = {
				[0] = { .name="tex0", .type=SG_IMAGETYPE_2D },
				[1] = { .name="tex1", .type=SG_IMAGETYPE_2D },
				[2] = { .name="tex2", .type=SG_IMAGETYPE_2D }
		},
		  .vs.source =
		            "#version 300 es\n"
		            "uniform vec2 offset;"
		            "in vec2 pos;\n"
		            "out vec2 uv0;\n"
		            "out vec2 uv1;\n"
		            "out vec2 uv2;\n"
		            "void main() {\n"
		            "  gl_Position = vec4(pos*2.0-1.0, 0.5, 1.0);\n"
		            "  uv0 = pos + vec2(offset.x, 0.0);\n"
		            "  uv1 = pos + vec2(0.0, offset.y);\n"
		            "  uv2 = pos;\n"
		            "}\n",
		        .fs.source =
		            "#version 300 es\n"
		            "precision mediump float;\n"
		            "uniform sampler2D tex0;\n"
		            "uniform sampler2D tex1;\n"
		            "uniform sampler2D tex2;\n"
		            "in vec2 uv0;\n"
		            "in vec2 uv1;\n"
		            "in vec2 uv2;\n"
		            "out vec4 frag_color;\n"
		            "void main() {\n"
		            "  vec3 c0 = texture(tex0, uv0).xyz;\n"
		            "  vec3 c1 = texture(tex1, uv1).xyz;\n"
		            "  vec3 c2 = texture(tex2, uv2).xyz;\n"
		            "  frag_color = vec4(c0 + c1 + c2, 1.0);\n"
		            "}\n"
	});


	/* the pipeline object for the fullscreen rectangle */
	sg_pipeline fsq_pip = sg_make_pipeline(&(sg_pipeline_desc){
		.vertex_layouts[0] = {
				.stride = 8,
				.attrs[0] = { .name="pos", .offset=0, .format=SG_VERTEXFORMAT_FLOAT2 }
		},
		.shader = fsq_shd,
		.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP
	});

	/* draw state to render the fullscreen quad */
	sg_draw_state fsq_ds = {
			.pipeline = fsq_pip,
			.vertex_buffers[0] = quad_buf,
			.fs_images = {
					[0] = offscreen_pass_desc.color_attachments[0].image,
					[1] = offscreen_pass_desc.color_attachments[1].image,
					[2] = offscreen_pass_desc.color_attachments[2].image,
			}
	};

	/* and prepare a quick'n'dirty draw state to render a debug visualizations
	       of the offscreen render target contents
	sg_draw_state dbg_ds = {
			.pipeline = sg_make_pipeline(&(sg_pipeline_desc){
				.vertex_layouts[0] = {
						.stride = 8,
						.attrs[0] = { .name="pos", .offset=0, .format=SG_VERTEXFORMAT_FLOAT2 }
				},
				.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
				.shader = sg_make_shader(&(sg_shader_desc){
					.vs.source =
							"#version 330\n"
							"in vec2 pos;\n"
							"out vec2 uv;\n"
							"void main() {\n"
							"  gl_Position = vec4(pos*2.0-1.0, 0.5, 1.0);\n"
							"  uv = pos;\n"
							"}\n",
							.fs.images[0] = { .name="tex", .type=SG_IMAGETYPE_2D },
							.fs.source =
									"#version 330\n"
									"uniform sampler2D tex;\n"
									"in vec2 uv;\n"
									"out vec4 frag_color;\n"
									"void main() {\n"
									"  frag_color = vec4(texture(tex,uv).xyz, 1.0);\n"
									"}\n"
				})
			}),
			.vertex_buffers[0] = quad_buf,

	};
*/

sg_draw_state dbg_ds = {
		.pipeline = sg_make_pipeline(&(sg_pipeline_desc){
			.vertex_layouts[0] = {
					.stride = 8,
					.attrs[0] = { .name="pos", .offset=0, .format=SG_VERTEXFORMAT_FLOAT2 }
			},
			.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
			.shader = sg_make_shader(&(sg_shader_desc){
				.vs.source =
				                    "#version 300 es\n"
				                    "uniform vec2 offset;"
				                    "in vec2 pos;\n"
				                    "out vec2 uv;\n"
				                    "void main() {\n"
				                    "  gl_Position = vec4(pos*2.0-1.0, 0.5, 1.0);\n"
				                    "  uv = pos;\n"
				                    "}\n",
				                .fs.images[0] = { .name="tex", .type=SG_IMAGETYPE_2D },
				                .fs.source =
				                    "#version 300 es\n"
				                    "precision mediump float;\n"
				                    "uniform sampler2D tex;\n"
				                    "in vec2 uv;\n"
				                    "out vec4 frag_color;\n"
				                    "void main() {\n"
				                    "  frag_color = vec4(texture(tex,uv).xyz, 1.0);\n"
				                    "}\n"
			})
		}),
		.vertex_buffers[0] = quad_buf,
		/* images will be filled right before rendering */
};


	/* default pass action, no clear needed, since whole screen is overwritten */
	sg_pass_action default_pass_action = {
			.colors = {
					[0].action = SG_ACTION_DONTCARE,
					[1].action = SG_ACTION_DONTCARE,
					[2].action = SG_ACTION_DONTCARE
			},
			.depth.action = SG_ACTION_DONTCARE,
			.stencil.action = SG_ACTION_DONTCARE
	};

	/* view-projection matrix for the offscreen-rendered cube */
	hmm_mat4 proj = HMM_Perspective(60.0f, (float)WIDTH/(float)HEIGHT, 0.01f, 10.0f);
	hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 6.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
	hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);

	offscreen_params_t offscreen_params;
	params_t params;
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

			rx += 1.0f; ry += 2.0f;
			hmm_mat4 rxm = HMM_Rotate(rx, HMM_Vec3(1.0f, 0.0f, 0.0f));
			hmm_mat4 rym = HMM_Rotate(ry, HMM_Vec3(0.0f, 1.0f, 0.0f));
			hmm_mat4 model = HMM_MultiplyMat4(rxm, rym);
			offscreen_params.mvp = HMM_MultiplyMat4(view_proj, model);
			params.offset = HMM_Vec2(HMM_SinF(rx*0.01f)*0.1f, HMM_SinF(ry*0.01f)*0.1f);

			/* render cube into MRT offscreen render targets */
			sg_begin_pass(offscreen_pass, &offscreen_pass_action);
			sg_apply_draw_state(&offscreen_ds);
			sg_apply_uniform_block(SG_SHADERSTAGE_VS, 0, &offscreen_params, sizeof(offscreen_params));
			sg_draw(0, 36, 1);
			sg_end_pass();

			/* render fullscreen quad with the 'composed image', plus 3 small
			           debug-views with the content of the offscreen render targets */
			// int cur_width, cur_height;
			// glfwGetFramebufferSize(w, &cur_width, &cur_height);
			sg_begin_default_pass(&default_pass_action, cur_width, cur_height);
			sg_apply_draw_state(&fsq_ds);
			sg_apply_uniform_block(SG_SHADERSTAGE_VS, 0, &params, sizeof(params));
			sg_draw(0, 4, 1);
			for (int i = 0; i < 3; i++) {
				sg_apply_viewport(i*100, 0, 100, 100, false);
				dbg_ds.fs_images[0] = offscreen_pass_desc.color_attachments[i].image;
				sg_apply_draw_state(&dbg_ds);
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


