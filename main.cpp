#include <cmath>
#include <iostream>

#include "tga/tgaimage.h"
#include <SimpleMath.h>

#include "shaders/FlatRandomColorShader.h"
#include "realization/Renderer.h"

using namespace DirectX::SimpleMath;



int main() {
    constexpr int width  = 1920;
    constexpr int height = 1920;

    constexpr vec3    eye{-1, 0, 2}; // camera position
    const vec3 center{  0.0f, 0.0f, 0.0f };   // смотрим в центр мира
    const vec3 up    {  0.0f, 1.0f, 0.0f };   // "верх"

    Camera cam;
    cam.setModelView(eye, center, up);
    cam.setPerspective((eye - center).Length());
    cam.setViewport(width/16, height/16, width*7/8, height*7/8);

    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    Renderer renderer(framebuffer, zbuffer);
    renderer.setCamera(cam);

    FlatRandomColorShader shader(cam);
    renderer.setShader(shader);

    Model demon;
    if (!demon.loadOBJ("obj/diablo3_pose/diablo3_pose.obj")) {
        std::cerr << "Error with loading model: diablo3_pose.obj" << std::endl;
    } else {
        std::cout << "Model diablo3_pose.obj successfully downloaded" << std::endl;
    }

    demon.debugInfo();
    renderer.drawModel(demon);

    renderer.GetFramebuffer().write_tga_file("framebuffer.tga");
    renderer.GetZbuffer().write_tga_file("zbuffer.tga");
    return 0;
}

