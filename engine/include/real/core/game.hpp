#ifndef REALLIB_GAME_HPP
#define REALLIB_GAME_HPP

#include "real/core/event_listener.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_database.hpp"
#include <optional>
#include <real/core/core.hpp>

namespace real {

class Instance;
class Game;
class Framebuffer;
class Scene;

typedef real::Game* create_real_game_f(Shared<real::Instance> instance, ArgParams params);
typedef void destroy_real_game_f(real::Game*);

/**
 * @brief the struct that contains the system dll handle, and
 * function pointers for loading and unloading the game*
 */
struct DLLGameLoad {
	void* game_dll_handle = nullptr;
	create_real_game_f *create_game;
	destroy_real_game_f *destroy_game;
};

/**
 * @brief Exposes a virtual class that the user will hook into via DLL injection to actualy program
 */
class REALLIB_EXPORT Game : public EventListener {
public:
	Game(Shared<Instance> _instance, ArgParams params);
	virtual ~Game();

public:
	/**
	 * @brief the function called once on start by a real application
	 */
	virtual void start() = 0;

	/**
	 * @brief called every frame by a real application
	 * 
	 * @param delta_time the time in microseconds between this frame and the last frame
	 */
	virtual void update(u32 delta_time) = 0;

	virtual void shutdown() = 0;

public:

	/**
	 * @brief loads an instance of the game class from a dll
	 * 
	 * @param instance A created instance class handle to be given to the game
	 * @return std::pair<Game*, DLLGameLoad> a pointer to the created game class and function pointers for creation and deletion functions for that dll
	 */
	static std::pair<Game*, DLLGameLoad> load_game_dll(Shared<Instance> instance, ArgParams params);

	/**
	 * @brief destroys a Game* from a dll
	 * 
	 * @param game the game to be destroyed
	 * @param load the load struct containing the destroy function
	 */
	static void destroy_game_dll(Game* game, DLLGameLoad load);

public:
	Shared<Instance> instance;
	Shared<ResourceDatabase> resource_database;
	Shared<Window> window;
    Shared<Renderer> renderer;
	Shared<Framebuffer> screen_framebuffer;
	Shared<Scene> scene;
};

}

#define EXPOSE_GAME_TO_REAL(GameName) \
		extern "C" { \
			REALLIB_EXPORT \
			Game *game_create(Shared<Instance> instance, ArgParams params) { \
				return new MyGame(instance, params); \
			} \
			REALLIB_EXPORT \
			void game_destroy(Game *game) { \
				delete (GameName*)game; \
			} \
		}

#endif
