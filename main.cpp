#include <cmath>
#include <iostream>

#include "tgaimage.h"
#include <SimpleMath.h>
#include "DrawKit.h"

using namespace DirectX::SimpleMath;



int main() {
    constexpr int width  = 800;
    constexpr int height = 800;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;
    DrawKit drk(framebuffer);
    //drk.drawLine(ax, ay, bx, by, white);
    Model model;
    if (!model.loadOBJ("obj/diablo3_pose/diablo3_pose.obj")) {
        std::cerr << "❌ Не удалось загрузить модель: obj/floor.obj" << std::endl;
    } else {
        std::cout << "✅ Модель успешно загружена" << std::endl;
    }
    drk.drawModel(model, red);

    bool res = drk.GetFramebuffer().write_tga_file("framebuffer.tga");
    return 0;
}

