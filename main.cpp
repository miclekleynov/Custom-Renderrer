#include <cmath>
#include <iostream>

#include "tgaimage.h"
#include <SimpleMath.h>
#include "Renderer.h"

using namespace DirectX::SimpleMath;



int main() {
    constexpr int width  = 800;
    constexpr int height = 800;

    constexpr vec3    eye{-1,0,2}; // camera position
    constexpr vec3 center{0,0,0};  // camera direction
    constexpr vec3     up{0,1,0};  // camera up vector

    Camera cam;
    cam.setModelView(eye, center, up);
    cam.setPerspective((eye - center).Length());
    cam.setViewport(width/16, height/16, width*7/8, height*7/8);


    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    Renderer renderer(framebuffer, zbuffer);

    Model model;
    if (!model.loadOBJ("obj/diablo3_pose/diablo3_pose.obj")) {
        std::cerr << "Error with loading model: diablo3_pose.obj" << std::endl;
    } else {
        std::cout << "✅ Модель успешно загружена" << std::endl;
    }
    renderer.drawRasterization(model, cam);

    renderer.GetFramebuffer().write_tga_file("framebuffer.tga");
    renderer.GetZbuffer().write_tga_file("zbuffer.tga");
    return 0;
}

