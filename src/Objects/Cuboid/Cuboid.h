//
// Created by Marian on 11.04.2026.
//

#ifndef BLOXORZ_SPG_CUBOID_H
#define BLOXORZ_SPG_CUBOID_H

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include "../ObjectModelInterface.h"

namespace ObjectModel {

    /**
     * @brief Stările posibile ale animației cuboidului.
     */
    enum AnimationState {
        IDLE,          ///< Fără animație în desfășurare
        MOVING_UP,     ///< Deplasare în direcția -Z
        MOVING_DOWN,   ///< Deplasare în direcția +Z
        MOVING_LEFT,   ///< Deplasare în direcția -X
        MOVING_RIGHT,  ///< Deplasare în direcția +X
        STANDING_UP,   ///< Tranziție din culcat în picioare
        LAYING_DOWN    ///< Tranziție din picioare în culcat (nefolosit explicit)
    };

    /**
     * @brief Reprezintă cuboidul (blocul) jucătorului.
     *
     * Gestionează geometria, textura, umbrele și animațiile de mișcare/răsturnare.
     */
    class Cuboid : public ObjectModelInterface {
    private:
        // --- Poziție și țintă pentru animație ---
        glm::vec3 position;        ///< Poziția curentă în lume
        glm::vec3 targetPosition;  ///< Poziția țintă în timpul animației
        glm::vec3 scale;           ///< Dimensiunea curentă (lățime, înălțime, adâncime)
        glm::vec3 targetScale;     ///< Dimensiunea țintă în timpul animației
        glm::vec3 color;           ///< Culoarea de bază (utilizată în lipsa texturii)
        float rotation;            ///< Unghiul de rotație curent (în radiani)
        float targetRotation;      ///< Unghiul țintă în timpul animației
        glm::vec3 rotationAxis;    ///< Axa de rotație (utilizată pentru animații)

        // --- Date despre geometrie ---
        int nrFaces;               ///< Numărul de fețe (6 pentru cuboid)
        int nrVerticesPerFace;     ///< Vertice pe față (6 pentru două triunghiuri)
        int nrVertices;            ///< Total vertice (36)
        std::vector<float> vertices; ///< Buffer cu datele geometrice (poziții, normale, culori, UV)

        // --- Matrice și lumini ---
        glm::mat4 projectionMatrix; ///< Matricea de proiecție (camera)
        glm::mat4 viewMatrix;       ///< Matricea de vizualizare (camera)
        glm::vec3 lightPos;         ///< Poziția sursei de lumină
        glm::vec3 viewPos;          ///< Poziția observatorului (camera)

        // --- Stare și animație ---
        bool isStanding;            ///< `true` dacă blocul este în picioare (scale.y > 1.5)
        AnimationState animState;   ///< Starea curentă a animației
        float animProgress;         ///< Progresul animației (0.0 → 1.0)
        float animSpeed;            ///< Viteza animației (unități pe secundă)

        // --- Textură și umbre ---
        GLuint textureID;           ///< ID-ul texturii principale (cuboid_wall.jpg)
        GLuint shadowMapFBO;        ///< Framebuffer pentru harta de umbre
        GLuint shadowMapTexture;    ///< Textura de adâncime pentru umbre
        GLint shadowMapSize;        ///< Rezoluția hărții de umbre (2048x2048)

        /**
         * @brief Generează vectorul `vertices` pe baza `scale`, `color` și a coordonatelor de textură.
         */
        void generateVertices();

        /**
         * @brief Pornește o animație către noile ținte.
         * @param state Starea de animație (MOVING_UP, STANDING_UP etc.)
         * @param newPos Poziția finală
         * @param newScale Dimensiunea finală
         * @param newRot Unghiul final (radiani)
         * @param rotAxis Axa de rotație folosită în animație
         */
        void startAnimation(AnimationState state, glm::vec3 newPos, glm::vec3 newScale,
                            float newRot, glm::vec3 rotAxis);

    public:
        /**
         * @brief Constructor implicit.
         *
         * Inițializează blocul în poziție verticală la (0, 1.1, 0), dimensiuni (1,2,1),
         * culoare albastră, viteza animației 2.0.
         */
        Cuboid();

        /**
         * @brief Constructor cu parametri.
         * @param pos Poziția inițială
         * @param scl Dimensiunea inițială
         * @param col Culoarea inițială (ignorată dacă textura este activă)
         */
        Cuboid(glm::vec3 pos, glm::vec3 scl, glm::vec3 col);

        /**
         * @brief Destructor.
         *
         * Eliberează resursele OpenGL (VAO, VBO, shader program).
         */
        ~Cuboid();

        // --- Interfață ObjectModelInterface ---
        /**
         * @brief Inițializează resursele OpenGL (shadere, VBO, VAO, textură, FBO pentru umbre).
         */
        void init() override;

        /**
         * @brief Randează cuboidul în scena curentă.
         *
         * Configurează uniform-urile (MVP, normalMatrix, lightSpaceMatrix etc.) și
         * desenează geometria cu textura și harta de umbre.
         */
        void display() override;

        /**
         * @brief Actualizează logica animației.
         * @param deltaTime Timpul scurs de la ultimul cadru (în secunde)
         */
        void update(float deltaTime) override;

        // --- Setteri pentru matrice și lumină ---
        /**
         * @brief Setează matricea de proiecție.
         * @param proj Matricea de proiecție (de obicei perspectivă)
         */
        void setProjectionMatrix(glm::mat4 proj);

        /**
         * @brief Setează matricea de vizualizare (view).
         * @param view Matricea camerei
         */
        void setViewMatrix(glm::mat4 view);

        /**
         * @brief Setează poziția sursei de lumină.
         * @param light Coordonatele lumini în spațiul lumii
         */
        void setLightPos(glm::vec3 light);

        /**
         * @brief Setează poziția observatorului (camera).
         * @param vPos Coordonatele camerei
         */
        void setViewPos(glm::vec3 vPos);

        // --- Setteri pentru proprietățile geometrice ---
        /**
         * @brief Setează poziția blocului (fără animație).
         * @param pos Noua poziție
         */
        void setPosition(glm::vec3 pos);

        /**
         * @brief Setează dimensiunile blocului (fără animație).
         * @param scl Noua scală (lățime, înălțime, adâncime)
         */
        void setScale(glm::vec3 scl);

        /**
         * @brief Setează culoarea blocului (folosită când textura nu este activă).
         * @param col Noua culoare RGB
         */
        void setColor(glm::vec3 col);

        /**
         * @brief Setează unghiul de rotație (în radiani).
         * @param rot Unghiul (în jurul axei Y)
         */
        void setRotation(float rot);

        // --- Getteri ---
        /**
         * @brief Returnează poziția curentă a blocului.
         * @return Poziția în spațiul lumii
         */
        glm::vec3 getPosition();

        /**
         * @brief Returnează dimensiunea curentă.
         * @return Vector (lățime, înălțime, adâncime)
         */
        glm::vec3 getScale();

        /**
         * @brief Verifică dacă blocul este în picioare (scale.y > 1.5).
         * @return `true` dacă este vertical, `false` dacă este culcat
         */
        bool getIsStanding();

        /**
         * @brief Verifică dacă există o animație în curs.
         * @return `true` dacă animState != IDLE
         */
        bool isAnimating();

        // --- Comenzi de mișcare ---
        /**
         * @brief Deplasează blocul în direcția -Z (în sus pe ecran).
         *
         * Dacă blocul este în picioare, se răstoarnă pe Z și coboară Y.
         * Dacă este culcat pe Z, se ridică în picioare.
         * Dacă este culcat pe X, alunecă o unitate pe -Z.
         */
        void moveUp();

        /**
         * @brief Deplasează blocul în direcția +Z (în jos pe ecran).
         */
        void moveDown();

        /**
         * @brief Deplasează blocul în direcția -X (stânga).
         */
        void moveLeft();

        /**
         * @brief Deplasează blocul în direcția +X (dreapta).
         */
        void moveRight();

        /**
         * @brief Actualizează câmpul `isStanding` pe baza scale.y.
         */
        void updateOrientation();
    };

} // namespace ObjectModel

#endif //BLOXORZ_SPG_CUBOID_H