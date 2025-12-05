//
// Created by mishka on 04.12.2025.
//

#include "PhongTextureShader.h"

#include "PhongTextureShader.h"

PhongTextureShader::PhongTextureShader(const Camera& camera,
                                       const TGAImage& diffuseMap,
                                       vec3 lightDir,
                                       vec3 lightColor,
                                       float ambientStrength,
                                       float specularStrength,
                                       float shininess)
    : camera_{camera}
    , diffuseMap_{diffuseMap}
    , lightDir_{lightDir}
    , lightColor_{lightColor}
    , ambientStrength_{ambientStrength}
    , specularStrength_{specularStrength}
    , shininess_{shininess}
{
    lightDir_.Normalize();
}

VertexOut PhongTextureShader::vertex(const VertexIn& in) {
    VertexOut out{};

    // Позиция и нормаль в мире (пока считаем, что модель в world space)
    out.worldPos = in.position;

    out.normal = in.normal;
    out.normal.Normalize();

    out.uv = in.uv;

    // clipPos = MVP * (x, y, z, 1)
    const mat4 mvp = camera_.getViewProjection();
    const vec4 worldPos4(out.worldPos.x, out.worldPos.y, out.worldPos.z, 1.0f);
    out.clipPos = vec4::Transform(worldPos4, mvp);

    return out;
}

TGAColor PhongTextureShader::sampleDiffuse(const vec2& uv) const {
    if (diffuseMap_.width() == 0 || diffuseMap_.height() == 0) {
        // fallback — белый
        TGAColor white;
        white[0] = 255; // B
        white[1] = 255; // G
        white[2] = 255; // R
        white[3] = 255; // A
        return white;
    }

    // --- 1. wrap uv в диапазон [0;1) ---
    auto wrap01 = [](float t) {
        t = std::fmod(t, 1.0f);
        if (t < 0.0f) t += 1.0f;
        return t;
    };

    float u = wrap01(uv.x);
    float v = wrap01(uv.y);

    const int texWidth  = diffuseMap_.width();
    const int texHeight = diffuseMap_.height();

    // учитываем flip_vertically() в Model::loadDiffuse
    // поэтому здесь НЕ переворачиваем ещё раз по Y
    const int x = static_cast<int>(u * static_cast<float>(texWidth  - 1));
    const int y = static_cast<int>(v * static_cast<float>(texHeight - 1));

    return diffuseMap_.get(x, y);
}

FragmentOut PhongTextureShader::fragment(const FragmentIn& in) const {
    FragmentOut out{};
    out.discard = false;

    const vec3& bar = in.bar;

    // 1. Интерполяция worldPos, normal, uv
    vec3 worldPos =
        in.vertices[0].worldPos * bar.x +
        in.vertices[1].worldPos * bar.y +
        in.vertices[2].worldPos * bar.z;

    vec3 normal =
        in.vertices[0].normal * bar.x +
        in.vertices[1].normal * bar.y +
        in.vertices[2].normal * bar.z;

    normal.Normalize();

    vec2 uv =
        in.vertices[0].uv * bar.x +
        in.vertices[1].uv * bar.y +
        in.vertices[2].uv * bar.z;

    // 2. Сэмплим диффузную текстуру
    TGAColor texColor = sampleDiffuse(uv);

    // В TinyTGA порядок обычно BGRA, будем считать так:
    // texColor[2] - R, [1] - G, [0] - B
    constexpr float inv255 = 1.0f / 255.0f;

    vec3 baseColor(
        texColor[2] * inv255,
        texColor[1] * inv255,
        texColor[0] * inv255
    );

    // 3. Phong lighting

    // Ambient
    vec3 ambient = ambientStrength_ * baseColor;

    // Diffuse
    const float ndotl = std::max(normal.Dot(lightDir_), 0.0f);
    vec3 diffuse = ndotl * baseColor * lightColor_;

    // Specular
    const vec3 eye = camera_.getEye(); // добавим getEye в Camera

    vec3 viewDir = eye - worldPos;
    viewDir.Normalize();

    // R = 2 * dot(N, L) * N - L, где L = lightDir_
    vec3 reflectDir = (2.0f * normal.Dot(lightDir_)) * normal - lightDir_;
    reflectDir.Normalize();

    const float specAngle = std::max(viewDir.Dot(reflectDir), 0.0f);
    const float specFactor = std::pow(specAngle, shininess_);

    vec3 specular = specularStrength_ * specFactor * lightColor_;

    // 4. Суммарный цвет
    vec3 finalColor = ambient + diffuse + specular;

    finalColor.x = std::clamp(finalColor.x, 0.0f, 1.0f);
    finalColor.y = std::clamp(finalColor.y, 0.0f, 1.0f);
    finalColor.z = std::clamp(finalColor.z, 0.0f, 1.0f);

    TGAColor outColor;
    // Снова в BGRA
    outColor[2] = static_cast<unsigned char>(finalColor.x * 255.0f); // R
    outColor[1] = static_cast<unsigned char>(finalColor.y * 255.0f); // G
    outColor[0] = static_cast<unsigned char>(finalColor.z * 255.0f); // B
    outColor[3] = 255;                                               // A

    out.color = outColor;
    return out;
}