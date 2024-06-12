#include <SDL2/SDL_video.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include "shader.h"

#define SDL_HINT_MOUSE_RELATIVE_SCALING "SDL_MOUSE_RELATIVE_SCALING"
#define SDL_MAIN_HANDLED

typedef uint64_t u64;

static bool should_quit = false;

struct Square {

  float vertices[36] = {
     // positions         // colors                 //texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
    0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // bottom right
    -0.5f,  -0.5f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // bottom left 
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f, // top left
  };

  unsigned int indices[6] = {
    0, 1, 3, //1st triangle
    1, 2, 3, //2nd triangle
  };
};

struct {
  u64 now = SDL_GetPerformanceCounter();
  u64 time = SDL_GetPerformanceFrequency();
  u64 last = 0;
  double delta = 0;

} Time;

struct {
  int width = 800;
  int height = 450;
} Window;

struct {
  Shader shader;
} Dewdrop;



static SDL_GLContext context;
static SDL_Window* window;

void Draw() {
  GLfloat vVertices[] = { 0.0f, 0.5f, 0.0f,
                           -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f };
  
  Dewdrop.shader.use();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
  glEnableVertexAttribArray(0);

  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void apocalypseNow() {

    Dewdrop.shader.kill();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// For emscripten compatability
static void mainloop(void) {
  if(should_quit) {  
    
    apocalypseNow();

    #ifdef __EMSCRIPTEN__
    emscripten_cancel_main_loop(); /* this should "kill" the app. */
    #else
    exit(EXIT_SUCCESS);
    #endif
  }

  Time.last = Time.now;
  Time.now = SDL_GetPerformanceCounter();
  Time.delta = (double)((Time.now - Time.last) * 1000 / (double)SDL_GetPerformanceFrequency());

  glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  Draw();


  SDL_GL_SwapWindow(window);
}

int main(int argc, char *argv[]) {
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0 ) {
    printf( "[sdl] SDL could not initialize! SDL_Error=   %s\n", SDL_GetError() );
    return EXIT_FAILURE;
  }
  // Creates window
  window = SDL_CreateWindow( "Sokoban!", 
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
      Window.width, Window.height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); 
  if( window == NULL ) {
    printf( "[sdl] Window could not be created! SDL_Error=   %s\n", SDL_GetError() );
    return EXIT_FAILURE;
  }
  else {
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    context = SDL_GL_CreateContext(window);
    if(!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
      printf( "[sdl] Failed to load GL=   %s\n", SDL_GetError() );
      SDL_DestroyWindow(window);
      SDL_Quit();
      return EXIT_FAILURE;
    }
    else {
      
      SDL_GL_MakeCurrent(window, context);

      //Use Vsync
      if( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
          printf("[sdl] Unable to set VSync! SDL Error= %s\n", SDL_GetError());
      }
      
      glViewport(0, 0, 800, 450);

      int nAttributes;
      glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nAttributes);

      printf("[ogl] Vendor=   %s\n", glGetString(GL_VENDOR));
      printf("[ogl] Renderer=   %s\n", glGetString(GL_RENDERER));
      printf("[ogl] Version=  %s\n", glGetString(GL_VERSION));
      printf("[ogl] Max number of vertex attributes=    %d\n", nAttributes);
    }
  }

  Shader shader("res/es-shaders/shader.vert", "res/es-shaders/shader.frag");
  Dewdrop.shader = shader;
  



  #ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(mainloop, -1, 1); // CAN PASS ARGs, consider passing context
  #else
  while (1) { mainloop(); }
  #endif
}
