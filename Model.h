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


class Model {
public:
    using Vec3  = DirectX::SimpleMath::Vector3;
    using Face  = std::array<int, 3>;

    bool loadOBJ(const std::string_view& filepath);

    Model() = default;
    explicit Model(const std::string_view& objPath) { loadOBJ(objPath); }

    void debugInfo() const;

    [[nodiscard]] const std::vector<Vec3>&  getVertices() const { return vertices_; }
    [[nodiscard]] const std::vector<Face>&  getFaces()    const { return faces_;    }

private:
    std::vector<Vec3> vertices_;
    std::vector<Face> faces_;

    // Разбирает токен индекса из "f"-строки (форматы: "i", "i/j", "i//k", "i/j/k").
    // Нам нужен только индекс вершины i. Возвращает 0-based индекс.
    static std::optional<int> parseVertexIndexToken(std::string_view token);
};



#endif //MODEL_H
