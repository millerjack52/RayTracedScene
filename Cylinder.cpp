#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method. The input is a ray.
*/
float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir)
{
    float a = dir.x * dir.x + dir.z * dir.z;
    float b = 2 * (dir.x * (p0.x - center.x) + dir.z * (p0.z - center.z));
    float c = (p0.x - center.x) * (p0.x - center.x) + (p0.z - center.z) * (p0.z - center.z) - radius * radius;

    float delta = b * b - 4 * a * c;

    if (delta < 0.001)
        return -1.0; // No intersection

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);

    float y1 = p0.y + t1 * dir.y;
    float y2 = p0.y + t2 * dir.y;

    if (y1 > center.y && y1 < center.y + height)
        return t1; // Intersection with the side of the cylinder

    if (y2 > center.y && y2 < center.y + height)
        return t2; // Intersection with the side of the cylinder

    float t3 = (center.y - p0.y) / dir.y;
    float y3 = p0.y + t3 * dir.y;

    if (y3 > center.y && y3 < center.y + height && t3 > 0)
        return t3; // Intersection with the top or bottom cap of the cylinder

    return -1.0; // No intersection
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = glm::vec3(p.x - center.x, 0, p.z - center.z);
    n = glm::normalize(n);
    return n;
}

