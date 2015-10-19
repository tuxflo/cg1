#include "camera.h"

Camera::Camera(bool stat)
    : _isStatic(stat)
{
    _lastTime = 0;
    _horizontalAngle = 3.14;
    _verticalAngle = 0.0;
    _initialFoV = 45.0f;
     _position = glm::vec3( 0, 0, 5 );
     _right = glm::vec3(0.0);
     _direction = glm::vec3(0.0);
     _mouseSpeed = 0.004f;
     _speed = 5.0f;
    _ViewMatrix =  glm::lookAt(
        glm::vec3(0, 0, 5), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0,1,0));
}

glm::mat4 Camera::getProjectionMatrix()
{
    if(_isStatic)
        return glm::perspective(_initialFoV, 4.0f / 3.0f, 0.1f, 100.0f);
    return _ProjectionMatrix;
}

glm::mat4 Camera::getViewMatrix()
{
    if(_isStatic)
    {
        glm::mat4 View = glm::lookAt(
                   glm::vec3(5,5,5),
                   glm::vec3(0,0,0),
                   glm::vec3(0,1,0)
                            );
        return View;
    }
    return _ViewMatrix;
}

void Camera::calculatePositions()
{
    _horizontalAngle += _mouseSpeed * float(1024/2 - _xpos );
    _verticalAngle   += _mouseSpeed * float( 768/2 - _ypos );

    _direction = glm::vec3(
                cos(_verticalAngle) * sin(_horizontalAngle),
                sin(_verticalAngle),
                cos(_verticalAngle) * cos(_horizontalAngle)
                );
    _right = glm::vec3(
                sin(_horizontalAngle -3.14f/2.0f),
                0,
                cos(_horizontalAngle -3.14f/2.0f)
                );
    glm::vec3 up = glm::cross( _right, _direction);

    float FoV = _initialFoV;

    _ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);

    _ViewMatrix = glm::lookAt(
                _position,
                _position + _direction,
                up);
}

void Camera::up()
{
   _position += _direction * _deltaTime * _speed;
   glutPostRedisplay();
}

void Camera::down()
{
   _position -= _direction * _deltaTime * _speed;
   glutPostRedisplay();
}

void Camera::left()
{
   _position -= _right * _deltaTime * _speed;
   glutPostRedisplay();
}

void Camera::right()
{
   _position += _right * _deltaTime * _speed;
   glutPostRedisplay();
}

void Camera::setPos(int x, int y)
{
   _xpos = x;
   _ypos = y;
   calculatePositions();
   if ( x != 1024 / 2 || y != 768/2 )
    glutWarpPointer(1024 / 2, 768 / 2);
}

float Camera::setDeltaTime(float delta)
{
    _deltaTime = delta;
}

glm::vec3 Camera::getPosition()
{
    return _position;
}

