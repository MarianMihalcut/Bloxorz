//
// Created by Marian on 17.04.2026.
//

#ifndef BLOXORZ_SPG_GAME_H
#define BLOXORZ_SPG_GAME_H

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <memory>
#include <string>
#include <chrono>

#include <glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "src/Levels/Level.h"
#include "src/Objects/Cuboid/Cuboid.h"
#include "src/Objects/Tiles/Tile.h"
#include "src/Objects/Tiles/ButtonTile.h"
#include "src/Objects/Tiles/BridgeTile.h"
#include "src/Objects/Tiles/FinishTile.h"

using namespace ObjectModel;
using namespace std::chrono;

// ---------------------------------------------------------------------------
// Constante fizice (trebuie sa corespunda cu scalele din Tile si Cuboid)
// ---------------------------------------------------------------------------
static constexpr float TILE_H         = 0.2f;
static constexpr float CUBOID_STAND_Y = TILE_H / 2.0f + 1.0f; // 1.1f
static const     glm::vec3 STAND_SCALE = glm::vec3(1.0f, 2.0f, 1.0f);


class Game {
public:
    // -----------------------------------------------------------------------
    // Stari posibile
    // -----------------------------------------------------------------------
    enum class State {
        PLAYING,         // ne aflam in timpul jocului(stare implicita)
        GAME_OVER,       // bloc a cazut - asteapta reset
        LEVEL_TRANSITION,// scurt delay intre niveluri
        GAME_COMPLETE    // jucatorul a terminat ultimul nivel
    };

private:
    // -----------------------------------------------------------------------
    // Singleton - GLUT are nevoie de callback-uri statice
    // -----------------------------------------------------------------------
    static Game* instance;

    // -----------------------------------------------------------------------
    // Obiecte scena
    // -----------------------------------------------------------------------
    Cuboid* cuboid = nullptr;
    std::vector<std::unique_ptr<Tile>> tiles;

    // -----------------------------------------------------------------------
    // Niveluri
    // -----------------------------------------------------------------------
    int currentLevelIdx = 0;  // 0 = Level1, 1 = Level2

    // -----------------------------------------------------------------------
    // Camera
    // -----------------------------------------------------------------------
    glm::mat4 projectionMatrix, viewMatrix;
    glm::vec3 lightPos  = glm::vec3(3.0f, 8.0f,  3.0f);
    glm::vec3 viewPos   = glm::vec3(2.5f, 10.0f, 12.0f);
    glm::vec3 viewTarget= glm::vec3(2.5f,  0.0f,  1.5f);
    int windowW = 1000, windowH = 900;

    // -----------------------------------------------------------------------
    // Stare joc
    // -----------------------------------------------------------------------
    State gameState = State::PLAYING;
    float stateTimer = 0.0f;
    static constexpr float GAME_OVER_DELAY       = 2.0f;
    static constexpr float TRANSITION_DELAY      = 1.5f;

    // -----------------------------------------------------------------------
    // Contor mutari (persista intre niveluri)
    // -----------------------------------------------------------------------
    int moveCount = 0;

    // -----------------------------------------------------------------------
    // Delta time
    // -----------------------------------------------------------------------
    high_resolution_clock::time_point lastTime;
    bool firstFrame = true;

    // -----------------------------------------------------------------------
    // Metode private
    // -----------------------------------------------------------------------
    void loadLevel(int idx);
    void resetCurrentLevel();

    // Logica de coliziune / interactiune
    std::vector<std::pair<int,int>> getOccupiedTiles() const;
    Tile* getTileAt(int gx, int gz) const;
    void  checkLanding();
    void  releaseButtons(const std::vector<std::pair<int,int>>& prev);

    // Tranzitie la urmatorul nivel
    void advanceLevel();

    // Randare overlay 2D (text)
    void renderText2D(const std::string& text, int x, int y,
                      float r = 1.f, float g = 1.f, float b = 1.f) const;
    void renderOverlay() const;

    // Actualizeaza viewMatrix dupa schimbarea camerei
    void updateViewMatrix();

public:
    Game();
    ~Game();

    static Game* getInstance(); //necesar pt a crea singleton-ul pe clasa Game

    void init(int windowW, int windowH);
    void reshape(int w, int h);

    // Apelate din callbacks GLUT
    void onDisplay();
    void onKeyboard(unsigned char key, int x, int y);
    void onSpecialKey(int key, int x, int y);

    //CB este callback
    // Callbacks statice pentru GLUT
    static void displayCB();
    static void reshapeCB(int w, int h);
    static void keyboardCB(unsigned char key, int x, int y); //pt tastele alfa-numerice
    static void specialCB(int key, int x, int y); //pt. tastele de tip sageata(foloseste constante GLUT)
    static void idleCB();
};



#endif //BLOXORZ_SPG_GAME_H
