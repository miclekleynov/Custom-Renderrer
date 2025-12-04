//
// Created by mishka on 22.11.2025.
//

#include "FlatRandomColorShader.h"

#include <bit>      // std::bit_cast
#include <cstdint>  // uint32_t

using std::uint32_t;

namespace {
    // Простая 32-битная хэш-функция
    uint32_t hash32(uint32_t x) {
        x += 0x9e3779b9u;
        x ^= x >> 15;
        x *= 0x85ebca6bu;
        x ^= x >> 13;
        x *= 0xc2b2ae35u;
        x ^= x >> 16;
        return x;
    }

    // Хэш от vec3 по битам float-компонент
    uint32_t hashVec3(const vec3& v) {
        const auto hx = std::bit_cast<uint32_t>(v.x);
        const auto hy = std::bit_cast<uint32_t>(v.y);
        const auto hz = std::bit_cast<uint32_t>(v.z);

        uint32_t h = hash32(hx);
        h ^= hash32(hy + 0x9e3779b9u + (h << 6) + (h >> 2));
        h ^= hash32(hz + 0x9e3779b9u + (h << 6) + (h >> 2));
        return h;
    }

    // Хэш всего треугольника по трём мировым позициям
    uint32_t hashTriangle(const VertexOut (&v)[3]) {
        const uint32_t h0 = hashVec3(v[0].worldPos);
        const uint32_t h1 = hashVec3(v[1].worldPos);
        const uint32_t h2 = hashVec3(v[2].worldPos);

        uint32_t h = h0;
        h ^= h1 + 0x9e3779b9u + (h << 6) + (h >> 2);
        h ^= h2 + 0x9e3779b9u + (h << 6) + (h >> 2);
        return h;
    }
} // namespace

FlatRandomColorShader::FlatRandomColorShader(const Camera& camera)
    : mvp_(camera.getViewProjection()) // матрица Projection * View
{}

VertexOut FlatRandomColorShader::vertex(const VertexIn& in) {
    VertexOut out{};

    // Положение в мире (у нас модель пока без отдельной Model-матрицы)
    out.worldPos = in.position;
    out.normal   = in.normal;
    out.uv       = in.uv;

    // clip-позиция = MVP * (x, y, z, 1)
    const vec4 world(in.position.x, in.position.y, in.position.z, 1.0f);
    out.clipPos = vec4::Transform(world, mvp_);

    return out;
}

FragmentOut FlatRandomColorShader::fragment(const FragmentIn& in) const {
    FragmentOut out{};

    // Один цвет на треугольник: хэшируем все три вершины
    const uint32_t h = hashTriangle(in.vertices);

    const auto r = static_cast<unsigned char>( h        & 0xFFu);
    const auto g = static_cast<unsigned char>((h >> 8 ) & 0xFFu);
    const auto b = static_cast<unsigned char>((h >> 16) & 0xFFu);

    out.color   = {r, g, b, 255};
    out.discard = false;
    return out;
}