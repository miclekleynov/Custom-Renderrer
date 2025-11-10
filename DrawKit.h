//
// Created by mishka on 10.11.2025.
//

#ifndef DRAWKIT_H
#define DRAWKIT_H

#include "tgaimage.h"
#include <SimpleMath.h>

#include <utility>
#include "Model.h"

using namespace DirectX::SimpleMath;



class DrawKit {
private:
    TGAImage framebuffer_;

public:
    explicit DrawKit(TGAImage framebuffer) : framebuffer_(std::move(framebuffer)) {};

    void drawLine(int ax, int ay, int bx, int by, const TGAColor& color);
    void triangle(int ax, int ay, int bx, int by, int cx, int cy, const TGAColor& color);

    [[nodiscard]] std::tuple<int, int> worldToScreen(const Vector3& v3) const;

    void drawModel(const Model& model, const TGAColor& color, const bool& show_verts = true);

    TGAImage& GetFramebuffer() {return framebuffer_;};
};



#endif //DRAWKIT_H
