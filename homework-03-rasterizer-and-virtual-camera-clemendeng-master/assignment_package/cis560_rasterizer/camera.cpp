#include "camera.h"

Camera::Camera() : forward(glm::vec4(0,0,-1,0)), right(glm::vec4(1,0,0,0)), up(glm::vec4(0,1,0,0)),
    pos(glm::vec4(0,0,10,1)), vertical(45), near(0.01), far(100), aspect(1)
{}

glm::mat4 Camera::view() {

}

glm::mat4 Camera::proj() {

}

void Camera::translateForward(float t) {

}
