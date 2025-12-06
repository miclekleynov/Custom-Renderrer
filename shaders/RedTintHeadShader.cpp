//
// Created by ROG on 06.12.2025.
//

#include "RedTintHeadShader.h"

VertexOut RedTintHeadShader::vertex(const VertexIn& in) {
    VertexOut out{};

    // Позиция в мире = как есть (можно добавить масштаб/сдвиг, если нужно)
    out.worldPos = in.position;
    out.normal   = in.normal;
    out.uv       = in.uv;

    // Переводим в clip space через матрицу ViewProjection
    vec4 worldPos4(in.position.x, in.position.y, in.position.z, 1.0f);
    vec4 clipPos = vec4::Transform(worldPos4, camera_.getViewProjection());
    out.clipPos  = clipPos;

    return out;
}

FragmentOut RedTintHeadShader::fragment(const FragmentIn& in) const {
    FragmentOut out{};

    // 1. Барицентрические координаты
    const float b0 = in.bar.x;
    const float b1 = in.bar.y;
    const float b2 = in.bar.z;

    // 2. Интерполяция UV
    const vec2 uv =
        in.vertices[0].uv * b0 +
        in.vertices[1].uv * b1 +
        in.vertices[2].uv * b2;

    const int texW = headDiffuse_.width();
    const int texH = headDiffuse_.height();

    const int u = std::clamp(
        static_cast<int>(uv.x * static_cast<float>(texW - 1)),
        0,
        texW - 1
    );
    const int v = std::clamp(
        static_cast<int>(uv.y * static_cast<float>(texH - 1)),
        0,
        texH - 1
    );

    TGAColor headColor = headDiffuse_.get(u, v);

    // 3. Маска из яркости головы (чтобы стекло было только там, где есть текстура)
    const float luminance =
        (static_cast<float>(headColor[0]) +
         static_cast<float>(headColor[1]) +
         static_cast<float>(headColor[2])) / (3.0f * 255.0f);

    // Базовая прозрачность, заданная в конструкторе (overlayAlpha_)
    const float effectiveAlpha = alpha_ * luminance;

    if (effectiveAlpha <= 0.001f) {
        // Почти полностью прозрачный — можно отбросить
        out.discard = true;
        return out;
    }

    // 4. Берём фон (Диабло) по экранным координатам фрагмента
    const int x = static_cast<int>(in.screenPos.x);
    const int y = static_cast<int>(in.screenPos.y);

    const int fbW = background_.width();
    const int fbH = background_.height();

    if (x < 0 || x >= fbW || y < 0 || y >= fbH) {
        out.discard = true;
        return out;
    }

    TGAColor bg = background_.get(x, y);

    // 5. Альфа-смешивание (BGRA!)
    TGAColor result{};
    const float invA = 1.0f - effectiveAlpha;

    // ВАЖНО: порядок каналов TGAColor — B, G, R, A
    // B
    result[0] = static_cast<unsigned char>(
        invA * static_cast<float>(bg[0])
        // + effectiveAlpha * 0.f  // стекло чисто красное, синий не добавляем
    );

    // G
    result[1] = static_cast<unsigned char>(
        invA * static_cast<float>(bg[1])
        // + effectiveAlpha * 0.f  // зелёный тоже не добавляем
    );

    // R — здесь делаем красное стекло
    result[2] = static_cast<unsigned char>(
        invA * static_cast<float>(bg[2]) +
        effectiveAlpha * 255.0f          // добавляем красный
    );

    // A (если используется) — просто непрозрачный пиксель в итоговом буфере
    result[3] = 255;

    out.color = result;
    out.discard = false;
    return out;
}