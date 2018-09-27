#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera
{
private:
    glm::vec4 forward, right, up, pos;
    float vertical, near, far, aspect;
public:
    Camera();

    //Matrices
    glm::mat4 view();
    glm::mat4 proj();

    //Translations
    void translateForward(float t);
    void translateRight(float t);
    void translateUp(float t);

    //Rotations
    void rotateZ(float r);
    void rotateX(float r);
    void rotateY(float r);

    glm::vec4 dir();
};

#endif // CAMERA_H
