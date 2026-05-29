#ifndef REALLIB_CORE_HPP
#define REALLIB_CORE_HPP

namespace real {

/**
 * @brief basic sanity check for the library.
 */
void print_version();

#define RL_CLASS(name) public: \
	virtual const char *get_class_name() { return #name; }

}

#define BIT(n) 1<<n
#define CHECK_FLAG(x, n) ((((u32)x) & ((u32)n)) != 0)


#define STRINGIFY(x) #x
#define STRINGIFY_EXP(x) STRINGIFY(x)

#define REAL_ENTRY extern Unique<Game> game_entrypoint(Shared<Instance> instance);

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
