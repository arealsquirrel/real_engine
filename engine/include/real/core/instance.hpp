#ifndef REALLIB_CORE_INSTANCE
#define REALLIB_CORE_INSTANCE

#include "real/core/logging.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"

namespace real {

/**
 * @brief defines an instance of the engine that manages
 * all the parts of running the engine
 */
class Instance {
public:
    Instance();
    ~Instance();

    void init(const WindowInfo &window_info);

    /**
     * @brief updates the instance of the enging
     * 
     * @returns if the engine should close
     */
    bool update();

public:
    Log log;
    Window *window;
    Renderer *renderer;
};

}

#endif