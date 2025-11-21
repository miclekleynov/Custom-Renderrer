//
// Created by mishka on 10.11.2025.
//

#ifndef DRAWKIT_H
#define DRAWKIT_H

#include "tgaimage.h"
#include <SimpleMath.h>
#include "Camera.h"

#include <utility>
#include "Model.h"

using vec3 = DirectX::SimpleMath::Vector3;
using vec4 = DirectX::SimpleMath::Vector4;
using mat4 = DirectX::SimpleMath::Matrix;


class Renderer {
private:
    TGAImage framebuffer_;
    TGAImage zbuffer_;

    static double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
        return .5*((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx));
    }

public:
    explicit Renderer(TGAImage framebuffer, TGAImage zbuffer) : framebuffer_(std::move(framebuffer)), zbuffer_(std::move(zbuffer)) {};

    void drawLine(int ax, int ay, int bx, int by, const TGAColor& color);
    void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int сz, const TGAColor &color);

    [[nodiscard]] std::tuple<int, int, int> worldToScreenLegacy(const vec3& v3) const;
    [[nodiscard]] std::tuple<int, int, int> worldToScreen(const vec3& v3, const Camera& camera) const;

    void drawWireframeRender(const Model& model, const Camera& camera, const TGAColor& color, const bool& show_verts = true);
    void drawRasterizationLegacy(const Model &model, const Camera& camera, const bool &show_verts = true);

    [[nodiscard]] TGAImage& GetFramebuffer() {return framebuffer_;};
    [[nodiscard]] TGAImage& GetZbuffer() {return zbuffer_;};
};



#endif //DRAWKIT_H
