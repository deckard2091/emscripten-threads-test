cmake_minimum_required(VERSION 3.10)

set(n sdl2-imgui-threads2)
project(${n})

# set(OpenGL_GL_PREFERENCE "GLVND")

include(FetchContent)
FetchContent_Declare(imgui_external
  #  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.92.1.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP
  EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(imgui_external)

file(GLOB sources ${imgui_external_SOURCE_DIR}/*.cpp)
list(APPEND sources
  ${imgui_external_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
  ${imgui_external_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
add_executable(${n} ${sources} main.cpp)
target_include_directories(${n} PRIVATE ${imgui_external_SOURCE_DIR})
target_include_directories(${n} PRIVATE ${imgui_external_SOURCE_DIR}/backends)

if (EMSCRIPTEN)
  set_target_properties(${n} PROPERTIES SUFFIX ".html")
  # -sALLOW_MEMORY_GROWTH=1 -sSINGLE_FILE=1
  # -sUSE_WEBGL2=1 -sFULL_ES3=1 # At least one of these is needed:
  target_link_options(${n} PRIVATE --bind -sUSE_WEBGL2 -sFULL_ES3=1 -sUSE_SDL=2 -sUSE_PTHREADS=1 -sPTHREAD_POOL_SIZE=4 --shell-file ${CMAKE_SOURCE_DIR}/shell-minimal.html --embed-file ${CMAKE_SOURCE_DIR}/fonts/Ubuntu_Mono/UbuntuMono-Regular.ttf@UbuntuMono-Regular.ttf)
  target_compile_options(${n} PRIVATE -sUSE_SDL=2 -sUSE_PTHREADS=1) # yes again
  file(COPY ${CMAKE_SOURCE_DIR}/app.js DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY ${CMAKE_SOURCE_DIR}/serve_coop_coep.py DESTINATION ${CMAKE_BINARY_DIR})
else ()
  find_package(OpenGL REQUIRED)
  find_package(SDL2 CONFIG QUIET)
  target_compile_definitions(${n} PRIVATE FONT_PATH="${CMAKE_SOURCE_DIR}/fonts/Ubuntu_Mono/UbuntuMono-Regular.ttf")
  if (SDL2_FOUND)
    target_link_libraries(${n} PRIVATE SDL2::SDL2 SDL2::SDL2main OpenGL::GL)
  else ()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    target_link_libraries(${n} ${SDL2_LIBRARIES} OpenGL::GL)
    target_include_directories(${n} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_compile_options(${n} PRIVATE ${SDL2_CFLAGS_OTHER})
  endif ()
endif ()
