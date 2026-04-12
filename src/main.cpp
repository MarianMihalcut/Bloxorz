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
#include "Objects/Cuboid/Cuboid.h"

#define PI glm::pi<float>()


using namespace ObjectModel;

Cuboid* cuboid;
glm::mat4 projectionMatrix, viewMatrix;
glm::vec3 lightPos(5.0f, 10.0f, 5.0f);
glm::vec3 viewPos(5.0f, 5.0f, 10.0f);
float rotAngle = 0.0f;
float rotAngleInc = PI / 64.0f; //creste cu PI/64

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cuboid->setProjectionMatrix(projectionMatrix);
    cuboid->setViewMatrix(viewMatrix);
    cuboid->setLightPos(lightPos);
    cuboid->setViewPos(viewPos);
    cuboid->setRotation(rotAngle);

    cuboid->display();

    glutSwapBuffers();
    glFlush();
}

void init() {
    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glewInit();

    //creare cuboid Bloxorz
    cuboid = new Cuboid(
        glm::vec3(0.0f, 0.25f, 0.0f), //position
        glm::vec3(1.0f, 0.5f, 2.0f), //scale
        glm::vec3(0.2f, 0.6f, 1.0f) //color
        );
    cuboid->init();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    projectionMatrix = glm::perspective(PI / 6.0f, (float)w / h, 0.1f, 100.0f);
    viewMatrix = glm::lookAt(viewPos, glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'a':
            rotAngle += rotAngleInc;
            break;
        case 'd':
            rotAngle -= rotAngleInc;
            break;
        case 'w':
            cuboid->move(glm::vec3(0.0f, 0.0f, -0.1f));
            break;
        case 's':
            cuboid->move(glm::vec3(0.0f, 0.0f, 0.1f));
            break;
        case 27: //ESC
            delete cuboid;
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            cuboid->rotateY(rotAngleInc);
            break;
        case GLUT_KEY_DOWN:
            cuboid->rotateY(-rotAngleInc);
            break;
        case GLUT_KEY_LEFT:
            cuboid->move(glm::vec3(-0.1f, 0.0f, 0.0f));
            break;
        case GLUT_KEY_RIGHT:
            cuboid->move(glm::vec3(0.1f, 0.0f, 0.0f));
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {

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
    glutMainLoop();

    delete cuboid;
    return 0;
}