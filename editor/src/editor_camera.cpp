
#include "real/graphics/camera.hpp"
#include "real/math/math_fwd.hpp"
#include <GLFW/glfw3.h>
#include <editor_camera.hpp>
#include <memory>
#include <real/graphics/window.hpp>

namespace editor {

EditorCamera::EditorCamera(real::Instance *_instance)
	: instance(_instance) {

	camera = std::make_shared<real::Camera>();

	auto [xpos, ypos] = instance->window->get_mouse_position();
	mouse_x = xpos;
	mouse_y = ypos;
}

void EditorCamera::update(u32 delta_time) {
	auto [xpos, ypos] = instance->window->get_mouse_position();
	delta_x = mouse_x - xpos;
	delta_y = mouse_y - ypos;
	mouse_x = xpos;
	mouse_y = ypos;

	if(block_input) {
		return;
	}

	float forward = 0.0f;
	float right = 0.0f;

	forward += (instance->window->input->poll_key(GLFW_KEY_W)) ? camera_speed : 0.0f;
	forward -= (instance->window->input->poll_key(GLFW_KEY_S)) ? camera_speed : 0.0f;
	right += (instance->window->input->poll_key(GLFW_KEY_D)) ? camera_speed : 0.0f;
	right -= (instance->window->input->poll_key(GLFW_KEY_A)) ? camera_speed : 0.0f;

    yaw -= delta_x * mouse_speed;
    pitch -= delta_y * mouse_speed;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	real::Vec3 direction;
	direction.x = cos(real::math::degrees_to_radians(yaw)) * cos(real::math::degrees_to_radians(pitch));
	direction.y = sin(real::math::degrees_to_radians(pitch));
	direction.z = sin(real::math::degrees_to_radians(yaw)) * cos(real::math::degrees_to_radians(pitch));
	direction = real::math::normalize_vec3(direction);
	camera->set_camera_front(direction);
	camera->locate_translate(real::Vec3{right, 0.0, forward});
}

}

