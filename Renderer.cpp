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
            framebuffer_.set(static_cast<int>(y), x, color);
        else {
            framebuffer_.set(x, static_cast<int>(y), color);
        }
        y += static_cast<float>(by-ay) / static_cast<float>(bx-ax);
    }
}

void Renderer::drawRasterization(const Model &model, const Camera &camera) {
    const auto& vertices = model.getVertices();
    const auto& faces    = model.getFaces();

    // Каждый кадр нужно очищать depth-buffer
    std::fill(depthBuffer_.begin(), depthBuffer_.end(),
              -std::numeric_limits<float>::max());

    const mat4 mvp = camera.makeMVP();

    for (const auto& face : faces) {
        vec4 clipSpaceVertices[3];

        // Собираем треугольник в clip-space: MVP * (x,y,z,1)
        for (int i = 0; i < 3; ++i) {
            const auto& v = vertices[face[i]];
            const vec4 worldPosition(v.x, v.y, v.z, 1.f);
            clipSpaceVertices[i] = vec4::Transform(worldPosition, mvp);
        }

        // Случайный цвет как в курсе
        TGAColor randomColor;
        for (int c = 0; c < 3; ++c) {
            randomColor[c] = static_cast<unsigned char>(std::rand() % 255);
        }

        rasterize(clipSpaceVertices, randomColor);
    }
}

void Renderer::rasterize(const vec4 clip[3], const TGAColor &color) {
    const int width  = framebuffer_.width();
    const int height = framebuffer_.height();

    // 1. Переводим из clip space в NDC (деление на w)
    vec3 ndc[3];
    for (int i = 0; i < 3; ++i) {
        const float invW = 1.0f / clip[i].w;
        ndc[i] = vec3(
            clip[i].x * invW,
            clip[i].y * invW,
            clip[i].z * invW
        );
    }

    // 2. Перевод NDC [-1,1] в координаты экрана [0, width/height]
    //    (чистая математика viewport'а, без использования матрицы)
    vec2 screen[3];
    for (int i = 0; i < 3; ++i) {
        const float sx = (ndc[i].x + 1.0f) * 0.5f * static_cast<float>(width);
        const float sy = (ndc[i].y + 1.0f) * 0.5f * static_cast<float>(height);
        screen[i] = vec2(sx, sy);
    }

    // 3. Backface culling + проверка на вырожденный треугольник
    const double totalArea = signed_triangle_area(
        screen[0].x, screen[0].y,
        screen[1].x, screen[1].y,
        screen[2].x, screen[2].y
    );

    // |area| < 1 пиксель — треугольник почти не виден / смотрит спиной
    if (std::abs(totalArea) < 1.0) {
        return;
    }

    // 4. Bounding box треугольника
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
        return; // всё вне экрана
    }

    // 5. Обход всех пикселей внутри bounding box
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            // 5.1. Считаем барицентрические координаты точки (x, y)
            const auto px = static_cast<double>(x);
            const auto py = static_cast<double>(y);

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

            // Если хоть одна барицентрическая координата < 0 — пиксель вне треугольника
            if (alpha < 0.0 || beta < 0.0 || gamma < 0.0) {
                continue;
            }

            // 5.2. Интерполяция глубины (z) в NDC с учётом перспективы
            // Здесь ndc[*].z уже после деления на w
            const float z =
                static_cast<float>(alpha) * ndc[0].z +
                static_cast<float>(beta)  * ndc[1].z +
                static_cast<float>(gamma) * ndc[2].z;

            const int index = x + y * width;

            // 5.3. Z-buffer test: оставляем только самый "близкий" пиксель
            if (z <= depthBuffer_[index]) {
                continue; // этот фрагмент дальше, чем уже нарисованный
            }

            // 5.4. Обновляем depth-buffer
            depthBuffer_[index] = z;

            // 5.5. Пишем цвет в основной framebuffer
            framebuffer_.set(x, y, color);
        }
    }
}
