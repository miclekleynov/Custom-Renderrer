//
// Created by mishka on 10.11.2025.
//

#include "DrawKit.h"


void DrawKit::drawLine(int ax, int ay, int bx, int by, const TGAColor& color) {
    const bool steep = std::abs(ax-bx) < std::abs(ay-by); // true, если линия крутая (дельта y больше дельты x)
    if (steep) { // транспонируем (ax -> ay, ay -> ax)
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax>bx) { // если у нас первая точка левее второй, меняем их, чтобы работал алгоритм из цикла
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    auto y = static_cast<float>(ay);
    for (int x=ax; x<=bx; x++) {
        if (steep) // если транспонировали, меняем обратно
            framebuffer_.set(y, x, color);
        else {
            framebuffer_.set(x, y, color);
        }
        y += static_cast<float>(by-ay) / static_cast<float>(bx-ax);
    }
}

void DrawKit::triangle(const int ax, const int ay, const int bx, const int by, const int cx, const int cy, const TGAColor &color) {
    drawLine(ax, ay, bx, by, color);
    drawLine(bx, by, cx, cy, color);
    drawLine(cx, cy, ax, ay, color);
}

std::tuple<int, int> DrawKit::worldToScreen(const Vector3 &v3) const {
    return {
        (v3.x + 1.) * framebuffer_.width() / 2,
        (v3.y + 1.) * framebuffer_.height() / 2
    };
}

void DrawKit::drawModel(const Model &model, const TGAColor& color, const bool& show_verts) {
    const auto& verts = model.getVertices();
    const auto& faces = model.getFaces();

    for (const auto& f : faces) {
        const auto& v0 = verts[f[0]];
        const auto& v1 = verts[f[1]];
        const auto& v2 = verts[f[2]];

        const auto [x0, y0] = worldToScreen(v0);
        const auto [x1, y1] = worldToScreen(v1);
        const auto [x2, y2] = worldToScreen(v2);

        drawLine(x0, y0, x1, y1, color);
        drawLine(x1, y1, x2, y2, color);
        drawLine(x2, y2, x0, y0, color);
    }
    if (!show_verts) {return;}
    for (const auto& v : verts) {
        const auto [x, y] = worldToScreen(v);
        framebuffer_.set(x, y, white);
    }

}
