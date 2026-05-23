//
// Created by Marian on 14.04.2026.
//

#ifndef BLOXORZ_SPG_BUTTONTILE_H
#define BLOXORZ_SPG_BUTTONTILE_H

#include "Tile.h"
#include <functional>

#include "src/stb_image.h"

namespace ObjectModel {
    /// Un tile cu buton vizibil pe suprafata superioara.
    /// Cand blocul sta pe el, se apasa (coborat vizual) si cheama un callback.
    /// Geometria: tile-ul de baza + un indicator (disc/prisma) pe fata superioara.
    ///
    /// Redare in doua draw call-uri:
    ///   1. Corpul tile-ului  (culoare: gri inchis)
    ///   2. Indicatorul       (culoare: rosu nepasit / verde pasat)
    class ButtonTile : public Tile {
    public:
        /// Tipuri de buton (determina daca bridgeul se mentine sau nu)
        enum class ButtonMode {
            TOGGLE,  // fiecare apasare schimba starea bridge-ului
            HOLD     // bridge-ul e activ DOAR cat blocul sta pe buton
            /// Logica de HOLD nu e implementata in acest proiect
        };
    private:
        // --- Starea butonului ---
        bool pressed;
        ButtonMode mode;

        // --- Callback apelat la schimbarea starii (pressed/unpressed) ---
        std::function<void(bool)> onStateChange;

        // Textura
        GLuint textureId = 0;
        std::string texturePath;

        // Culori
        static constexpr glm::vec3 COLOR_BODY     = glm::vec3(0.25f, 0.25f, 0.30f); // gri inchis

        // -----------------------------------------------------------------------
        // Incarca textura PNG cu stb_image
        // -----------------------------------------------------------------------
        void loadTexture() {
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_set_flip_vertically_on_load(true); // OpenGL are Y inversat fata de imagini
            int w, h, channels;
            unsigned char* data = stbi_load(texturePath.c_str(), &w, &h, &channels, 0);
            if (data) {
                GLenum fmt = (channels == 4) ? GL_RGBA : GL_RGB;
                glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(data);
                printf("[ButtonTile] Textura incarcata: %s (%dx%d, %dch)\n",
                       texturePath.c_str(), w, h, channels);
            } else {
                printf("[ButtonTile] EROARE textura: %s\n", texturePath.c_str());
            }
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    public:
        /// @param gx, gz       - pozitie in grila
        /// @param mode         - TOGGLE sau HOLD
        /// @param callback     - functie apelata cu (true) la apasare, (false) la eliberare
        /// @param vertPath/fragPath - cai shadere
        ButtonTile(int gx, int gz, ButtonMode bMode = ButtonMode::TOGGLE,
                   std::function<void(bool)> callback = nullptr,
                   const std::string& texPath = "../src/Objects/Tiles/500px-Smiley.png",
                   const std::string& vertPath = "../src/Objects/Tiles/tile.vert",
                   const std::string& fragPath = "../src/Objects/Tiles/tile_button.frag")
            : Tile(gx, gz,
                   1.0f, 0.2f, 1.0f,
                   COLOR_BODY, vertPath, fragPath)
            , pressed(false)
            , mode(bMode)
            , onStateChange(std::move(callback))
            , texturePath(texPath)
        {}

        ~ButtonTile() override {
            if (textureId)
                glDeleteTextures(1, &textureId);
        }

        TileType getType() const override { return TileType::BUTTON; }
        bool isPressed()   const          { return pressed; }
        ButtonMode getMode() const        { return mode; }

        void init() override {
            loadShaders(vertShaderPath, fragShaderPath);

            // Corpul tile-ului
            std::vector<TileVertex> verts;
            std::vector<GLuint>     idxs;
            buildBox(tileWidth, tileHeight, tileDepth, baseColor, verts, idxs);
            setupMesh(verts, idxs);

            loadTexture();
        }

        /// Apelata de logica jocului cand blocul intra pe tile
        void press() {
            if (mode == ButtonMode::TOGGLE && pressed) {
                // A doua apasare pe TOGGLE: dezactiveaza
                pressed = false;
                if (onStateChange) onStateChange(false);
                printf("[ButtonTile] TOGGLE off la (%d,%d)\n", gridX, gridZ);
                return;
            }
            if (pressed) return; // HOLD deja apasat
            pressed = true;
            if (onStateChange) onStateChange(true);
            printf("[ButtonTile] Apasat la (%d,%d)\n", gridX, gridZ);
        }

        /// Apelata cand blocul paraseste tile-ul
        void release() {
            if (!pressed) return;
            pressed = false;
            if (onStateChange) onStateChange(false);
            printf("[ButtonTile] Eliberat la (%d,%d)\n", gridX, gridZ);
        }

        void render(const glm::mat4& vpMatrix,
                const glm::vec3& lightPos,
                const glm::vec3& viewPos) override
        {
            if (!active) return;

            glm::mat4 mvp = vpMatrix * modelMatrix;
            applyUniforms(mvp, lightPos, viewPos);

            // Trimitem pozitia in grila catre shader (pentru calculul UV)
            GLint locX = glGetUniformLocation(shader_programme, "tileGridX");
            GLint locZ = glGetUniformLocation(shader_programme, "tileGridZ");
            glUniform1f(locX, (float)gridX);
            glUniform1f(locZ, (float)gridZ);

            // Legam textura la unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureId);
            glUniform1i(glGetUniformLocation(shader_programme, "buttonTex"), 0);

            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void display() override {}

        void update(float deltaTime) override {}

    };
}

#endif //BLOXORZ_SPG_BUTTONTILE_H
