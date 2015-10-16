#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "glm/glm/glm.hpp"


class Triangle
{
public:
    Triangle();
	glm::vec3 getNormal();
private:
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
};

#endif // TRIANGLE_H
