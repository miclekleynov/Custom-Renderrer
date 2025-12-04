//
// Created by mishka on 10.11.2025.
//

#include "Model.h"

#include <charconv>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <optional>

// Локальная структура: индексы на позицию, uv и нормаль в исходных массивах
namespace {
    struct IndexTriplet {
        int v  = -1; // index in positions
        int vt = -1; // index in texcoords
        int vn = -1; // index in normals
    };

    // Парсит одно число из string_view (1-based индекс из OBJ) в 0-based
    std::optional<int> parseObjIndex(std::string_view sv) {
        if (sv.empty()) return std::nullopt;

        int idx = 0;
        auto* first = sv.data();
        auto* last  = sv.data() + sv.size();

        if (std::from_chars(first, last, idx).ec != std::errc{}) {
            return std::nullopt;
        }

        // Пока поддерживаем только положительные индексы
        if (idx <= 0) return std::nullopt;

        return idx - 1; // OBJ -> 0-based
    }

    // Парсит токен формата:
    // v
    // v/vt
    // v//vn
    // v/vt/vn
    std::optional<IndexTriplet> parseIndexTriplet(std::string_view token) {
        if (token.empty()) return std::nullopt;

        IndexTriplet result{};

        const std::size_t firstSlash  = token.find('/');
        const std::size_t secondSlash = (firstSlash == std::string_view::npos)
                                        ? std::string_view::npos
                                        : token.find('/', firstSlash + 1);

        if (firstSlash == std::string_view::npos) {
            // Формат: v
            auto vi = parseObjIndex(token);
            if (!vi) return std::nullopt;
            result.v = *vi;
            return result;
        }

        // Есть хотя бы один '/'
        std::string_view vPart  = token.substr(0, firstSlash);
        std::string_view vtPart;
        std::string_view vnPart;

        if (secondSlash == std::string_view::npos) {
            // Формат: v/vt
            vtPart = token.substr(firstSlash + 1);
        } else {
            // Формат: v/vt/vn или v//vn
            vtPart = token.substr(firstSlash + 1, secondSlash - firstSlash - 1);
            vnPart = token.substr(secondSlash + 1);
        }

        // v
        auto vi = parseObjIndex(vPart);
        if (!vi) return std::nullopt;
        result.v = *vi;

        // vt (может быть пустым)
        if (!vtPart.empty()) {
            if (auto vti = parseObjIndex(vtPart)) {
                result.vt = *vti;
            }
        }

        // vn (может быть пустым)
        if (!vnPart.empty()) {
            if (auto vni = parseObjIndex(vnPart)) {
                result.vn = *vni;
            }
        }

        return result;
    }
} // namespace

bool Model::loadOBJ(const std::string_view& filepath) {
    std::ifstream in(std::string{filepath}, std::ios::in);
    if (!in) {
        std::cerr << "Failed to open OBJ file: " << filepath << '\n';
        return false;
    }

    // Временные буферы "как в файле"
    std::vector<vec3> positions; // v
    std::vector<vec3> normals;   // vn
    std::vector<vec2> texcoords; // vt

    vertices_.clear();
    faces_.clear();
    normals_.clear();
    uvs_.clear();

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') continue; // комментарий

        // v x y z
        if (line.rfind("v ", 0) == 0) {
            std::istringstream iss(line.substr(2));
            float x = 0.f, y = 0.f, z = 0.f;
            if (!(iss >> x >> y >> z)) continue;
            positions.emplace_back(x, y, z);
        }
        // vt u v [w]
        else if (line.rfind("vt ", 0) == 0) {
            std::istringstream iss(line.substr(3));
            float u = 0.f, v = 0.f;
            if (!(iss >> u >> v)) continue;
            texcoords.emplace_back(u, v);
        }
        // vn x y z
        else if (line.rfind("vn ", 0) == 0) {
            std::istringstream iss(line.substr(3));
            float x = 0.f, y = 0.f, z = 0.f;
            if (!(iss >> x >> y >> z)) continue;
            normals.emplace_back(x, y, z);
        }
        // f v1/... v2/... v3/...
        else if (line.rfind("f ", 0) == 0) {
            std::istringstream iss(line.substr(2));
            std::vector<std::string> tokens;
            std::string t;
            while (iss >> t) {
                tokens.push_back(std::move(t));
            }
            if (tokens.size() < 3) continue;

            // Если больше трёх вершин — триангулируем "веером"
            // (v0, vk-1, vk)
            for (std::size_t i = 1; i + 1 < tokens.size(); ++i) {
                const std::string& t0 = tokens[0];
                const std::string& t1 = tokens[i];
                const std::string& t2 = tokens[i + 1];

                auto i0 = parseIndexTriplet(t0);
                auto i1 = parseIndexTriplet(t1);
                auto i2 = parseIndexTriplet(t2);
                if (!i0 || !i1 || !i2) continue;

                face f{}; // индексы в итоговый vertices_

                auto makeVertex = [&](const IndexTriplet& idx) -> int {
                    Vertex v{};
                    // position
                    if (idx.v >= 0 && idx.v < static_cast<int>(positions.size())) {
                        v.position = positions[static_cast<std::size_t>(idx.v)];
                    } else {
                        v.position = vec3(0.f, 0.f, 0.f);
                    }
                    // uv
                    if (idx.vt >= 0 && idx.vt < static_cast<int>(texcoords.size())) {
                        v.uv = texcoords[static_cast<std::size_t>(idx.vt)];
                    } else {
                        v.uv = vec2(0.f, 0.f);
                    }
                    // normal
                    if (idx.vn >= 0 && idx.vn < static_cast<int>(normals.size())) {
                        v.normal = normals[static_cast<std::size_t>(idx.vn)];
                    } else {
                        v.normal = vec3(0.f, 0.f, 0.f);
                    }

                    vertices_.push_back(v);
                    return static_cast<int>(vertices_.size()) - 1;
                };

                f[0] = makeVertex(*i0);
                f[1] = makeVertex(*i1);
                f[2] = makeVertex(*i2);

                faces_.push_back(f);
            }
        }
    }

    // Сохраняем исходные normals_/uvs_ "как в файле" (могут пригодиться отдельно)
    normals_ = std::move(normals);
    uvs_     = std::move(texcoords);

    const bool ok = !vertices_.empty() && !faces_.empty();
    if (!ok) {
        std::cerr << "OBJ load failed or file is empty: " << filepath << '\n';
    }
    return ok;
}

void Model::debugInfo() const {
    using std::cout;
    using std::endl;

    cout << "=== Model Debug Info ===" << endl;
    cout << "Vertices: " << vertices_.size() << endl;
    cout << "Faces:    " << faces_.size() << endl;
    cout << "Normals: " << normals_.size() << endl;
    cout << "Uvs:    " << uvs_.size() << endl;
    cout << endl;
}
