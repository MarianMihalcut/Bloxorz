#define GLM_ENABLE_EXPERIMENTAL

#include <glew.h>
#include <GL/freeglut.h>
#include "glew.h"
#include "Game/Game.h"

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
 *  TODO: de lucru la partea grafica
    La tile-uri se pot pune texturi(la tile-uri si cub)
    De important un obiect obj de tip cub(vezi lab 9) la tile buton
    Background mai luminos(sau o lumina)
*/