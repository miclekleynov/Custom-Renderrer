//
// Created by mishka on 10.11.2025.
//

#ifndef DRAWKIT_H
#define DRAWKIT_H

#include "tgaimage.h"
#include <SimpleMath.h>
#include "Camera.h"
#include <vector>
#include <limits>
#include <algorithm>
#include <utility>
#include "Model.h"

using vec2 = DirectX::SimpleMath::Vector2;
using vec3 = DirectX::SimpleMath::Vector3;
using vec4 = DirectX::SimpleMath::Vector4;
using mat4 = DirectX::SimpleMath::Matrix;


class Renderer {
private:
    TGAImage framebuffer_;
    TGAImage zbuffer_;
    std::vector<float> depthBuffer_;

    static double signed_triangle_area(double ax, double ay,
                                       double bx, double by,
                                       double cx, double cy) {
        return .5 * ((by - ay) * (bx + ax)
                   + (cy - by) * (cx + bx)
                   + (ay - cy) * (ax + cx));
    }

public:
    explicit Renderer(TGAImage framebuffer, TGAImage zbuffer)
    : framebuffer_(std::move(framebuffer))
    , zbuffer_(std::move(zbuffer))
    , depthBuffer_(framebuffer_.width() * framebuffer_.height(),
                   -std::numeric_limits<float>::max()) {}


    void drawLine(int ax, int ay, int bx, int by, const TGAColor& color);
    void drawRasterization(const Model& model, const Camera& camera);
    void rasterize(const vec4 clip[3], const TGAColor& color);

    [[nodiscard]] TGAImage& GetFramebuffer() {return framebuffer_;};
    [[nodiscard]] TGAImage& GetZbuffer() {return zbuffer_;};
};



#endif //DRAWKIT_H
