//
// Created by mishka on 22.11.2025.
//

#ifndef ISHADER_H
#define ISHADER_H

#include "ShaderTypes.h"

class IShader {
public:
    virtual ~IShader() = default;

    virtual VertexOut vertex(const VertexIn& in) = 0;
    [[nodiscard]] virtual FragmentOut fragment(const FragmentIn& in) const = 0;
};

#endif //ISHADER_H
