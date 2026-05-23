//
// Created by Marian on 14.04.2026.
//

#include "../Headers/ButtonTile.h"
#include <cstdio>

namespace ObjectModel {
    void ButtonTile::loadTexture() {
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

    ButtonTile::ButtonTile(int gx, int gz, ButtonMode bMode,
                           std::function<void(bool)> callback,
                           const std::string& texPath,
                           const std::string& vertPath,
                           const std::string& fragPath)
        : Tile(gx, gz, 1.0f, 0.2f, 1.0f, COLOR_BODY, vertPath, fragPath)
        , pressed(false)
        , mode(bMode)
        , onStateChange(std::move(callback))
        , textureId(0)
        , texturePath(texPath) {}

    ButtonTile::~ButtonTile() {
        if (textureId)
            glDeleteTextures(1, &textureId);
    }

    TileType ButtonTile::getType() const { return TileType::BUTTON; }
    bool ButtonTile::isPressed() const { return pressed; }
    ButtonTile::ButtonMode ButtonTile::getMode() const { return mode; }

    void ButtonTile::init() {
        loadShaders(vertShaderPath, fragShaderPath);

        // Corpul tile-ului
        std::vector<TileVertex> verts;
        std::vector<GLuint>     idxs;
        buildBox(tileWidth, tileHeight, tileDepth, baseColor, verts, idxs);
        setupMesh(verts, idxs);

        loadTexture();
    }

    void ButtonTile::press() {
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

    void ButtonTile::release() {
        if (!pressed) return;
        pressed = false;
        if (onStateChange) onStateChange(false);
        printf("[ButtonTile] Eliberat la (%d,%d)\n", gridX, gridZ);
    }

    void ButtonTile::render(const glm::mat4& vpMatrix,
                            const glm::vec3& lightPos,
                            const glm::vec3& viewPos) {
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

    void ButtonTile::display() {}
    void ButtonTile::update(float) {}
}