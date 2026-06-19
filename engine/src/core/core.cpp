
#include <real/core/core.hpp>
#include <fmt/format.h>
#include <argparse.h>
#include <string>

namespace real {

void print_version() {
    
}

ArgParams parse_args(int argc, char **argv) {
    using namespace argparse;
    Parser parser;
    auto window_width = parser.AddArg<int>("window_width", 'w', "The width of the rendering framebuffer");
    auto window_height = parser.AddArg<int>("window_height", 'h', "The height of the rendering framebuffer");
    auto game_dll_path = parser.AddArg<std::string>("game", 'g', "the path to the game to be loaded").Required();
    parser.ParseArgs(argc, argv);

    ArgParams ar;
    if(window_width) ar.window_width = *window_width; else ar.window_width = 960;
    if(window_height) ar.window_height = *window_height; else ar.window_height = 540;
    if(game_dll_path) ar.game_dll_path = *game_dll_path;

    return ar;
}

}

