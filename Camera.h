//
// Created by mishka on 21.11.2025.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <SimpleMath.h>
#include "MathTypes.h"


class Camera {
private:
    mat4 modelView_{};
    mat4 viewport_{};
    mat4 perspective_{};

public:
    void setModelView(const vec3& eye, const vec3& center, const vec3& up);
    void setPerspective(float f);
    void setViewport(int x, int y, int w, int h);

    [[nodiscard]] const mat4& getModelView()        const { return modelView_; }
    [[nodiscard]] const mat4& getViewport()  const { return viewport_; }
    [[nodiscard]] const mat4& getPerspective()    const { return perspective_; }
    [[nodiscard]] mat4 getViewProjection() const {
        return perspective_ * modelView_;
    }



};



#endif //CAMERA_H
