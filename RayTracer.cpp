/*==================================================================================
* COSC 363  Computer Graphics (2023)
* Department of Computer Science and Software Engineering, University of Canterbury.
*
* A basic ray tracer
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/freeglut.h>
#include "Plane.h"
#include "TextureBMP.h"
#include "Cylinder.h"

using namespace std;

const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 50;
const float XMIN = -10.0;
const float XMAX = 10.0;
const float YMIN = -10.0;
const float YMAX = 10.0;

vector<SceneObject*> sceneObjects;

Sphere *sphere2 = new Sphere(glm::vec3(-10, -7.0, -100.0), 8.0);
//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
    glm::vec3 backgroundCol(0);                     //Background colour = (0,0,0)
    glm::vec3 lightPos(0, 38, -40);                 //Light's position
    glm::vec3 color(0);
    SceneObject* obj;

    ray.closestPt(sceneObjects);                    //Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;       //no intersection
    obj = sceneObjects[ray.index];                  //object on which the closest point of intersection is found

    if (ray.index == 3)
    {
    // Checkerboard pattern
        int gridSize = 10;
        int ix = static_cast<int>(ray.hit.x) / gridSize;
        int iz = static_cast<int>(ray.hit.z) / gridSize;
        int k = (ix + iz) % 2; // 2 colors
        if (k == 0) color = glm::vec3(0, 1, 0);
        else color = glm::vec3(1, 1, 0.5);
        obj->setColor(color);

    }

    color = obj->lighting(lightPos, -ray.dir, ray.hit);   //Object's colour

    // Add shadow calculations
    glm::vec3 lightVec = lightPos - ray.hit;
    Ray shadowRay(ray.hit, lightVec);
    shadowRay.closestPt(sceneObjects);

    if (shadowRay.index > -1 && shadowRay.dist < glm::length(lightVec))
    {
        if (sceneObjects[shadowRay.index] == sphere2)
        {
            color = 0.6f * obj->getColor();
        }
        else
        {
            color = 0.2f * obj->getColor();
        }
    }

    if(obj->isReflective() && step < MAX_STEPS)
    {
        float rho = obj->getReflectionCoeff();
        glm::vec3 normalVec = obj->normal(ray.hit);
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
        Ray reflectedRay(ray.hit + 0.01f * reflectedDir, reflectedDir);
        glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
        color = color + (rho * reflectedColor);
    }

    if (obj->isTransparent() && step < MAX_STEPS)
    {
        glm::vec3 offsetDir = ray.dir;

        glm::vec3 offset = 0.01f * offsetDir;
        Ray transparentRay(ray.hit + offset, offsetDir);
        glm::vec3 transparentColor = trace(transparentRay, step + 1);

        color = 0.2f * obj->getColor();
        color = color + transparentColor;
    }



    return color;
}



//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
    float xp, yp;  //grid point
    float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
    float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
    glm::vec3 eye(0., 0., 0.);

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);  //Each cell is a tiny quad.

    for (int i = 0; i < NUMDIV; i++)    //Scan every cell of the image plane
    {
        xp = XMIN + i * cellX;
        for (int j = 0; j < NUMDIV; j++)
        {
            yp = YMIN + j * cellY;

            glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);  //direction of the primary ray

            Ray ray = Ray(eye, dir);

            glm::vec3 col = trace(ray, 1); //Trace the primary ray and get the colour value
            glColor3f(col.r, col.g, col.b);
            glVertex2f(xp, yp);             //Draw each cell with its color value
            glVertex2f(xp + cellX, yp);
            glVertex2f(xp + cellX, yp + cellY);
            glVertex2f(xp, yp + cellY);
        }
    }

    glEnd();
    glFlush();
}
//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

    Sphere *sphere1 = new Sphere(glm::vec3(20.0, 0.0, -120.0), 8.0);
    sphere1->setColor(glm::vec3(0, 0, 1));   //Set colour to blue
    sphere1->setShininess(50);
    sphere1->setReflectivity(true, 0.8);
    sceneObjects.push_back(sphere1);         //Add sphere to scene objects

    Sphere *sphere3 = new Sphere(glm::vec3(-20, -5, -150.0), 10.0);
    sphere3->setColor(glm::vec3(1, 0, 0));   //Set colour to blue
    sphere3->setShininess(5);
    sphere3->setReflectivity(true, 0.2);
    sceneObjects.push_back(sphere3);         //Add sphere to scene objects

    Plane *leftWall = new Plane(glm::vec3(-40., -15, 0),   // Point A
                           glm::vec3(-40., -15, -200),  // Point B
                           glm::vec3(-40., 40, -200),   // Point C
                           glm::vec3(-40., 40, 0));   // Point D
    leftWall->setColor(glm::vec3(0.5, 0.5, 0.5));  // Set color to yellow
    sceneObjects.push_back(leftWall);

    Plane *floorPlane = new Plane (glm::vec3(-40., -15, 0), //point A
                              glm::vec3(40., -15, 0), //point B
                              glm::vec3(40., -15, -200),    //point C
                              glm::vec3(-40, -15, -200));   //point D
    floorPlane->setColor(glm::vec3(0.1, 0.1, 0.1));
    sceneObjects.push_back(floorPlane);

    sphere2->setColor(glm::vec3(0.5, 0.5, 0.8));
    sphere2->setTransparency(true, 0.5);
    sceneObjects.push_back(sphere2);         //Add sphere to scene objects

    Plane *mirrorPlane = new Plane (glm::vec3(-40., -15, -200), //point A
                                glm::vec3(40., -15, -200),  //point B
                                glm::vec3(40., 40, -200),    //point C
                                glm::vec3(-40, 40, -200));   //point D
    mirrorPlane->setColor(glm::vec3(0, 0, 0));
    mirrorPlane->setShininess(500);
    mirrorPlane->setReflectivity(true, 0.9); // Make the plane reflective
    sceneObjects.push_back(mirrorPlane);

    Plane *mirrorPlaneBehind = new Plane (glm::vec3(-40., -15, 0), //point A
                            glm::vec3(40., -15, 0),  //point B
                            glm::vec3(40., 40, 0),    //point C
                            glm::vec3(-40, 40, 0));   //point D
    mirrorPlaneBehind->setColor(glm::vec3(0, 0, 0));
    mirrorPlaneBehind->setShininess(500);
    mirrorPlaneBehind->setReflectivity(true, 0.9); // Make the plane reflective
    sceneObjects.push_back(mirrorPlaneBehind);

    Plane *ceiling = new Plane(glm::vec3(-40., 40, -200),  // Point D
                               glm::vec3(40., 40, -200),   // Point C
                               glm::vec3(40., 40, 0),      // Point B
                               glm::vec3(-40, 40, 0));     // Point A
    ceiling->setColor(glm::vec3(1, 0, 0));
    sceneObjects.push_back(ceiling);

    Cylinder* cylinder = new Cylinder(glm::vec3(20, -15, -120), 4, 8);
    cylinder->setColor(glm::vec3(0.5, 0.5, 0.5)); // Set color to gray
    sceneObjects.push_back(cylinder);             // Add cylinder to scene objects

    Plane *rightWall = new Plane (glm::vec3(40., 40, 0),
                                 glm::vec3(40., 40, -200),
                                 glm::vec3(40., -15, -200),
                                 glm::vec3(40.,-15, 0));
    rightWall->setColor(glm::vec3(1, 1, 0));
    sceneObjects.push_back(rightWall);
}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
