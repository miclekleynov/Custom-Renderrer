//
// Created by mishka on 10.11.2025.
//

#ifndef MODEL_H
#define MODEL_H
#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <SimpleMath.h>
#include "MathTypes.h"
#include "ShaderTypes.h"


class Model {
public:
    Model() = default;
    explicit Model(const std::string_view& objPath) { loadOBJ(objPath); }

    bool loadOBJ(const std::string_view& filepath);
    void debugInfo() const;

    [[nodiscard]] const std::vector<Vertex>&  getVertices() const { return vertices_; }
    [[nodiscard]] const std::vector<face>&  getFaces()    const { return faces_;    }

private:
    std::vector<Vertex> vertices_;
    std::vector<face> faces_;
    std::vector<vec3> normals_;
    std::vector<vec2> uvs_;
};



#endif //MODEL_H
