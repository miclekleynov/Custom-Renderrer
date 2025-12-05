#include <cmath>
#include <iostream>

#include "tga/tgaimage.h"
#include <SimpleMath.h>

#include "shaders/FlatRandomColorShader.h"
#include "realization/Renderer.h"
#include "shaders/PhongTextureShader.h"



int main() {
    constexpr int width  = 1920;
    constexpr int height = 1920;

    constexpr vec3    eye{0.0f, 0, 2}; // camera position
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

    Model head;
    if (!head.loadOBJ("obj/african_head/african_head.obj")) {
        std::cerr << "Error with loading model: african_head.obj" << std::endl;
        return 1;
    }
    if (!head.loadDiffuse("obj/african_head/african_head_diffuse.tga")) {
        std::cerr << "Error with loading diffuse texture: african_head_diffuse.tga" << std::endl;
        return 1;
    }
    std::cout << "Head model successfully loaded.\n";

    Model eyeInner;
    if (!eyeInner.loadOBJ("obj/african_head/african_head_eye_inner.obj")) {
        std::cerr << "Error with loading model: african_head_eye_inner.obj" << std::endl;
        return 1;
    }
    if (!eyeInner.loadDiffuse("obj/african_head/african_head_eye_inner_diffuse.tga")) {
        std::cerr << "Error with loading diffuse texture: african_head_eye_inner_diffuse.tga" << std::endl;
        return 1;
    }
    std::cout << "Inner eye model successfully loaded.\n";


    Model eyeOuter;
    if (!eyeOuter.loadOBJ("obj/african_head/african_head_eye_outer.obj")) {
        std::cerr << "Error with loading model: african_head_eye_outer.obj" << std::endl;
        return 1;
    }
    if (!eyeOuter.loadDiffuse("obj/african_head/african_head_eye_outer_diffuse.tga")) {
        std::cerr << "Error with loading diffuse texture: african_head_eye_outer_diffuse.tga" << std::endl;
        return 1;
    }
    std::cout << "Outer eye model successfully loaded.\n";

    // Directional light
    const vec3 lightDir   { 1.0f, 1.0f, 1.0f };
    const vec3 lightColor { 1.0f, 1.0f, 1.0f };

    PhongTextureShader headShader(
    cam,
    head.getDiffuse(),
    lightDir,
    lightColor,
    0.15f,
    0.08f,
    12.0f
    );

    PhongTextureShader eyeInnerShader(
    cam,
    eyeInner.getDiffuse(),
    lightDir,
    lightColor,
    0.15f,
    0.2f,     // глаза сильнее бликуют
    32.0f
    );

    PhongTextureShader eyeOuterShader(
    cam,
    eyeOuter.getDiffuse(),
    lightDir,
    lightColor,
    0.1f,
    0.4f,      // сильный highlight
    64.0f
    );

    renderer.setShader(headShader);
    renderer.drawModel(head);

    renderer.setShader(eyeInnerShader);
    renderer.drawModel(eyeInner);
    
    renderer.setShader(eyeOuterShader);
    renderer.drawModel(eyeOuter);

    renderer.GetFramebuffer().write_tga_file("framebuffer.tga");
    return 0;
}

