
project(app)

file(GLOB_RECURSE SRCS src/*.cpp)

# Create the executable
add_executable(app ${SRCS})

target_link_libraries(app
    reallib
)

# target_compile_options(app PRIVATE -fsanitize=address)
# target_link_options(app PRIVATE -fsanitize=address)
