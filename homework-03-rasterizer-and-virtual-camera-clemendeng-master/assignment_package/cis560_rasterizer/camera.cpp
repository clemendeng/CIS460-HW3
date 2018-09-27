#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() : forward(glm::vec4(0,0,-1,0)), right(glm::vec4(1,0,0,0)), up(glm::vec4(0,1,0,0)),
    pos(glm::vec4(0,0,10,1)), vertical(45), near(0.01), far(100), aspect(1)
{}

glm::mat4 Camera::view() {
    glm::mat4 orientation(-1 * right[0], up[0], forward[0], 0,
                          right[1], -1 * up[1], forward[1], 0,
                          right[2], up[2], forward[2], 0,
                          0, 0, 0, 1);
    glm::mat4 translation(1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, 0,
                          -pos[0], -pos[1], -pos[2], 1);
    return orientation * translation;
}

glm::mat4 Camera::proj() {
    float P = far / (far - near);
    float Q = -far * near / (far - near);
    float S = 1 / tan(vertical/2);
    return glm::mat4(S / aspect, 0, 0, 0,
                     0, S, 0, 0,
                     0, 0, P, 1,
                     0, 0, Q, 0);
}

void Camera::translateForward(float t) {
    glm::vec4 addition = forward * t;
    pos += addition;
}

void Camera::translateRight(float t) {
    glm::vec4 addition = right * t;
    pos += addition;
}

void Camera::translateUp(float t) {
    glm::vec4 addition = up * t;
    pos += addition;
}

void Camera::rotateZ(float r) {
    glm::mat4 rot(1.0f);
    rot = glm::rotate(rot, r, glm::vec3(forward[0], forward[1], forward[2]));
    right = rot * right;
    up = rot * up;
}

void Camera::rotateX(float r) {
    glm::mat4 rot(1.0f);
    rot = glm::rotate(rot, r, glm::vec3(right[0], right[1], right[2]));
    forward = rot * forward;
    up = rot * up;
}

void Camera::rotateY(float r) {
    glm::mat4 rot(1.0f);
    rot = glm::rotate(rot, r, glm::vec3(up[0], up[1], up[2]));
    right = rot * right;
    forward = rot * forward;
}

glm::vec4 Camera::dir() {
    return forward;
}
