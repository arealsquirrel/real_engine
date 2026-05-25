
project(real_editor)

file(GLOB_RECURSE SRCS src/*.cpp)

add_executable(real_editor ${SRCS})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
	reallib
	app
)

