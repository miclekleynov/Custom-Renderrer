//
// Created by ROG on 06.12.2025.
//

#ifndef TINYRENDERER_REDTINTHEADSHADER_H
#define TINYRENDERER_REDTINTHEADSHADER_H


#include "../realization/IShader.h"
#include "../realization/Camera.h"
#include "../realization/MathTypes.h"
#include "../tga/tgaimage.h"
#include <algorithm> // std::clamp

class RedTintHeadShader final : public IShader {
public:
    RedTintHeadShader(const Camera& camera,
                      const TGAImage& headDiffuse,
                      const TGAImage& background,
                      float alpha)
        : camera_{camera}
    , headDiffuse_{headDiffuse}
    , background_{background}
    , alpha_{alpha}
    {}

    VertexOut vertex(const VertexIn& in) override;
    [[nodiscard]] FragmentOut fragment(const FragmentIn& in) const override;

private:
    const Camera&  camera_;
    const TGAImage& headDiffuse_;
    const TGAImage& background_;
    float          alpha_{1.0f}; // 0..1, базовая прозрачность
};



#endif //TINYRENDERER_REDTINTHEADSHADER_H