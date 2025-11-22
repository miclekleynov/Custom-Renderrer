//
// Created by mishka on 21.11.2025.
//

#include "Camera.h"

void Camera::setModelView(const vec3& eye,
                          const vec3& center,
                          const vec3& up)
{
    vec3 z = eye - center; // forward (от центра к камере)
    z.Normalize();

    vec3 x = up.Cross(z);  // right
    x.Normalize();

    vec3 y = z.Cross(x);   // up (ортогонализованный)
    y.Normalize();

    const float tx = -eye.Dot(x);
    const float ty = -eye.Dot(y);
    const float tz = -eye.Dot(z);

    modelView_ = mat4(
        x.x, x.y, x.z, 0.f,
        y.x, y.y, y.z, 0.f,
        z.x, z.y, z.z, 0.f,
        tx,  ty,  tz,  1.f
    );
}

void Camera::setPerspective(const float f) {
    perspective_ = mat4(
            1.f, 0.f, 0.f,    0.f,
            0.f, 1.f, 0.f,    0.f,
            0.f, 0.f, 1.f,    0.f,
            0.f, 0.f, -1.f / f, 1.f
        );
}

void Camera::setViewport(const int x, const int y, const int w, const int h) {
    const float sx = w * 0.5f;
    const float sy = h * 0.5f;
    const float tx = static_cast<float>(x) + sx; // центр по X
    const float ty = static_cast<float>(y) + sy; // центр по Y

    // В SimpleMath векторы — строки, поэтому:
    // масштаб в диагонали, сдвиг в четвертой строке.
    viewport_ = mat4(
        sx,  0.f, 0.f, 0.f,
        0.f, sy,  0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        tx,  ty,  0.f, 1.f
    );
}
