
project(examples)

file(GLOB_RECURSE SRCS src/*.cpp)
add_library(app SHARED ${SRCS})
target_compile_options(app PRIVATE -fPIC -fvisibility=default)
target_include_directories( ${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(app
    reallib
)

