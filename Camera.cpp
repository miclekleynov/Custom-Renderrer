//
// Created by mishka on 21.11.2025.
//

#include "Camera.h"

void Camera::setModelView(const vec3 &eye, const vec3 &center, const vec3 &up) {
    vec3 z = (eye - center);
    z.Normalize();

    vec3 x = up.Cross(z);
    x.Normalize();

    vec3 y = z.Cross(x);
    y.Normalize();

    modelView_ = mat4(
        x.x, x.y, x.z, 0,
        y.x, y.y, y.z, 0,
        z.x, z.y, z.z, 0,
        -eye.Dot(x), -eye.Dot(y), -eye.Dot(z), 1
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
    viewport_ = mat4(
    w/2.f, 0,     0,     0,
    0,     h/2.f, 0,     0,
    0,     0,     1.f,   0,
    x+w/2.f, y+h/2.f, 0, 1
);
}
