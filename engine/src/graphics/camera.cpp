
#include "real/core/core.hpp"
#include "real/math/mat4.hpp"
#include "real/math/math_fwd.hpp"
#include <real/graphics/camera.hpp>
#include <tracy/Tracy.hpp>

namespace real {

Camera::Camera(u32 _width, u32 _height, Projection projection,
                float _fov, float _near, float _far) 
    : width(_width), height(_height), projection(projection), 
        fov(_fov), near(_near), far(_far), clear_color(1.0f, 1.0f, 1.0f, 1.0f) {

    update_proj();
    update_view();

	camera_front = world_front;
	camera_up = world_up;
	camera_right = world_right;
}

Camera::~Camera() = default;

void Camera::translate_camera(Vec3 pos) {
    position += pos;
    update_view();
}

void Camera::locate_translate(Vec3 pos) {
	position += camera_front * pos.z;
	position += camera_right * pos.x;
	position += camera_up * pos.y;
	update_view();
}

void Camera::set_camera_front(Vec3 front) {
	camera_front = front;
	camera_right = math::normalize_vec3(math::cross(camera_front, world_up));
    camera_up = math::normalize_vec3(math::cross(camera_right, camera_front));
	update_view();
}

void Camera::set_position(Vec3 pos) {
    if(pos == position)
        return;

    position = pos;
    update_view();
}

void Camera::rotate_camera(Vec3 rot) {
    rotation += rot;
    update_view();
}

void Camera::set_rotation(Vec3 rot) {
    if(rot == rotation)
        return;

    rotation = rot;
    update_view();
}

void Camera::viewport(u32 _width, u32 _height) {
    if(width != _width || height != _height) {
        width = _width;
        height = _height;
        update_proj();
    }
}

void Camera::change_projection(Projection _projection) {
    if(projection != _projection) {
        projection = _projection;
        update_proj();
    }
}

void Camera::update_proj() {
	ZoneScoped
    float aspect = (960)/(540.0f);// ((float)width) / height;

    if(projection == Projection::Perspective) {
        proj = math::perspective(math::degrees_to_radians(70.0f), aspect, 0.01f, 100.0f);
    } else {
		proj = math::ortho(aspect*5.0f, aspect*5.0f, aspect*5.0f, aspect*5.0f, near, far);
    }
}

void Camera::update_view() {
	ZoneScoped
	view = math::look_at(position, position + camera_front, camera_up);
}

}
