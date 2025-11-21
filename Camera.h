//
// Created by mishka on 21.11.2025.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <SimpleMath.h>

using vec3 = DirectX::SimpleMath::Vector3;
using mat4 = DirectX::SimpleMath::Matrix;

class Camera {
private:
    mat4 modelView_{};
    mat4 viewport_{};
    mat4 perspective_{};

public:
    using vec3 = DirectX::SimpleMath::Vector3;
    using mat4 = DirectX::SimpleMath::Matrix;

    void setModelView(const vec3& eye, const vec3& center, const vec3& up);
    void setPerspective(float f);
    void setViewport(int x, int y, int w, int h);

    [[nodiscard]] const mat4& GetModelView()        const { return modelView_; }
    [[nodiscard]] const mat4& GetViewport()  const { return viewport_; }
    [[nodiscard]] const mat4& GetPerspective()    const { return perspective_; }

    [[nodiscard]] mat4 makeMVP() const {
        return perspective_ * modelView_;
    }



};



#endif //CAMERA_H
