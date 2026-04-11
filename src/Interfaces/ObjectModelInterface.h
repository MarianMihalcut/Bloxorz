//
// Created by Marian on 11.04.2026.
//

#ifndef BLOXORZ_SPG_OBJECTMODELINTERFACE_H
#define BLOXORZ_SPG_OBJECTMODELINTERFACE_H

#include <fstream>
#include <string>
#include <GL/gl.h>
#include <glm/gtc/type_ptr.hpp>


namespace ObjectModel {

class ObjectModelInterface {

protected:
    /// shadder_programme - incarca shaderele obiectelor
    /// vao - Vertex Array Object
    /// vbo - Vertex Buffer Object
    /// ebo - Element Buffer Object(nu intotdeauna utilizat)
    GLuint shader_programme, vao, vbo, ebo;

    const GLuint PI = glm::pi<float>();

    /// Citeste continutul fragment si vertex shaders.
    /// Apelat in clasele copil.
    std::string textFileRead(std::string fn) {
        std::ifstream ifile(fn);
        std::string filetext;
        while (ifile.good()) {
            std::string line;
            std::getline(ifile, line);
            filetext.append(line + "\n");
        }
        return filetext;
    }

    void printShaderInfoLog(GLuint obj)
    {
        int infologLength = 0;
        int charsWritten = 0;
        char *infoLog;

        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

        if (infologLength > 0)
        {
            infoLog = (char *)malloc(infologLength);
            glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
            printf("%s\n", infoLog);
            free(infoLog);
        }
    }

    void printProgramInfoLog(GLuint obj)
    {
        int infologLength = 0;
        int charsWritten = 0;
        char *infoLog;

        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

        if (infologLength > 0)
        {
            infoLog = (char *)malloc(infologLength);
            glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
            printf("%s\n", infoLog);
            free(infoLog);
        }
    }
public:
    ///<summary>
    /// Constructorul clasei, creat pentru a initializa implicit elementele la instantierea clasei
    ///</summary>
    ObjectModelInterface() {
        shader_programme = 0;
        vao = 0;
        ebo = 0;
        vbo = 0;
    }

    virtual ~ObjectModelInterface() {
        shader_programme = 0;
        vao = 0;
        ebo = 0;
        vbo = 0;
    }

    /// Declaram clasele abstracte, care trebuie implementate diferit in clasele copil
    virtual void display() = 0; //display la obiectul creat
    virtual void init() = 0; //initializari shadere
    virtual void show_object() = 0; //afisare obiect in interfata
};

} // ObjectModel

#endif //BLOXORZ_SPG_OBJECTMODELINTERFACE_H
