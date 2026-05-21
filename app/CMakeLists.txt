
project(app)

file(GLOB_RECURSE SRCS src/*.cpp)

# Create the executable
add_library(app ${SRCS})

target_link_libraries(app
    reallib
)

