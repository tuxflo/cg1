#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "glm/glm/glm.hpp"
#include <iostream>

class Triangle
{
public:
    Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    Triangle();
    glm::vec3 getNormal();
private:
    glm::vec3 _a;
    glm::vec3 _b;
    glm::vec3 _c;
};

#endif // TRIANGLE_H
