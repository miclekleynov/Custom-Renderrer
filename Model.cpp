//
// Created by mishka on 10.11.2025.
//

#include "Model.h"

#include <charconv>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

using Vec3 = DirectX::SimpleMath::Vector3;

std::optional<int> Model::parseVertexIndexToken(std::string_view token) {
    // Берём подстроку до первого '/'
    const std::size_t slashPos = token.find('/');
    const std::string_view head = (slashPos == std::string_view::npos) ? token : token.substr(0, slashPos);

    // Пусто — невалидно
    if (head.empty()) return std::nullopt;

    // OBJ индексы 1-based (могут быть и отрицательные — пока не поддерживаем)
    int idx = 0;
    auto* first = head.data();
    auto* last  = head.data() + head.size();
    if (std::from_chars(first, last, idx).ec != std::errc{}) return std::nullopt;
    if (idx <= 0) return std::nullopt; // TODO: поддержать отрицательные индексы (-1 — последняя вершина)
    return idx - 1; // переводим в 0-based
}

bool Model::loadOBJ(const std::string_view& filepath) {
    std::ifstream in(std::string{filepath}, std::ios::in);
    if (!in) return false;

    vertices_.clear();
    faces_.clear();

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        // Пропускаем комментарии
        if (line[0] == '#') continue;

        // Быстрая проверка директивы
        if (line.rfind("v ", 0) == 0) {
            std::istringstream iss(line.substr(2));
            float x = 0.f, y = 0.f, z = 0.f;
            if (!(iss >> x >> y >> z)) continue;
            vertices_.emplace_back(x, y, z);

        } else if (line.rfind("f ", 0) == 0) {
            // Ожидаем треугольники; если N>3 — можно разбивать в fan, но TinyRenderer использует треугольники
            std::istringstream iss(line.substr(2));
            std::string t0, t1, t2;
            if (!(iss >> t0 >> t1 >> t2)) continue;

            std::optional<int> i0 = parseVertexIndexToken(t0);
            std::optional<int> i1 = parseVertexIndexToken(t1);
            std::optional<int> i2 = parseVertexIndexToken(t2);
            if (!i0 || !i1 || !i2) continue;

            faces_.push_back(Face{ *i0, *i1, *i2 });
        }
    }

    return !vertices_.empty() && !faces_.empty();
}

void Model::debugInfo() const {
    using std::cout;
    using std::endl;

    cout << "=== Model Debug Info ===" << endl;
    cout << "Vertices: " << vertices_.size() << endl;
    cout << "Faces:    " << faces_.size() << endl;
    cout << endl;
}
