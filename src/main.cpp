#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <chrono>

#include "glew.h"
#include "Objects/Cuboid/Cuboid.h"

#define PI glm::pi<float>()

using namespace ObjectModel;
using namespace std::chrono;

Cuboid* cuboid;
glm::mat4 projectionMatrix, viewMatrix;
glm::vec3 lightPos(5.0f, 10.0f, 5.0f);
glm::vec3 viewPos(5.0f, 5.0f, 10.0f);

// Pentru delta time
high_resolution_clock::time_point lastTime;
bool firstFrame = true;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculare delta time
    if (firstFrame) {
        lastTime = high_resolution_clock::now();
        firstFrame = false;
    }

    auto currentTime = high_resolution_clock::now();
    float deltaTime = duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;

    // Actualizare animație
    cuboid->update(deltaTime);

    cuboid->setProjectionMatrix(projectionMatrix);
    cuboid->setViewMatrix(viewMatrix);
    cuboid->setLightPos(lightPos);
    cuboid->setViewPos(viewPos);

    cuboid->display();

    glutSwapBuffers();
    glFlush();

    // Continuă să apeleze display pentru animație
    if (cuboid->isAnimating()) {
        glutPostRedisplay();
    }
}

void init()
{
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glewInit();

    cuboid = new Cuboid(glm::vec3(0.0f, 0.5f, 0.0f),
                        glm::vec3(0.5f, 1.0f, 0.5f),
                        glm::vec3(0.2f, 0.6f, 1.0f));
    cuboid->init();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    projectionMatrix = glm::perspective(PI / 6.0f, (float)w / h, 0.1f, 100.0f);
    viewMatrix = glm::lookAt(viewPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: // ESC
            delete cuboid;
            exit(0);
            break;
        case 'r': // Reset poziție
            cuboid->setPosition(glm::vec3(0.0f, 0.5f, 0.0f));
            cuboid->setScale(glm::vec3(0.5f, 1.0f, 0.5f));
            cuboid->setRotation(0.0f);
            break;
    }
    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:
            cuboid->moveLeft();
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT:
            cuboid->moveRight();
            glutPostRedisplay();
            break;
        case GLUT_KEY_UP:
            cuboid->moveUp();
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN:
            cuboid->moveDown();
            glutPostRedisplay();
            break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Bloxorz SPG");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutIdleFunc([]() { glutPostRedisplay(); });  // Pentru animație continuă

    glutMainLoop();

    delete cuboid;
    return 0;
}