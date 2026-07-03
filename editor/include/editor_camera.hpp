#ifndef EDITOR_CAMERA_HPP
#define EDITOR_CAMERA_HPP

#include "real/core/instance.hpp"
#include "real/graphics/camera.hpp"
#include <memory>

namespace editor {

/**
 * a wrapper around the camera class that allows us to move around
 * from user input!
 */
class EditorCamera {
public:
	EditorCamera(real::Instance *instance);
	~EditorCamera() = default;

	void update(u32 delta_time);

public:
	bool block_input {false};
	float camera_speed {0.1f};
	float mouse_speed {0.1f};
	std::shared_ptr<real::Camera> camera;

private:
	real::Instance *instance;
	double mouse_x;
	double mouse_y;

	double delta_x;
	double delta_y;

	float yaw {90.0f};
	float pitch {0.0f};
};

}

#endif
