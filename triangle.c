
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


	sg_desc desc={0};

	/* setup sokol_gfx */
	sg_setup(&desc);

	/* a vertex buffer */
	const float vertices[] = {
			// positions            // colors
			0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
	};

	sg_buffer_desc buff={0};

	buff.size = sizeof(vertices);
	buff.content = vertices;

	sg_buffer vbuf = sg_make_buffer( &buff );

	sg_shader_desc shader={0};

/*
	shader.vs.source =
			//"#version 2\n"
			"in vec4 position;\n"
			"in vec4 color0;\n"
			"out vec4 color;\n"
			"void main() {\n"
			"  gl_Position = position;\n"
			"  color = color0;\n"
			"}\n";

	shader.fs.source =
			//"#version 2\n"
			"in vec4 color;\n"
			"out vec4 frag_color;\n"
			"void main() {\n"
			"  frag_color = color;\n"
			"}\n";
*/


	shader.vs.source =
			//	"#version 300\n"
			 "attribute vec4 position;\n"
			            "attribute vec4 color0;\n"
			            "varying vec4 color;\n"
			            "void main() {\n"
			            "  gl_Position = position;\n"
			            "  color = color0;\n"
				"}\n";

		shader.fs.source =
			//	"#version 300\n"
				 "precision mediump float;\n"
				            "varying vec4 color;\n"
				            "void main() {\n"
				            "  gl_FragColor = color;\n"
				"}\n";


	/* a shader */
	sg_shader shd = sg_make_shader(&shader);



	sg_pipeline_desc pipeline={0};



	pipeline.shader = shd;

	pipeline.vertex_layouts[0].stride = 28;
	pipeline.vertex_layouts[0].attrs[0].name="position";
	pipeline.vertex_layouts[0].attrs[0].offset=0;
	pipeline.vertex_layouts[0].attrs[0].format=SG_VERTEXFORMAT_FLOAT3;


	pipeline.vertex_layouts[0].attrs[1].name="color0";
	pipeline.vertex_layouts[0].attrs[1].offset=12;
	pipeline.vertex_layouts[0].attrs[1].format=SG_VERTEXFORMAT_FLOAT4;

	/* a pipeline state object */
	sg_pipeline pip = sg_make_pipeline(&pipeline);


	/* a draw state with all the resource binding */
	sg_draw_state draw_state={0};
	draw_state.pipeline = pip;
	draw_state.vertex_buffers[0] = vbuf;



	/* default pass action (clear to grey) */
	sg_pass_action pass_action = {0};

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


		// glfwGetFramebufferSize(w, &cur_width, &cur_height);
		sg_begin_default_pass(&pass_action, cur_width, cur_height);
		sg_apply_draw_state(&draw_state);
		sg_draw(0, 3, 1);
		sg_end_pass();
		sg_commit();

		SDL_GL_SwapWindow(window);

		// glfwSwapBuffers(w);
		// glfwPollEvents();
	}

	/* cleanup */
	sg_shutdown();
	//glfwTerminate();



	sg_shutdown();
	SDL_DestroyWindow(window);
	SDL_Quit();


}
