#ifndef REALLIB_CAMERA_HPP
#define REALLIB_CAMERA_HPP

#include "real/container/color.hpp"
#include "real/core/core.hpp"
#include "real/core/types.hpp"
#include <real/math/math.hpp>

namespace real {

/**
 * @brief this struct is meant to be sent to shaders to show the camera data.
 */

class REALLIB_EXPORT Camera {
    EXPOSE_TO_EDITOR

public:
    enum class Projection {
        Orthographic,
        Perspective
    };

    static constexpr Vec3 world_right {1.0f, 0.0f, 0.0f};
    static constexpr Vec3 world_up    {0.0f, 1.0f, 0.0f};
    static constexpr Vec3 world_front {0.0f, 0.0f, 1.0f};

public:
    Camera(u32 _width=960, u32 _height=540, Projection projection=Projection::Perspective,
           float _fov=70.0f, float _near=0.01f, float _far=100.0f);

    ~Camera();

    // CameraData get_camera_data() const { return data; };

    void translate_camera(Vec3 pos);
    void set_position(Vec3 pos);
    void rotate_camera(Vec3 rot);
    void set_rotation(Vec3 rot);

	void set_camera_front(Vec3 front);

	// translates local to the cameras view
	void locate_translate(Vec3 pos);

    void viewport(u32 _width, u32 _height);
    void change_projection(Projection projection);

private:
    void update_proj();
    void update_view();

public:
	Color4 clear_color;
	Mat4 view;
	Mat4 proj;

private:
    // CameraData data;
    Projection projection;
    Vec3 position {0.0f, 0.0f, 0.0f};
    Vec3 rotation {0.0f, 0.0f, 0.0f};

	Vec3 camera_front;
	Vec3 camera_right;
	Vec3 camera_up;

    u32 width;
    u32 height;

    float fov;
    float near;
    float far;
};

}

#endif
