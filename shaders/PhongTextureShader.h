//
// Created by mishka on 04.12.2025.
//

#ifndef PHONGTEXTURESHADER_H
#define PHONGTEXTURESHADER_H

#include <algorithm> // std::clamp
#include <cmath>     // std::max, std::pow

#include "../realization/IShader.h"
#include "../realization/Camera.h"
#include "../realization/MathTypes.h"
#include "../tga/tgaimage.h"

class PhongTextureShader final : public IShader {
public:
    PhongTextureShader(const Camera& camera,
                       const TGAImage& diffuseMap,
                       vec3 lightDir,
                       vec3 lightColor        = vec3(1.0f, 1.0f, 1.0f),
                       float ambientStrength  = 0.1f,
                       float specularStrength = 0.5f,
                       float shininess        = 32.0f);

    VertexOut vertex(const VertexIn& in) override;
    [[nodiscard]] FragmentOut fragment(const FragmentIn& in) const override;

private:
    const Camera&   camera_;
    const TGAImage& diffuseMap_;

    vec3 lightDir_;     // нормализованное направление света
    vec3 lightColor_;

    float ambientStrength_;
    float specularStrength_;
    float shininess_;

    [[nodiscard]] TGAColor sampleDiffuse(const vec2& uv) const;
};


#endif //PHONGTEXTURESHADER_H
