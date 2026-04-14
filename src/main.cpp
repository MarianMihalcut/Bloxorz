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
#include "Objects/Tiles/NormalTile.h"
#include "Objects/Tiles/ButtonTile.h"
#include "Objects/Tiles/BridgeTile.h"

#define PI glm::pi<float>()

using namespace ObjectModel;
using namespace std::chrono;

// ---------------------------------------------------------------------------
// Constante nivel
// ---------------------------------------------------------------------------
static constexpr float TILE_W = 1.0f;  // latimea unui tile in spatiu world
static constexpr float TILE_H = 0.2f;  // inaltimea unui tile
static constexpr float TILE_D = 1.0f;  // adancimea unui tile

// Inaltimea la care "sta" cuboid-ul deasupra tile-ului
static constexpr float CUBOID_BASE_Y = TILE_H + 0.5f; // jumatatea inaltimii cuboid-ului (scala 1.0)

// ---------------------------------------------------------------------------
// Starea globala a scenei
// ---------------------------------------------------------------------------
Cuboid* cuboid = nullptr;

// Toate tile-urile nivelului curent (ordinea nu conteaza pentru randare)
std::vector<std::unique_ptr<Tile>> tiles; //unique_ptr e un tip predefinit de smart pointer

// Pointer rapid catre bridge-uri (pentru callback-uri butoane)
// Indexat dupa (gridX, gridZ) - vom folosi o functie helper
BridgeTile* bridgeTile1 = nullptr;
BridgeTile* bridgeTile2 = nullptr;

glm::mat4 projectionMatrix, viewMatrix;
glm::vec3 lightPos(5.0f, 10.0f, 5.0f);
glm::vec3 viewPos(5.0f, 5.0f, 10.0f);
glm::vec3 viewTarget(3.0f, 0.0f, 3.0f); // centrul nivelului

// Pentru delta time
high_resolution_clock::time_point lastTime;
bool firstFrame = true;

// Pozitia cuboid-ului in grila (coloana, rand)
int cuboidGridX = 0;
int cuboidGridZ = 0;

// ---------------------------------------------------------------------------
// Helper: gaseste tile-ul la coordonatele (gx, gz)
// ---------------------------------------------------------------------------
Tile* getTileAt(int gx, int gz) {
    for (auto& t : tiles) {
        if (t->getGridX() == gx && t->getGridZ() == gz) {
            return t.get();
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Logica de interactiune: apelata dupa fiecare miscare a cuboid-ului
// ---------------------------------------------------------------------------
void onCuboidLanded() {
    // Verificam daca cuboid-ul se afla pe un tile activ
    Tile* current = getTileAt(cuboidGridX, cuboidGridZ);

    if (!current || !current->isActive()) {
        // Cuboid-ul a cazut in gol - reset pozitie
        printf("[Game] Cuboid a cazut! Reset pozitie.\n");
        cuboidGridX = 0;
        cuboidGridZ = 0;
        cuboid->setPosition(glm::vec3(
            cuboidGridX * TILE_W,
            CUBOID_BASE_Y,
            cuboidGridZ * TILE_D
        ));
        return;
    }

    // Daca e ButtonTile, il apasam
    if (current->getType() == TileType::BUTTON) {
        auto* btn = static_cast<ButtonTile*>(current);
        btn->press();
    }
}

// Eliberam butonul de pe care am plecat
void onCuboidLeft(int prevX, int prevZ) {
    Tile* prev = getTileAt(prevX, prevZ);
    if (prev && prev->getType() == TileType::BUTTON) {
        auto* btn = static_cast<ButtonTile*>(prev);
        if (btn->getMode() == ButtonTile::ButtonMode::HOLD) {
            btn->release();
        }
    }
}

//DOAR PENTRU SCOPURI DE TEST

// ---------------------------------------------------------------------------
// Construirea nivelului demo
//
//  Grila (X→, Z↓):
//
//    0   1   2   3   4   5
//  0 [N] [N] [N] [N] [ ] [ ]
//  1 [N] [ ] [ ] [B] [ ] [ ]    B = ButtonTile
//  2 [N] [ ] [ ] [N] [P] [P]    P = BridgeTile (pod)
//  3 [N] [N] [N] [N] [ ] [ ]
//
//  Blocul porneste la (0,0). Butonul de la (3,1) activeaza podul (4,2)+(5,2).
// ---------------------------------------------------------------------------
void buildLevel() {
    // -- Rand 0 --
    tiles.emplace_back(std::make_unique<NormalTile>(0, 0));
    tiles.emplace_back(std::make_unique<NormalTile>(1, 0));
    tiles.emplace_back(std::make_unique<NormalTile>(2, 0));
    tiles.emplace_back(std::make_unique<NormalTile>(3, 0));

    // -- Rand 1 --
    tiles.emplace_back(std::make_unique<NormalTile>(0, 1));
    // (1,1) si (2,1) - gol
    {
        // Buton la (3,1) - mod TOGGLE, activeaza bridge-urile
        auto btn = std::make_unique<ButtonTile>(
            3, 1,
            ButtonTile::ButtonMode::TOGGLE,
            [](bool pressed) {
                if (pressed) {
                    if (bridgeTile1) bridgeTile1->activate();
                    if (bridgeTile2) bridgeTile2->activate();
                    printf("[Game] Pod activat!\n");
                } else {
                    if (bridgeTile1) bridgeTile1->deactivate();
                    if (bridgeTile2) bridgeTile2->deactivate();
                    printf("[Game] Pod dezactivat!\n");
                }
            }
        );
        tiles.emplace_back(std::move(btn));
    }

    // -- Rand 2 --
    tiles.emplace_back(std::make_unique<NormalTile>(0, 2));
    // (1,2) si (2,2) - gol
    tiles.emplace_back(std::make_unique<NormalTile>(3, 2));
    {
        auto b1 = std::make_unique<BridgeTile>(4, 2, false, 0.4f);
        auto b2 = std::make_unique<BridgeTile>(5, 2, false, 0.4f);
        bridgeTile1 = b1.get();
        bridgeTile2 = b2.get();
        tiles.emplace_back(std::move(b1));
        tiles.emplace_back(std::move(b2));
    }

    // -- Rand 3 --
    tiles.emplace_back(std::make_unique<NormalTile>(0, 3));
    tiles.emplace_back(std::make_unique<NormalTile>(1, 3));
    tiles.emplace_back(std::make_unique<NormalTile>(2, 3));
    tiles.emplace_back(std::make_unique<NormalTile>(3, 3));

    // Initializam toate tile-urile (shaders + GPU buffers)
    for (auto& t : tiles) {
        t->init();
    }
}

// ---------------------------------------------------------------------------
// Callbacks GLUT
// ---------------------------------------------------------------------------
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

    // --- Update ---
    cuboid->update(deltaTime);

    for (auto& t : tiles) {
        t->update(deltaTime);
    }

    // --- Randare tile-uri ---
    glm::mat4 vpMatrix = projectionMatrix * viewMatrix;

    for (auto& t : tiles) {
        t->render(vpMatrix, lightPos, viewPos);
    }

    // --- Randare cuboid ---
    cuboid->setProjectionMatrix(projectionMatrix);
    cuboid->setViewMatrix(viewMatrix);
    cuboid->setLightPos(lightPos);
    cuboid->setViewPos(viewPos);
    cuboid->display();

    glutSwapBuffers();
    glFlush();

    // Continuă să apeleze display pentru animație
    //if (cuboid->isAnimating()) {
    //    glutPostRedisplay();
    //}
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

    // Cuboid-ul porneste deasupra tile-ului (0,0)
    cuboid = new Cuboid(
        glm::vec3(cuboidGridX * TILE_W, CUBOID_BASE_Y, cuboidGridZ * TILE_D),
        glm::vec3(0.5f, 1.0f, 0.5f),   // scala: 1x1 tile, inaltime 1
        glm::vec3(0.2f, 0.6f, 1.0f)    // culoare albastru
    );
    cuboid->init();

    buildLevel();
    printf("[Game] Level construit. Folositi sagetile pentru a muta blocul.\n");
    printf("[Game] Gasiti butonul pentru a activa podul!\n");
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    projectionMatrix = glm::perspective(PI / 4.0f, (float)w / h, 0.1f, 100.0f);
    viewMatrix = glm::lookAt(viewPos, viewTarget, glm::vec3(0.0f, 1.0f, 0.0f));
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
            cuboidGridX = 0;
            cuboidGridZ = 0;
            cuboid->setPosition(glm::vec3(cuboidGridX * TILE_W, CUBOID_BASE_Y, cuboidGridZ * TILE_D));
            cuboid->setScale(glm::vec3(0.5f, 1.0f, 0.5f));
            cuboid->setRotation(0.0f);
            printf("[Game] Pozitie resetata la (0,0).\n");
            break;
    }
    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y)
{
    // Nu miscam cuboid-ul in timp ce e in animatie
    if (cuboid->isAnimating()) return;

    int prevX = cuboidGridX;
    int prevZ = cuboidGridZ;

    switch (key)
    {
        case GLUT_KEY_LEFT:
            cuboidGridX --;
            cuboid->moveLeft();
            break;
            
        case GLUT_KEY_RIGHT:
            cuboidGridX ++;
            cuboid->moveRight();
            break;

        case GLUT_KEY_UP:
            cuboidGridZ --;
            cuboid->moveUp();
            break;

        case GLUT_KEY_DOWN:
            cuboidGridZ ++;
            cuboid->moveDown();
            break;

        default:
            return;
    }

    printf("[Game] Cuboid mutat la (%d, %d)\n", cuboidGridX, cuboidGridZ);

    onCuboidLeft(prevX, prevZ);
    onCuboidLanded();

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(900, 700);
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

//TODO: De accentuat in partea de sus muchiile la tile-uri