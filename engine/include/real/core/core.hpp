#ifndef REALLIB_CORE_HPP
#define REALLIB_CORE_HPP

#include <functional>

namespace real {

/**
 * @brief basic sanity check for the library.
 */
void print_version();

class Object;

template<typename T>
using EventFunctionPtr = std::function<void(T &event, Object *from)>;

}

#define BIT(n) 1<<n
#define CHECK_FLAG(x, n) ((((u32)x) & ((u32)n)) != 0)


#define STRINGIFY(x) #x
#define STRINGIFY_EXP(x) STRINGIFY(x)

#define REAL_ENTRY extern Unique<::real::Game> game_entrypoint();

#ifdef REALLIB_EDITOR

namespace editor {
class Panel;
class Editor;
class PanelResourceDatabase;
class PanelResourceViewer;
}

#define EXPOSE_TO_EDITOR \
	friend editor::Panel; \
	friend editor::Editor; \
	friend editor::PanelResourceDatabase; \
	friend editor::PanelResourceViewer;


#else
#define EXPOSE_TO_EDITOR
#endif

#ifdef __linux__

#define REALLIB_EXPORT __attribute__((visibility("default")))

#else
#error "trash"
#endif

#endif
