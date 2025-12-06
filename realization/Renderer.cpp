//
// Created by mishka on 10.11.2025.
//

#include "Renderer.h"


double Renderer::signed_triangle_area(double ax, double ay, double bx, double by, double cx, double cy) {
    return .5 * ((by - ay) * (bx + ax)
                 + (cy - by) * (cx + bx)
                 + (ay - cy) * (ax + cx));
}

Renderer::Renderer(TGAImage framebuffer, TGAImage zbuffer): framebuffer_(std::move(framebuffer))
                                                            , zbuffer_(std::move(zbuffer))
                                                            , depthBuffer_(framebuffer_.width() * framebuffer_.height(),
                                                                           -std::numeric_limits<float>::max()) {

}

void Renderer::clearDepth() {
    std::ranges::fill(depthBuffer_
                      ,
                      -std::numeric_limits<float>::max()
    );
}

void Renderer::drawModel(const Model &model) {
    assert(camera_ && "Renderer::drawModel: camera is not set");
    assert(shader_ && "Renderer::drawModel: shader is not set");

    const auto& vertices = model.getVertices();
    const auto& faces    = model.getFaces();

    for (const auto& face : faces) {
        VertexOut vOut[3];

        for (int i = 0; i < 3; ++i) {
            const auto&[position, normal, uv] = vertices[face[i]];

            VertexIn vin{};
            vin.position = position;
            vin.normal   = normal;
            vin.uv       = uv;

            // vertex() обязан вернуть clipPos (и worldPos, normal, uv при желании)
            vOut[i] = shader_->vertex(vin);
        }

        // 2. Растеризуем треугольник
        rasterizeTriangle(vOut);
    }
}

void Renderer::rasterizeTriangle(const VertexOut (&verts)[3]) {
    assert(camera_ && "Renderer::rasterizeTriangle camera is not set");
    assert(shader_ && "Renderer::rasterizeTriangle is not set");

    const int width  = framebuffer_.width();
    const int height = framebuffer_.height();

    // 1. Clip → NDC (делим на w)
    vec3 ndc[3];
    for (int i = 0; i < 3; ++i) {
        const vec4& c = verts[i].clipPos;
        const float invW = 1.0f / c.w;
        ndc[i] = vec3(
            c.x * invW,
            c.y * invW,
            c.z * invW
        );
    }

    // 2. NDC → Screen через viewport-матрицу камеры
    vec2 screen[3];
    const mat4& viewport = camera_->getViewport(); // getViewport() есть в Camera

    for (int i = 0; i < 3; ++i) {
        vec4 ndc4(ndc[i].x, ndc[i].y, ndc[i].z, 1.0f);
        vec4 s = vec4::Transform(ndc4, viewport);
        screen[i] = vec2(s.x, s.y);
    }

    // 3. Площадь треугольника в screen space (backface + вырожденные)
    const double totalArea = signed_triangle_area(
        screen[0].x, screen[0].y,
        screen[1].x, screen[1].y,
        screen[2].x, screen[2].y
    );

    if (std::abs(totalArea) < 1.0) {
        return; // почти невидимый или вырожденный
    }

    // 4. Bounding box
    const float minXf = std::min({screen[0].x, screen[1].x, screen[2].x});
    const float maxXf = std::max({screen[0].x, screen[1].x, screen[2].x});
    const float minYf = std::min({screen[0].y, screen[1].y, screen[2].y});
    const float maxYf = std::max({screen[0].y, screen[1].y, screen[2].y});

    int minX = static_cast<int>(std::floor(minXf));
    int maxX = static_cast<int>(std::ceil (maxXf));
    int minY = static_cast<int>(std::floor(minYf));
    int maxY = static_cast<int>(std::ceil (maxYf));

    // Обрезаем по границам экрана
    minX = std::max(minX, 0);
    minY = std::max(minY, 0);
    maxX = std::min(maxX, width  - 1);
    maxY = std::min(maxY, height - 1);

    if (minX > maxX || minY > maxY) {
        return;
    }

    // 5. Обход пикселей
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            const double px = static_cast<double>(x);
            const double py = static_cast<double>(y);

            const double alpha = signed_triangle_area(
                px, py,
                screen[1].x, screen[1].y,
                screen[2].x, screen[2].y
            ) / totalArea;

            const double beta = signed_triangle_area(
                screen[0].x, screen[0].y,
                px, py,
                screen[2].x, screen[2].y
            ) / totalArea;

            const double gamma = 1.0 - alpha - beta;

            if (alpha < 0.0 || beta < 0.0 || gamma < 0.0) {
                continue;
            }

            // Интерполяция глубины (по ndc.z)
            const float z =
                static_cast<float>(alpha) * ndc[0].z +
                static_cast<float>(beta)  * ndc[1].z +
                static_cast<float>(gamma) * ndc[2].z;

            const int index = x + y * width;
            if (z <= depthBuffer_[index]) {
                continue;
            }

            depthBuffer_[index] = z;

            // Сборка входа для фрагментного шейдера
            FragmentIn fin{};
            fin.bar = vec3(
                static_cast<float>(alpha),
                static_cast<float>(beta),
                static_cast<float>(gamma)
            );
            fin.vertices[0] = verts[0];
            fin.vertices[1] = verts[1];
            fin.vertices[2] = verts[2];

            // НОВОЕ: экранные координаты текущего фрагмента
            fin.screenPos = vec2(
                static_cast<float>(x),
                static_cast<float>(y)
            );

            FragmentOut fout = shader_->fragment(fin);
            if (!fout.discard) {
                framebuffer_.set(x, y, fout.color);
            }
        }
    }
}
