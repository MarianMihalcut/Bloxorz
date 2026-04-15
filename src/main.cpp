#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <chrono>
#include <GL/glu.h>

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

// Y-centrul cuboidului in picioare deasupra unui tile:
//   fata superioara tile = TILE_H/2 = 0.1
//   jumatatea inaltimii cuboidului in picioare (scale.y=2 → Ly=1.0)
static constexpr float CUBOID_STAND_Y = TILE_H / 2.0f + 1.0f; // 1.1f

// Pozitia si scala initiala
static const glm::vec3 START_POS   = glm::vec3(0.0f, CUBOID_STAND_Y, 0.0f);
static const glm::vec3 START_SCALE = glm::vec3(1.0f, 2.0f, 1.0f);

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
glm::vec3 lightPos(3.0f, 8.0f, 3.0f);
glm::vec3 viewPos(4.0f, 8.0f, 10.0f);
glm::vec3 viewTarget(2.5f, 0.0f, 1.5f); // centrul nivelului

int windowW = 900, windowH = 700;

// Pentru delta time
high_resolution_clock::time_point lastTime;
bool firstFrame = true;

// Pozitia cuboid-ului in grila (coloana, rand)
int cuboidGridX = 0;
int cuboidGridZ = 0;

// ---------------------------------------------------------------------------
// Stare joc
// ---------------------------------------------------------------------------
enum class GameState { PLAYING, GAME_OVER };
GameState gameState    = GameState::PLAYING;
float gameOverTimer = 0.0f;
static constexpr float GAME_OVER_DURATION = 2.0f; // secunde pana la reset

// ---------------------------------------------------------------------------
// Helper: tile-urile ocupate de cuboid, calculate din pozitia si scala world
//
// Cuboidul poate ocupa 1 sau 2 tile-uri.
// Centrele tile-urilor sunt la pozitii intregi (gx * TILE_W, 0, gz * TILE_D).
// Formula: din footprint [pos - half, pos + half] pe X si Z,
//          gasim toate centrele de tile incluse.
// ---------------------------------------------------------------------------
std::vector<std::pair<int,int>> getOccupiedTiles() {
    glm::vec3 pos = cuboid->getPosition();
    glm::vec3 scl = cuboid->getScale();

    float hx = scl.x / 2.0f;
    float hz = scl.z / 2.0f;

    // Primul si ultimul tile pe fiecare axa
    // (pos - half + 0.5) rotunjit = primul centru de tile acoperit
    int x1 = (int)roundf(pos.x - hx + 0.5f);
    int x2 = (int)roundf(pos.x + hx - 0.5f);
    int z1 = (int)roundf(pos.z - hz + 0.5f);
    int z2 = (int)roundf(pos.z + hz - 0.5f);

    std::vector<std::pair<int,int>> result;
    for (int x = x1; x <= x2; x++)
        for (int z = z1; z <= z2; z++)
            result.push_back({x, z});
    return result;
}

Tile* getTileAt(int gx, int gz) {
    for (auto& t : tiles) {
        if (t->getGridX() == gx && t->getGridZ() == gz) {
            return t.get();
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Randare text 2D (overlay Game Over)
// Foloseste fixed-function pipeline (context compatibility) + GLUT bitmap fonts
// ---------------------------------------------------------------------------
/*void renderText2D(const std::string& text, int x, int y,
                  float r = 1.0f, float g = 0.0f, float b = 0.0f)
{
    // Dezactivam shader-ul curent
    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowW, 0, windowH);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(r, g, b);
    glWindowPos2i(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}*/

// ---------------------------------------------------------------------------
// Logica joc: verificare pozitie dupa fiecare miscare terminata
// Apelata din specialKeyboard() si din display() cand animatia tocmai s-a incheiat
// ---------------------------------------------------------------------------
void checkLanding() {
    if (cuboid->isAnimating()) return;
    if (gameState == GameState::GAME_OVER) return;

    auto occupied = getOccupiedTiles();

    // Verificam ca TOATE tile-urile ocupate exista si sunt active
    for (auto [tx, tz] : occupied) {
        Tile* t = getTileAt(tx, tz);
        if (!t || !t->isActive()) {
            printf("[Game] Bloc a cazut la tile (%d, %d)! GAME OVER.\n", tx, tz);
            gameState     = GameState::GAME_OVER;
            gameOverTimer = 0.0f;
            return;
        }
    }

    // Aterizare valida: verificam butoane
    for (auto [tx, tz] : occupied) {
        Tile* t = getTileAt(tx, tz);
        if (t && t->getType() == TileType::BUTTON) {
            auto* btn = static_cast<ButtonTile*>(t);
            btn->press();
            printf("[Game] Buton apasat la (%d, %d).\n", tx, tz);
        }
    }

    // Log pozitie curenta
    printf("[Game] Bloc pe tile-uri: ");
    for (auto [tx, tz] : occupied) printf("(%d,%d) ", tx, tz);
    printf("\n");
}

// Apelata cand blocul pleaca de pe un tile cu buton
void releaseButtons(const std::vector<std::pair<int,int>>& prevOccupied) {
    for (auto [tx, tz] : prevOccupied) {
        Tile* t = getTileAt(tx, tz);
        if (t && t->getType() == TileType::BUTTON) {
            auto* btn = static_cast<ButtonTile*>(t);
            if (btn->getMode() == ButtonTile::ButtonMode::HOLD) {
                btn->release();
                printf("[Game] Buton eliberat la (%d, %d).\n", tx, tz);
            }
        }
    }
}

// Reset complet la pozitia initiala
void resetGame() {
    cuboid->setPosition(START_POS);
    cuboid->setScale(START_SCALE);
    cuboid->setRotation(0.0f);
    gameState     = GameState::PLAYING;
    gameOverTimer = 0.0f;
    printf("[Game] Reset. Bloc la pozitia initiala.\n");
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

    printf("[Game] Nivel incarcat. Tile-uri: %zu\n", tiles.size());
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

    // --- Update tile-uri---
    cuboid->update(deltaTime);

    for (auto& t : tiles) {
        t->update(deltaTime);
    }

    if (gameState == GameState::PLAYING) {
        // Update cuboid
        bool wasAnimating = cuboid->isAnimating();
        cuboid->update(deltaTime);
        bool isNowIdle = wasAnimating && !cuboid->isAnimating();

        // Verificam aterizarea exact cand animatia s-a incheiat
        if (isNowIdle)
            checkLanding();

    } else { // GAME_OVER
        gameOverTimer += deltaTime;
        if (gameOverTimer >= GAME_OVER_DURATION)
            resetGame();
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

    // --- Overlay Game Over ---
    /*if (gameState == GameState::GAME_OVER) {
        // Fundal semi-transparent (quad rosu inchis peste tot ecranul)
        glUseProgram(0);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, windowW, 0, windowH);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor4f(0.5f, 0.0f, 0.0f, 0.45f);
        glBegin(GL_QUADS);
        glVertex2i(0,       0);
        glVertex2i(windowW, 0);
        glVertex2i(windowW, windowH);
        glVertex2i(0,       windowH);
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // Text "GAME OVER"
        int textX = windowW / 2 - 100;
        int textY = windowH / 2 + 10;
        renderText2D("GAME OVER", textX, textY, 1.0f, 1.0f, 1.0f);

        // Sub-text cu countdown
        int secondsLeft = (int)(GAME_OVER_DURATION - gameOverTimer) + 1;
        std::string sub = "Reset in " + std::to_string(secondsLeft) + "s...";
        renderText2D(sub, textX + 10, textY - 35, 0.9f, 0.9f, 0.9f);
    }*/

    glutSwapBuffers();
    glFlush();

}

void init()
{
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glewInit();

    // Cuboid-ul porneste deasupra tile-ului (0,0)
    cuboid = new Cuboid(START_POS, START_SCALE, glm::vec3(0.2f, 0.6f, 1.0f));
    cuboid->init();

    buildLevel();

    // Verificam pozitia de start
    checkLanding();

    printf("[Game] Level construit. Folositi sagetile pentru a muta blocul.\n");
    printf("[Game] Gasiti butonul pentru a activa podul!\n");
}

void reshape(int w, int h)
{
    windowW = w;
    windowH = h;
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
        case 'R':
            resetGame();
            break;
    }
    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y)
{
    // Nu miscam cuboid-ul in timp ce e in animatie
    if (cuboid->isAnimating()) return;

    if (gameState == GameState::GAME_OVER) return; // blocat la game over

    // Salvam tile-urile ocupate INAINTE de miscare (pentru release butoane HOLD)
    auto prevOccupied = getOccupiedTiles();

    switch (key)
    {
        case GLUT_KEY_LEFT:
            cuboid->moveLeft();
            break;
            
        case GLUT_KEY_RIGHT:
            cuboid->moveRight();
            break;

        case GLUT_KEY_UP:
            cuboid->moveUp();
            break;

        case GLUT_KEY_DOWN:
            cuboid->moveDown();
            break;

        default:
            return;
    }

    // Eliberam butoanele HOLD de pe care am plecat
    releaseButtons(prevOccupied);

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(windowW, windowH);
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