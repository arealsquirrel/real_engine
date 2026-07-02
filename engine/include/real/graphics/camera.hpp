#ifndef REALLIB_CAMERA_HPP
#define REALLIB_CAMERA_HPP

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "real/core/core.hpp"
#include "real/core/types.hpp"

namespace real {

/**
 * @brief this struct is meant to be sent to shaders to show the camera data.
 */
struct CameraData {
    alignas(16) glm::mat4 proj;
    alignas(16) glm::mat4 view;
};

class REALLIB_EXPORT Camera {
    EXPOSE_TO_EDITOR

public:
    enum class Projection {
        Orthographic,
        Perspective
    };

    static constexpr glm::vec3 world_right {1.0f, 0.0f, 0.0f};
    static constexpr glm::vec3 world_up {0.0f, 1.0f, 0.0f};
    static constexpr glm::vec3 world_front {0.0f, 1.0f, 1.0f};

public:
    Camera(u32 _width=960, u32 _height=540, Projection projection=Projection::Perspective,
           float _fov=70.0f, float _near=0.01f, float _far=100.0f);

    ~Camera();

    CameraData get_camera_data() const { return data; };

    void translate_camera(glm::vec3 pos);
    void set_position(glm::vec3 pos);
    void rotate_camera(glm::vec3 rot);
    void set_rotation(glm::vec3 rot);

    void viewport(u32 _width, u32 _height);
    void change_projection(Projection projection);

private:
    void update_proj();
    void update_view();

private:
    CameraData data;
    Projection projection;
    glm::vec3 position {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation {0.0f, 0.0f, 0.0f};

    u32 width;
    u32 height;

    float fov;
    float near;
    float far;
};

}

#endif
