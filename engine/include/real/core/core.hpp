#ifndef REALLIB_CORE_HPP
#define REALLIB_CORE_HPP

#include "real/core/types.hpp"
#include <filesystem>
#include <functional>
#include <cassert>

#ifdef __linux__
#define REALLIB_EXPORT __attribute__((visibility("default")))
#else
#error "trash"
#endif

#define BIT(n) 1<<n
#define CHECK_FLAG(x, n) ((((u32)x) & ((u32)n)) != 0)
#define STRINGIFY(x) #x
#define STRINGIFY_EXP(x) STRINGIFY(x)
#define REAL_ENTRY extern Unique<::real::Game> game_entrypoint();
#define RL_ASSERT(cond, ...) assert(cond)
#define RL_MEM_POINTER(var) reinterpret_cast<char*>(var)
#define RL_CAST_MEM(var, type) reinterpret_cast<type*>(var)

namespace real {

struct REALLIB_EXPORT ArgParams {
	u32 window_width;
	u32 window_height;
	std::filesystem::path game_dll_path;
};

ArgParams REALLIB_EXPORT parse_args(int argc, char **argv);

class Object;

template<typename T>
using EventFunctionPtr = std::function<void(T &event, Object *from)>;

}


#ifdef REALLIB_EDITOR

namespace editor {
class Panel;
class Editor;
class PanelResourceDatabase;
class PanelResourceViewer;
class PanelSceneView;
}

#define EXPOSE_TO_EDITOR \
	friend editor::Panel; \
	friend editor::Editor; \
	friend editor::PanelResourceDatabase; \
	friend editor::PanelResourceViewer; \
	friend editor::PanelSceneView;


#else
#define EXPOSE_TO_EDITOR
#endif

#endif
