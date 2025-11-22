//
// Created by mishka on 22.11.2025.
//

#ifndef SHADERTYPES_H
#define SHADERTYPES_H

#include "MathTypes.h"
#include "tgaimage.h"

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

struct VertexOut {
    vec4 clipPos;
    vec3 worldPos;
    vec3 normal;
    vec2 uv;
};

struct FragmentIn {
    vec3 bar;
    VertexOut vertices[3];
};

struct FragmentOut {
    bool discard = false;
    TGAColor color;
};

#endif //SHADERTYPES_H
