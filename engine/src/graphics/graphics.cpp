
#include <memory>
#include <real/graphics/graphics.hpp>

namespace real {

template<typename ...Args>
Shared<Window> Graphics::create_window(Args&&... args) {
	return std::make_shared<Window>(std::forward<Args>(args)...);
}

}

