#ifndef H_CYLINDER
#define H_CYLINDER

#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a Cylinder located at 'center' with the specified radius and height.
 */
class Cylinder : public SceneObject
{
private:
    glm::vec3 center; // Center position of the cylinder
    float radius;     // Radius of the cylinder
    float height;     // Height of the cylinder

public:
    Cylinder() : center(glm::vec3(0)), radius(1.0), height(1.0) {}

    Cylinder(glm::vec3 c, float r, float h) : center(c), radius(r), height(h) {}

    float intersect(glm::vec3 p0, glm::vec3 dir);

    glm::vec3 normal(glm::vec3 p);
};

#endif // H_CYLINDER
