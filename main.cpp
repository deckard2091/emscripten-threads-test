#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#else
#include <GL/gl.h>
#endif
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <thread>     // std::this_thread::sleep_for
#include <future>     // std::async

struct State
{
  SDL_Window*    window_;
  SDL_GLContext  gl_context_;   // SDL_GLContext == void*
  int            width_, height_;
  bool           running_;
};

void internet_request()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

void render_frame(State& s)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Hello, world!");
  ImGui::Text("This is a minimal ImGui example.");
  ImGui::End();

  static bool show_window = true;
  static std::future<void> fh{};
  if (show_window)
  {
    static char buffer[128] = ""; // Declare a buffer to hold input text
    ImGui::Begin("User Details");
    ImGui::InputText("Email Address", buffer, IM_ARRAYSIZE(buffer));
    if (ImGui::Button("Submit"))
    {
      fh = std::async(std::launch::async, &internet_request);
      std::cout << buffer << std::endl;
    }
    if (fh.valid() &&
        fh.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
      show_window = false;
    }
    ImGui::End();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  SDL_GL_SwapWindow(s.window_);
}

void main_loop(void* arg)
{
  State& s = *static_cast<State*>(arg);

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);

#ifndef __EMSCRIPTEN__
    // Not for Emscripten, it would just freeze the browser tab, and look odd
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
      s.running_ = false;
    }
#endif

    if (event.type == SDL_QUIT) {
      s.running_ = false;
    }
  }

  if (s.running_) {
    render_frame(s);
  }
}

int main()
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    return -1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#ifdef __EMSCRIPTEN__
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  const unsigned int fps = 60, frame_time = 1000 / fps;
#ifdef __EMSCRIPTEN__
  // Get browser window size
  double canvas_width, canvas_height;
  emscripten_get_element_css_size("#canvas", &canvas_width, &canvas_height);
  const int width = (int)canvas_width;
  const int height = (int)canvas_height;
#else
  const int width = 1024, height = 768;
#endif
  State s;
  s.window_ = SDL_CreateWindow(  // CreateWindowAndRenderer ?
    "Simple SDL2 Application",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    width, height,
    SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
  );

  if (!s.window_) {
    SDL_Quit();
    return -1;
  }

  s.gl_context_ = SDL_GL_CreateContext(s.window_);
  SDL_GL_MakeCurrent(s.window_, s.gl_context_);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplSDL2_InitForOpenGL(s.window_, s.gl_context_);
#ifdef __EMSCRIPTEN__
  ImGui_ImplOpenGL3_Init("#version 300 es");
#else
  ImGui_ImplOpenGL3_Init("#version 130"); // OpenGL 3.0 -> GLSL 1.30.10
#endif

  ImGuiIO& io = ImGui::GetIO();

  ImFontConfig font_config;
  font_config.SizePixels = 32.0f; // Font size for the TTF vector font
  font_config.GlyphExtraAdvanceX = font_config.SizePixels / 20.0f; // a guess

#ifdef FONT_PATH
  const char* ubuntu_font = FONT_PATH;
#else
  const char* ubuntu_font = "UbuntuMono-Regular.ttf";
#endif

  ImFont* font = io.Fonts->AddFontFromFileTTF(ubuntu_font, font_config.SizePixels, &font_config);
  io.Fonts->Build();
  ImGui::PushFont(font); // ImGui::GetFont() can get this ImFont* from here

  ImGui::StyleColorsDark();

  s.width_ = width;
  s.height_ = height;
  s.running_ = true;

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(main_loop, &s, fps, 1);
#else
  while (s.running_) {
    main_loop(&s);
    SDL_Delay(frame_time);
  }
#endif

  ImGui::PopFont();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(s.gl_context_);
  SDL_DestroyWindow(s.window_);
  SDL_Quit();

  return 0;
}
