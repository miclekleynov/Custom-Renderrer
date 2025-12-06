//
// Created by mishka on 22.11.2025.
//

#ifndef SHADERTYPES_H
#define SHADERTYPES_H

#include "MathTypes.h"
#include "../tga/tgaimage.h"

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 uv;
};

struct VertexIn {
    vec3 position;
    vec3 normal;
    vec2 uv;
};

// Выход из вершинного шейдера
struct VertexOut {
    vec4 clipPos{};   // позиция в clip space (после MVP)
    vec3 worldPos{};  // позиция в мировом пространстве (по желанию шейдера)
    vec3 normal{};    // нормаль в мировом/видовом пространстве (по желанию)
    vec2 uv{};        // координаты текстуры
};

// Вход во фрагментный шейдер
struct FragmentIn {
    vec3      bar{};              // барицентрические координаты
    VertexOut vertices[3]{};      // три вершины треугольника
    vec2      screenPos{};        // экранные координаты фрагмента (x, y)
};

// Выход из фрагментного шейдера
struct FragmentOut {
    bool    discard = false;
    TGAColor color{};
};

#endif //SHADERTYPES_H
