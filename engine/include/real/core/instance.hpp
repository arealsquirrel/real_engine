#ifndef REALLIB_INSTANCE_HPP
#define REALLIB_INSTANCE_HPP

#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_database.hpp"
#include <real/core/core.hpp>

namespace real {

class REALLIB_EXPORT Instance {
public:
	Instance();
	~Instance();

public:
	bool should_close();

public:
	Shared<ResourceDatabase> resource_database;
	Shared<Window> window;
    Shared<Renderer> renderer;
};

}

#endif