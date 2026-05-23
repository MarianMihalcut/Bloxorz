//
// Created by Marian on 17.04.2026.
//

#include "Game.h"
#include "../Levels/Headers/Level1.h"
#include "../Levels/Headers/Level2.h"

#include <GL/glu.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <cmath>

#define PI glm::pi<float>()

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------
Game* Game::instance = nullptr;

Game* Game::getInstance() {
    if (!instance) instance = new Game();
    return instance;
}

Game::Game()  = default; // constructor default
Game::~Game() { delete cuboid; } //la terminarea jocului se sterge cuboidul

// ---------------------------------------------------------------------------
// Incarcare nivel
// ---------------------------------------------------------------------------
void Game::loadLevel(int idx) {
    // Curatam tile-urile anterioare
    tiles.clear();

    std::unique_ptr<Levels::Level> level;

    if (idx == 0)
        level = std::make_unique<Levels::Level1>();
    else if (idx == 1)
        level = std::make_unique<Levels::Level2>();
    else
        return; // nu exista nivelul

    level->buildLevel();

    // Setam callback-ul de finish pe toate FinishTile-urile din nivel
    for (auto& t : level->getTiles()) {
        if (t->getType() == TileType::FINISH) {
            auto* ft = static_cast<FinishTile*>(t.get());
            ft->setOnFinish([this]() { advanceLevel(); });
        }
    }

    // Mutam tile-urile in vectorul propriu
    for (auto& t : level->getTiles())
        tiles.push_back(std::move(t));

    // Pozitionam cuboidul la startul nivelului
    glm::ivec2 startGrid = level->getStartGridPos();
    glm::vec3  startPos  = glm::vec3(startGrid.x * 1.0f, CUBOID_STAND_Y,
                                     startGrid.y * 1.0f);
    cuboid->setPosition(startPos);
    cuboid->setScale(STAND_SCALE);
    cuboid->setRotation(0.0f);

    // Actualizam camera pentru noul nivel
    viewPos    = level->getCameraPos();
    viewTarget = level->getCameraTarget();

    updateViewMatrix();

    gameState  = State::PLAYING;
    stateTimer = 0.0f;

    checkLanding();
    printf("[Game] Nivel %d incarcat. Mutari totale: %d\n", idx + 1, moveCount);
}

void Game::resetCurrentLevel() {
    loadLevel(currentLevelIdx);
}

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------
void Game::init(int w, int h) {
    windowW = w; windowH = h;

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version  = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version: %s\n", version);

    glClearColor(0.53f, 0.73f, 0.87f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glewInit();

    cuboid = new Cuboid(glm::vec3(0.0f, CUBOID_STAND_Y, 0.0f),
                        STAND_SCALE,
                        glm::vec3(1.0f, 0.0f, 0.0f));
    cuboid->init();

    currentLevelIdx = 0;
    moveCount       = 0;
    loadLevel(0);
}

// ---------------------------------------------------------------------------
// Reshape
// ---------------------------------------------------------------------------
void Game::reshape(int w, int h) {
    windowW = w; windowH = h;
    glViewport(0, 0, w, h);
    projectionMatrix = glm::perspective(PI / 4.0f, (float)w / h, 0.1f, 200.0f);
    updateViewMatrix();
}

void Game::updateViewMatrix() {
    viewMatrix = glm::lookAt(viewPos, viewTarget, glm::vec3(0.0f, 1.0f, 0.0f));
}

// ---------------------------------------------------------------------------
// Tile helpers
// ---------------------------------------------------------------------------
std::vector<std::pair<int,int>> Game::getOccupiedTiles() const {
    glm::vec3 pos = cuboid->getPosition();
    glm::vec3 scl = cuboid->getScale();
    float hx = scl.x / 2.0f, hz = scl.z / 2.0f;
    int x1 = (int)roundf(pos.x - hx + 0.5f), x2 = (int)roundf(pos.x + hx - 0.5f);
    int z1 = (int)roundf(pos.z - hz + 0.5f), z2 = (int)roundf(pos.z + hz - 0.5f);
    std::vector<std::pair<int,int>> result;
    for (int x = x1; x <= x2; x++)
        for (int z = z1; z <= z2; z++)
            result.push_back({x, z});
    return result;
}

Tile* Game::getTileAt(int gx, int gz) const {
    for (auto& t : tiles)
        if (t->getGridX() == gx && t->getGridZ() == gz)
            return t.get();
    return nullptr;
}

// ---------------------------------------------------------------------------
// Logica de aterizare
// ---------------------------------------------------------------------------
void Game::checkLanding() {
    if (cuboid->isAnimating()) return;
    if (gameState != State::PLAYING) return;

    auto occupied = getOccupiedTiles();

    // Verificam ca TOATE tile-urile ocupate exista si sunt active
    for (auto [tx, tz] : occupied) {
        Tile* t = getTileAt(tx, tz);
        if (!t || !t->isActive()) {
            printf("[Game] Bloc a cazut la (%d,%d)! GAME OVER.\n", tx, tz);
            gameState  = State::GAME_OVER;
            stateTimer = 0.0f;
            return;
        }
    }

    // Aterizare valida - interactiuni
    bool isStanding = cuboid->getIsStanding();

    for (auto [tx, tz] : occupied) {
        Tile* t = getTileAt(tx, tz);
        if (!t) continue;

        if (t->getType() == TileType::BUTTON) {
            auto* btn = static_cast<ButtonTile*>(t);
            btn->press();
            printf("[Game] Buton apasat la (%d,%d).\n", tx, tz);
        }

        // FinishTile se activeaza DOAR cand blocul e in picioare (ocupa 1 tile)
        if (t->getType() == TileType::FINISH && isStanding && occupied.size() == 1) {
            static_cast<FinishTile*>(t)->triggerFinish();
            return;
        }
    }

    printf("[Game] Bloc la: ");
    for (auto [tx, tz] : occupied) printf("(%d,%d) ", tx, tz);
}

void Game::releaseButtons(const std::vector<std::pair<int,int>>& prev) {
    for (auto [tx, tz] : prev) {
        Tile* t = getTileAt(tx, tz);
        if (t && t->getType() == TileType::BUTTON) {
            auto* btn = static_cast<ButtonTile*>(t);
            if (btn->getMode() == ButtonTile::ButtonMode::HOLD)
                btn->release();
        }
    }
}

// ---------------------------------------------------------------------------
// Tranzitie nivel
// ---------------------------------------------------------------------------
void Game::advanceLevel() {
    if (currentLevelIdx + 1 < 2) {
        // Mai exista un nivel urmator
        gameState  = State::LEVEL_TRANSITION;
        stateTimer = 0.0f;
        printf("[Game] Nivel complet! Tranzitie la nivelul %d...\n", currentLevelIdx + 2);
    } else {
        // Ultimul nivel terminat
        gameState  = State::GAME_COMPLETE;
        stateTimer = 0.0f;
    }
}

// ---------------------------------------------------------------------------
// Display
// ---------------------------------------------------------------------------
void Game::onDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Delta time
    if (firstFrame) {
        lastTime = high_resolution_clock::now();
        firstFrame = false;
    }
    auto  now= high_resolution_clock::now();
    float deltaTime = duration<float>(now - lastTime).count();
    lastTime = now;

    // Update tile-uri (animatii bridge etc.)
    for (auto& t : tiles)
        t->update(deltaTime);

    // Update stare joc
    switch (gameState) {
        case State::PLAYING: {
            bool wasAnim = cuboid->isAnimating();
            cuboid->update(deltaTime);
            if (wasAnim && !cuboid->isAnimating())
                checkLanding();
            break;
        }
        case State::GAME_OVER: {
            stateTimer += deltaTime;
            if (stateTimer >= GAME_OVER_DELAY) {
                resetCurrentLevel();
            }
            break;
        }
        case State::LEVEL_TRANSITION: {
            stateTimer += deltaTime;
            if (stateTimer >= TRANSITION_DELAY) {
                currentLevelIdx++;
                loadLevel(currentLevelIdx);
            }
            break;
        }
        case State::GAME_COMPLETE:
            // Ramane in aceasta stare pana la reset manual
            break;
    }

    // Randare tile-uri
    glm::mat4 vp = projectionMatrix * viewMatrix;
    for (auto& t : tiles)
        t->render(vp, lightPos, viewPos);

    // Randare cuboid
    cuboid->setProjectionMatrix(projectionMatrix);
    cuboid->setViewMatrix(viewMatrix);
    cuboid->setLightPos(lightPos);
    cuboid->setViewPos(viewPos);
    cuboid->display();

    // Overlay 2D
    renderOverlay();

    glutSwapBuffers();
    glFlush();
}

// ---------------------------------------------------------------------------
// Overlay 2D
// ---------------------------------------------------------------------------
void Game::renderText2D(const std::string& text, int x, int y,
                         float r, float g, float b) const
{
    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    //glm::ortho(0.0f, (float) windowW, 0.0f, (float) windowH, -1.0f, 1.0f); - De uitat aici
    glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();

    glColor3f(r, g, b);
    glWindowPos2i(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

void Game::drawFullscreenOverlay(float r, float g, float b, float a) const {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(0); // Folosește pipeline-ul fix

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowW, 0, windowH, -1, 1); // Setează sistemul de coordonate în pixeli

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowW, 0);
    glVertex2f(windowW, windowH);
    glVertex2f(0, windowH);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Game::renderOverlay() const {
    // UI-ul persistent (Mereu vizibil)
    renderText2D("Mutari: " + std::to_string(moveCount), 10, windowH - 30, 1.0f, 1.0f, 1.0f);
    renderText2D("Nivel: " + std::to_string(currentLevelIdx + 1), 10, windowH - 60, 1.0f, 1.0f, 1.0f);

    // Overlay-uri specifice stării
    if (gameState == State::GAME_OVER) {
        drawFullscreenOverlay(0.5f, 0.0f, 0.0f, 0.45f); // Roșu transparent

        renderText2D("GAME OVER", windowW/2 - 80, windowH/2 + 10);
        int sec = (int)(GAME_OVER_DELAY - stateTimer) + 1;
        renderText2D("Reset in " + std::to_string(sec) + "s...", windowW/2 - 70, windowH/2 - 25);
    }
    else if (gameState == State::LEVEL_TRANSITION) {
        drawFullscreenOverlay(0.0f, 0.3f, 0.0f, 0.50f); // Verde transparent

        renderText2D("NIVEL COMPLET!", windowW/2 - 90, windowH/2 + 10, 0.2f, 1.0f, 0.4f);
        int sec = (int)(TRANSITION_DELAY - stateTimer) + 1;
        renderText2D("Urmatorul nivel in " + std::to_string(sec) + "s...", windowW/2 - 100, windowH/2 - 25);
    }
    else if (gameState == State::GAME_COMPLETE) {
        drawFullscreenOverlay(0.0f, 0.0f, 0.3f, 0.60f); // Albastru transparent

        renderText2D("JOC COMPLET!", windowW/2 - 80, windowH/2 + 30, 0.3f, 0.8f, 1.0f);
        renderText2D("Total mutari: " + std::to_string(moveCount), windowW/2 - 90, windowH/2, 1.0f, 1.0f, 0.3f);
        renderText2D("Apasa R pentru a relua", windowW/2 - 110, windowH/2 - 35, 0.8f, 0.8f, 0.8f);
    }
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------
void Game::onKeyboard(unsigned char key, int /*x*/, int /*y*/) {
    switch (key) {
        case 27: exit(0); break;
        case 'r': case 'R':
            moveCount       = 0;
            currentLevelIdx = 0;
            resetCurrentLevel();
            printf("[Game] Joc resetat.\n");
            break;
    }
    glutPostRedisplay();
}

void Game::onSpecialKey(int key, int /*x*/, int /*y*/) {
    if (cuboid->isAnimating()) return;
    if (gameState != State::PLAYING) return;

    auto prev = getOccupiedTiles();

    switch (key) {
        case GLUT_KEY_LEFT:
            cuboid->moveLeft();
            moveCount++;
            break;
        case GLUT_KEY_RIGHT:
            cuboid->moveRight();
            moveCount++;
            break;
        case GLUT_KEY_UP:
            cuboid->moveUp();
            moveCount++;
            break;
        case GLUT_KEY_DOWN:
            cuboid->moveDown();
            moveCount++;
            break;
        default:
            return;
    }
    releaseButtons(prev);
    glutPostRedisplay();
}

// ---------------------------------------------------------------------------
// Callbacks statice GLUT
// ---------------------------------------------------------------------------
void Game::displayCB()
    { getInstance()->onDisplay(); }
void Game::reshapeCB(int w, int h)
    { getInstance()->reshape(w, h); }
void Game::keyboardCB(unsigned char k, int x, int y)
    { getInstance()->onKeyboard(k, x, y); }
void Game::specialCB(int k, int x, int y)
    { getInstance()->onSpecialKey(k, x, y); }
void Game::idleCB()
    { glutPostRedisplay(); }