#define GLM_ENABLE_EXPERIMENTAL

#include <glew.h>
#include <GL/freeglut.h>
#include "glew.h"
#include "Game/Game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(1000, 900);
    glutCreateWindow("Bloxorz SPG");

    Game::getInstance()->init(1000, 900);

    glutDisplayFunc (Game::displayCB);
    glutReshapeFunc (Game::reshapeCB);
    glutKeyboardFunc(Game::keyboardCB);
    glutSpecialFunc (Game::specialCB);
    glutIdleFunc    (Game::idleCB);

    glutMainLoop();
    return 0;
}

/*
 *  TODO: adaugare textura pe cuboid
 *  (o singura sursa de lumina peste tot) - lab12 sau Lighting din learn OpenGL
 *  adaugare umbra la cuboid - sa fie vizibila
 *  Procesare grafica: texturi, umbre, etc.
*/