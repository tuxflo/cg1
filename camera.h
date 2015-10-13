#ifndef CAMERA_H
#define CAMERA_H
#include "glm/glm/glm.hpp"
#include <GL/freeglut.h>
#include "glm/glm/gtc/matrix_transform.hpp"
#include <iostream>

class Camera
{
public:
    Camera();
    glm::mat4 getProjectionMatrix();
    glm::mat4 getViewMatrix();
    void calculatePositions();
    void up();
    void down();
    void left();
    void right();
    void setPos(int x, int y);
    float setDeltaTime(float delta);
private:
    glm::mat4 _ProjectionMatrix;
    glm::mat4 _ViewMatrix;
    glm::vec3 _direction;
    glm::vec3 _right;
    // Initial position : on +Z
    glm::vec3 _position;
    // Initial horizontal angle : toward -Z
    float _horizontalAngle;
    // Initial vertical angle : none
    float _verticalAngle;
    // Initial Field of View
    float _initialFoV;

    float _speed; // 3 units / second
    float _mouseSpeed;
    int _lastTime;
    int _xpos;
    int _ypos;
    float _deltaTime;
};

#endif // CAMERA_H
