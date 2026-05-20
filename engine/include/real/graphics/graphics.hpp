#ifndef REALLIB_GRAPHICS_HPP
#define REALLIB_GRAPHICS_HPP

#include "real/core/types.hpp"
#include "real/graphics/window.hpp"

namespace real {

typedef void* GraphicsBackend;

struct GraphicsInfo {

};

/**
 * @brief contains static methods to work with graphics backends to setup static one-instance
 * level thingies.
 */
class Graphics {
public:
    static void init_backend(const GraphicsInfo &info);
    static GraphicsBackend get_backend();

    static void destroy_backend();

	template<typename ...Args>
	static Shared<Window> create_window(Args&&... args);
};

}

#endif
