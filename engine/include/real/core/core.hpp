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


#define STRINGIFY(x) #x
#define STRINGIFY_EXP(x) STRINGIFY(x)

#define REAL_ENTRY extern Unique<Game> game_entrypoint(Shared<Instance> instance);

#ifdef REALLIB_EDITOR

namespace editor {
class Panel;
class Editor;
class PanelResourceDatabase;
}

#define EXPOSE_TO_EDITOR \
	friend editor::Panel; \
	friend editor::Editor; \
	friend editor::PanelResourceDatabase


#else
#define EXPOSE_TO_EDITOR
#endif

#endif
