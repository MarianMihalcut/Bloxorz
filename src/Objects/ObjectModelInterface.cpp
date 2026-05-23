//
// Created by Marian on 11.04.2026.
//

#include "ObjectModelInterface.h"
#include <cstdio>

namespace ObjectModel {

    std::string ObjectModelInterface::textFileRead(std::string fn) {
        std::ifstream ifile(fn);
        std::string filetext;
        while (ifile.good()) {
            std::string line;
            std::getline(ifile, line);
            filetext.append(line + "\n");
        }
        return filetext;
    }

    void ObjectModelInterface::printShaderInfoLog(GLuint obj) {
        int infologLength = 0;
        int charsWritten = 0;
        char *infoLog;

        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

        if (infologLength > 0) {
            infoLog = (char *)malloc(infologLength);
            glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
            printf("%s\n", infoLog);
            free(infoLog);
        }
    }

    void ObjectModelInterface::printProgramInfoLog(GLuint obj) {
        int infologLength = 0;
        int charsWritten = 0;
        char *infoLog;

        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

        if (infologLength > 0) {
            infoLog = (char *) malloc(infologLength);
            glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
            printf("%s\n", infoLog);
            free(infoLog);
        }
    }

    ObjectModelInterface::ObjectModelInterface() {
        shader_programme = 0;
        vao = 0;
        ebo = 0;
        vbo = 0;
    }

    ObjectModelInterface::~ObjectModelInterface() {
        shader_programme = 0;
        vao = 0;
        ebo = 0;
        vbo = 0;
    }

} // ObjectModel