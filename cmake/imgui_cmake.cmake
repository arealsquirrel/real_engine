cmake_minimum_required(VERSION 4.0)

project(imgui_cmake_wrapper)

set(IMGUI_RENDERER_BACKEND
    "opengl3"
    CACHE
      STRING
      "Allows useres to choose which renderer(e.g. OpenGL2,OpenGL3,Vulkan) they want to build imgui for"
)

set(IMGUI_PLATFORM_BACKEND
    "glfw"
    CACHE
      STRING
      "Allows useres to choose which platform(e.g. glfw,SDL2,SDL3) they want to build imgui for"
)

CPMAddPackage(
  NAME imgui
  GITHUB_REPOSITORY ocornut/imgui
  GIT_TAG "docking"  # Specify your branch here
  VERSION 1.92.4
  DOWNLOAD_ONLY)

add_library(imgui_cmake_wrapper SHARED)

target_sources(
  imgui_cmake_wrapper
  PRIVATE ${imgui_SOURCE_DIR}/imgui.cpp
          ${imgui_SOURCE_DIR}/imgui_draw.cpp
          ${imgui_SOURCE_DIR}/imgui_demo.cpp
          ${imgui_SOURCE_DIR}/imgui_tables.cpp
          ${imgui_SOURCE_DIR}/imgui_widgets.cpp
          ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp)

if(${IMGUI_RENDERER_BACKEND} STREQUAL "opengl3")
  message(STATUS "Build imgui for OpenGL3")
  target_sources(imgui_cmake_wrapper
                 PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
  find_package(OpenGL 3 EXACT REQUIRED)
  target_link_libraries(imgui_cmake_wrapper PUBLIC OpenGL::OpenGL)
elseif(${IMGUI_RENDERER_BACKEND} STREQUAL "vulkan")
  message(STATUS "Build imgui for Vulkan")
  target_sources(imgui_cmake_wrapper
                 PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)
  find_package(Vulkan REQUIRED)
  target_link_libraries(imgui_cmake_wrapper PUBLIC Vulkan::Vulkan)
endif()

if(${IMGUI_PLATFORM_BACKEND} STREQUAL "glfw")
  message(STATUS "Build imgui for glfw")

  target_sources(imgui_cmake_wrapper
                 PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)

  target_link_libraries(imgui_cmake_wrapper PUBLIC glfw)
endif()


target_include_directories(
  imgui_cmake_wrapper PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends
                             ${imgui_SOURCE_DIR}/misc/cpp)