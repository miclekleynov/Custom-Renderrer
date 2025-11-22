//
// Created by mishka on 22.11.2025.
//

#ifndef FLATRANDOMCOLORSHADER_H
#define FLATRANDOMCOLORSHADER_H



#include "IShader.h"
#include "Camera.h"
#include "MathTypes.h"

// Шейдер, который даёт каждому треугольнику свой случайный цвет
class FlatRandomColorShader final : public IShader {
public:
    explicit FlatRandomColorShader(const Camera& camera);

    VertexOut vertex(const VertexIn& in) override;
    [[nodiscard]] FragmentOut fragment(const FragmentIn& in) const override;

private:
    mat4 mvp_;
};



#endif //FLATRANDOMCOLORSHADER_H
