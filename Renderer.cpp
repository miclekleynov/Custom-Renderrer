//
// Created by mishka on 10.11.2025.
//

#include "Renderer.h"


void Renderer::drawLine(int ax, int ay, int bx, int by, const TGAColor& color) {
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

void Renderer::triangle(const int ax, const int ay, const int az, const int bx, const int by, const int bz,
    const int cx, const int cy, const int cz, const TGAColor &color) {

    int bbminx = std::min(std::min(ax, bx), cx); // левая нижняя точка
    int bbminy = std::min(std::min(ay, by), cy);
    int bbmaxx = std::max(std::max(ax, bx), cx); // правая верхняя точка
    int bbmaxy = std::max(std::max(ay, by), cy);
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    if (total_area<1) return;

    //#pragma omp parallel for
    for (int x=bbminx; x<=bbmaxx; x++) {
        for (int y=bbminy; y<=bbmaxy; y++) {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta  = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha<0 || beta<0 || gamma<0) continue; // точка вне треугольника
            unsigned char z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
            if (z <= zbuffer_.get(x, y)[0]) continue;
            zbuffer_.set(x, y, {z});
            framebuffer_.set(x, y, color);
        }
    }
}

std::tuple<int, int, int> Renderer::worldToScreen(const vec3 &v3, const Camera &camera) const {
    const mat4 mvp = camera.makeMVP();

    vec4 world(v3.x, v3.y, v3.z, 1.f);
    vec4 clip = vec4::Transform(world, mvp);
    vec4 ndc = clip / clip.w;
    vec4 screen = vec4::Transform(ndc, camera.GetViewport());

    return {
        static_cast<int>(std::round(screen.x)),
        static_cast<int>(std::round(screen.y)),
        static_cast<int>(std::round(screen.z))
        };
}

void Renderer::drawWireframeRender(const Model &model, const Camera& camera, const TGAColor& color, const bool& show_verts) {
    const auto& verts = model.getVertices();
    const auto& faces = model.getFaces();

    for (const auto& f : faces) {
        const auto& v0 = verts[f[0]];
        const auto& v1 = verts[f[1]];
        const auto& v2 = verts[f[2]];

        const auto [x0, y0, z0] = worldToScreen(v0, camera);
        const auto [x1, y1, z1] = worldToScreen(v1, camera);
        const auto [x2, y2, z2] = worldToScreen(v2, camera);

        drawLine(x0, y0, x1, y1, color);
        drawLine(x1, y1, x2, y2, color);
        drawLine(x2, y2, x0, y0, color);
    }
    if (!show_verts) {return;}
    for (const auto& v : verts) {
        const auto [x, y, z] = worldToScreen(v, camera);
        framebuffer_.set(x, y, white);
    }

}

void Renderer::drawRasterizationLegacy(const Model &model, const Camera& camera, const bool &show_verts) {
    const auto& verts = model.getVertices();
    const auto& faces = model.getFaces();

    for (const auto& f : faces) {
        const auto& v0 = verts[f[0]];
        const auto& v1 = verts[f[1]];
        const auto& v2 = verts[f[2]];

        const auto [x0, y0, z0] = worldToScreen(v0, camera);
        const auto [x1, y1, z1] = worldToScreen(v1, camera);
        const auto [x2, y2, z2] = worldToScreen(v2, camera);

        TGAColor rnd;
        for (int c=0; c<3; c++) rnd[c] = std::rand()%255;
        triangle(x0, y0, z0, x1, y1, z1, x2, y2, z2, rnd);
    }
    if (!show_verts) {return;}
    for (const auto& v : verts) {
        const auto [x, y, z] = worldToScreen(v, camera);
        framebuffer_.set(x, y, white);
    }
}
