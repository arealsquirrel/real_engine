
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include <real/graphics/camera.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace real {

Camera::Camera(u32 _width, u32 _height, Projection projection,
                float _fov, float _near, float _far) 
    : width(_width), height(_height), projection(projection), 
        fov(_fov), near(_near), far(_far) {

    update_proj();
    update_view();

	camera_front = world_front;
	camera_up = world_up;
	camera_right = world_right;
}

Camera::~Camera() = default;

void Camera::translate_camera(glm::vec3 pos) {
    position += pos;
    update_view();
}

void Camera::locate_translate(glm::vec3 pos) {
	position += camera_front * pos.z;
	position += camera_right * pos.x;
	position += camera_up * pos.y;
	update_view();
}

void Camera::set_camera_front(glm::vec3 front) {
	camera_front = front;
	camera_right = glm::normalize(glm::cross(camera_front, world_up));
    camera_up = glm::normalize(glm::cross(camera_right, camera_front));
	update_view();
}

void Camera::set_position(glm::vec3 pos) {
    if(pos == position)
        return;

    position = pos;
    update_view();
}

void Camera::rotate_camera(glm::vec3 rot) {
    rotation += rot;
    update_view();
}

void Camera::set_rotation(glm::vec3 rot) {
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
    float aspect = ((float)width) / height;

    glm::mat4 proj(1.0f);
    if(projection == Projection::Perspective) {
        data.proj = glm::perspective(glm::radians(fov), aspect, 0.01f, 100.0f);
    } else {
        data.proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, near, far);
    }
}

void Camera::update_view() {
	/*
    glm::mat4 view(1.0f);
    view = glm::translate(view, position);
    glm::mat4 rot = glm::toMat4(glm::quat(rotation));
    data.view = view * rot;
	*/

	data.view = glm::lookAt(position, position + camera_front, camera_up);
}

}
