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

#include "../Levels/Headers/Level.h"
#include "src/Objects/Cuboid/Cuboid.h"
#include "../Objects/Tiles/Headers/Tile.h"
#include "src/Objects/Tiles/Headers//ButtonTile.h"
#include "../Objects/Tiles/Headers/BridgeTile.h"
#include "../Objects/Tiles/Headers/FinishTile.h"

using namespace ObjectModel;
using namespace std::chrono;

// ---------------------------------------------------------------------------
// Constante fizice (trebuie sa corespunda cu scalele din Tile si Cuboid)
// ---------------------------------------------------------------------------
static constexpr float TILE_H         = 0.2f;          ///< Înălțimea unui tile
static constexpr float CUBOID_STAND_Y = TILE_H / 2.0f + 1.0f; ///< Poziția Y când blocul stă în picioare (1.1f)
static const     glm::vec3 STAND_SCALE = glm::vec3(1.0f, 2.0f, 1.0f); ///< Dimensiunea blocului în picioare

/**
 * @brief Clasa principală a jocului.
 *
 * Gestionează logica jocului, încărcarea nivelurilor, camera, input-ul,
 * animațiile și tranzițiile între stări (PLAYING, GAME_OVER, etc.).
 *
 * Implementează un singleton pentru a putea fi folosită în callback-urile statice GLUT.
 */
class Game {
public:
    // -----------------------------------------------------------------------
    // Stari posibile
    // -----------------------------------------------------------------------
    /**
     * @brief Stările posibile ale jocului.
     */
    enum class State {
        PLAYING,          ///< Joc normal, playerul controlează blocul
        GAME_OVER,        ///< Blocul a căzut – așteaptă resetarea nivelului
        LEVEL_TRANSITION, ///< Tranziție între niveluri (scurt delay)
        GAME_COMPLETE     ///< Jucătorul a terminat ultimul nivel
    };

private:
    // -----------------------------------------------------------------------
    // Singleton - GLUT are nevoie de callback-uri statice
    // -----------------------------------------------------------------------
    static Game* instance; ///< Instanța unică a jocului (singleton)

    // -----------------------------------------------------------------------
    // Obiecte scena
    // -----------------------------------------------------------------------
    Cuboid* cuboid = nullptr;                     ///< Blocul controlat de jucător
    std::vector<std::unique_ptr<Tile>> tiles;     ///< Toate tile-urile nivelului curent

    // -----------------------------------------------------------------------
    // Niveluri
    // -----------------------------------------------------------------------
    int currentLevelIdx = 0;  ///< Indexul nivelului curent (0 = Level1, 1 = Level2)

    // -----------------------------------------------------------------------
    // Camera
    // -----------------------------------------------------------------------
    glm::mat4 projectionMatrix;   ///< Matricea de proiecție (perspectivă)
    glm::mat4 viewMatrix;         ///< Matricea de vizualizare (camera)
    glm::vec3 lightPos  = glm::vec3(3.0f, 8.0f,  3.0f); ///< Poziția sursei de lumină
    glm::vec3 viewPos   = glm::vec3(2.5f, 10.0f, 12.0f); ///< Poziția camerei
    glm::vec3 viewTarget= glm::vec3(2.5f,  0.0f,  1.5f); ///< Punctul urmărit de cameră
    int windowW = 1000, windowH = 900;           ///< Dimensiunile ferestrei

    // -----------------------------------------------------------------------
    // Stare joc
    // -----------------------------------------------------------------------
    State gameState = State::PLAYING;    ///< Starea curentă a jocului
    float stateTimer = 0.0f;             ///< Timer pentru stările temporare (GAME_OVER, TRANSITION)
    static constexpr float GAME_OVER_DELAY       = 2.0f;   ///< Secunde până la resetare după Game Over
    static constexpr float TRANSITION_DELAY      = 1.5f;   ///< Secunde de pauză între niveluri

    // -----------------------------------------------------------------------
    // Contor mutari (persista intre niveluri)
    // -----------------------------------------------------------------------
    int moveCount = 0;   ///< Numărul total de mișcări efectuate în sesiunea curentă

    // -----------------------------------------------------------------------
    // Delta time
    // -----------------------------------------------------------------------
    high_resolution_clock::time_point lastTime;  ///< Momentul ultimului cadru
    bool firstFrame = true;                      ///< Flag pentru inițializarea delta time-ului

    // Global lighting parameters
    static constexpr glm::vec3 GLOBAL_LIGHT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);   ///< Culoarea luminii
    static constexpr float GLOBAL_LIGHT_INTENSITY = 1.0f;                           ///< Intensitatea luminii

    // -----------------------------------------------------------------------
    // Metode private
    // -----------------------------------------------------------------------
    /**
     * @brief Încarcă un nivel după index.
     * @param idx Indexul nivelului (0, 1, ...)
     */
    void loadLevel(int idx);

    /// Resetează nivelul curent (reîncarcă aceleași date).
    void resetCurrentLevel();

    // Logica de coliziune / interactiune
    /**
     * @brief Returnează coordonatele în grilă ocupate de cuboid.
     * @return Listă de perechi (X, Z) ale tile-urilor acoperite.
     */
    std::vector<std::pair<int,int>> getOccupiedTiles() const;

    /**
     * @brief Returnează pointer la tile-ul de la coordonatele date.
     * @param gx Coordonata X în grilă
     * @param gz Coordonata Z în grilă
     * @return Pointer la tile, sau nullptr dacă nu există.
     */
    Tile* getTileAt(int gx, int gz) const;

    /**
     * @brief Verifică dacă aterizarea este validă și aplică efectele butoanelor/finish.
     * @details Se apelează după ce cuboidul își termină animația de mișcare.
     */
    void checkLanding();

    /**
     * @brief Eliberează butoanele de tip HOLD care nu mai sunt acoperite de cuboid.
     * @param prev Lista tile-urilor ocupate înainte de mutare.
     */
    void releaseButtons(const std::vector<std::pair<int,int>>& prev);

    /// Avansează la nivelul următor (apelează finish callback-ul).
    void advanceLevel();

    // Randare overlay 2D (text)
    /**
     * @brief Desenează text 2D la coordonatele date.
     * @param text Șirul de caractere
     * @param x Coordonata X (pixeli de la stânga)
     * @param y Coordonata Y (pixeli de jos)
     * @param r Componenta roșu (0-1)
     * @param g Componenta verde (0-1)
     * @param b Componenta albastru (0-1)
     */
    void renderText2D(const std::string& text, int x, int y,
                      float r = 1.f, float g = 1.f, float b = 1.f) const;

    /// Desenează toate elementele UI (contor mutări, nivel, mesaje stare).
    void renderOverlay() const;

    /**
     * @brief Desenează un dreptunghi colorat pe întreg ecranul (pentru efecte de tranziție).
     * @param r Roșu (0-1)
     * @param g Verde (0-1)
     * @param b Albastru (0-1)
     * @param a Alfa (transparență)
     */
    void drawFullscreenOverlay(float r, float g, float b, float a) const;

    /// Actualizează viewMatrix pe baza viewPos și viewTarget.
    void updateViewMatrix();

public:
    /**
     * @brief Constructor.
     */
    Game();

    /// Destructor.
    ~Game();

    /**
     * @brief Setează poziția luminii globale și o propagă către cuboid și tile-uri.
     * @param pos Noua poziție a luminii.
     */
    void setGlobalLightPos(glm::vec3 pos) {
        lightPos = pos;
        cuboid->setLightPos(pos);
        for (auto& tile : tiles)
            tile->setLightPos(pos);
    }

    /**
     * @brief Returnează instanța unică a jocului (singleton).
     * @return Pointer la instanța Game.
     */
    static Game* getInstance();

    /**
     * @brief Inițializează jocul (OpenGL, cuboid, nivelul 0).
     * @param windowW Lățimea ferestrei
     * @param windowH Înălțimea ferestrei
     */
    void init(int windowW, int windowH);

    /**
     * @brief Reacționează la redimensionarea ferestrei.
     * @param w Noua lățime
     * @param h Noua înălțime
     */
    void reshape(int w, int h);

    // Apelate din callbacks GLUT
    void onDisplay();                        ///< Randare scenă
    void onKeyboard(unsigned char key, int x, int y);  ///< Gestionare taste alfanumerice
    void onSpecialKey(int key, int x, int y);          ///< Gestionare taste săgeți

    // Callbacks statice pentru GLUT
    static void displayCB();          ///< Callback pentru afișare
    static void reshapeCB(int w, int h);  ///< Callback pentru redimensionare
    static void keyboardCB(unsigned char key, int x, int y); ///< Callback taste normale
    static void specialCB(int key, int x, int y);            ///< Callback taste speciale
    static void idleCB();             ///< Callback pentru idle (redesenează)
};

#endif //BLOXORZ_SPG_GAME_H