
project(real_editor)

file(GLOB_RECURSE SRCS src/*.cpp)

add_executable(real_editor ${SRCS})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# Syntax: target_compile_definitions(<target> <PRIVATE|PUBLIC|INTERFACE> <definition>...)
target_compile_definitions(${PROJECT_NAME}
	PRIVATE REALLIB_EDITOR)

target_link_libraries(${PROJECT_NAME}
	reallib
	app
)

