#ifndef REALLIB_GAME_HPP
#define REALLIB_GAME_HPP

#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_database.hpp"
#include <optional>
#include <real/core/core.hpp>

namespace real {

class Instance;
class Game;

typedef real::Game* create_real_game_f(Shared<real::Instance> instance);
typedef void destroy_real_game_f(real::Game*);

struct DLLGameLoad {
	void* game_dll_handle = nullptr;
	create_real_game_f *create_game;
	destroy_real_game_f *destroy_game;
};

class REALLIB_EXPORT Game {
public:
	Game(Shared<Instance> _instance);
	~Game();

public:
	virtual void start() = 0;
	virtual void update(u32 delta_time) = 0;
	virtual void render(real::FrameContext frame) = 0;

public:
	static std::pair<Game*, DLLGameLoad> load_game_dll(Shared<Instance> instance);
	static void destroy_game_dll(Game* game, DLLGameLoad load);

public:
	Shared<Instance> instance;

	Shared<ResourceDatabase> resource_database;
	Shared<Window> window;
    Shared<Renderer> renderer;
};


}

#endif
