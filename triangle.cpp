#include "triangle.h"

Triangle::Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    : _a(a),
      _b(b),
      _c(c)
{

}

Triangle::Triangle()
    : _a(0.0),
      _b(0.0),
      _c(0.0)
{

}
glm::vec3 Triangle::getNormal()
{
    glm::vec3 tmp = glm::normalize(glm::cross(_c -_a, _b -_a));
    return tmp;
}

