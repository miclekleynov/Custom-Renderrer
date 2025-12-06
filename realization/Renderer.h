//
// Created by mishka on 10.11.2025.
//

#ifndef DRAWKIT_H
#define DRAWKIT_H

#include "../tga/tgaimage.h"
#include <SimpleMath.h>
#include "Camera.h"
#include <vector>
#include <limits>
#include <algorithm>
#include <utility>
#include "MathTypes.h"
#include "IShader.h"
#include "Model.h"



class Renderer {
private:
    TGAImage framebuffer_;
    TGAImage zbuffer_;
    std::vector<float> depthBuffer_;

    const Camera* camera_ { nullptr };
    IShader* shader_  { nullptr };

    static double signed_triangle_area(double ax, double ay,
                                       double bx, double by,
                                       double cx, double cy);
    void rasterizeTriangle(const VertexOut (&verts)[3]);

public:
    explicit Renderer(TGAImage framebuffer, TGAImage zbuffer);

    void setCamera(const Camera& cam) { camera_ = &cam; }
    void setShader(IShader& shader)   { shader_ = &shader; }

    void clearDepth();

    void drawModel(const Model& model);

    [[nodiscard]] TGAImage& GetFramebuffer() {return framebuffer_;};
    [[nodiscard]] TGAImage& GetZbuffer() {return zbuffer_;};
};



#endif //DRAWKIT_H
