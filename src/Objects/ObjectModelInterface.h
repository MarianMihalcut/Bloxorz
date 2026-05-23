//
// Created by Marian on 11.04.2026.
//

#ifndef BLOXORZ_SPG_OBJECTMODELINTERFACE_H
#define BLOXORZ_SPG_OBJECTMODELINTERFACE_H
#include <glew.h>
#include <fstream>
#include <string>
#include <GL/gl.h>
#include <glm/gtc/type_ptr.hpp>


namespace ObjectModel {

    class ObjectModelInterface {

    protected:
        /// shadder_programme - incarca shaderele obiectelor
        /// ebo - Element Buffer Object(nu intotdeauna utilizat)
        GLuint shader_programme;
        GLuint vao; ///<vao - Vertex Array Object
        GLuint vbo; ///<vbo - Vertex Buffer Object
        GLuint ebo; ///<ebo - Element Buffer Object(nu intotdeauna utilizat)

        const GLuint PI = glm::pi<float>();

        /// Citeste continutul fragment si vertex shaders.
        /// Apelat in clasele copil.
        std::string textFileRead(std::string fn);

        void printShaderInfoLog(GLuint obj);

        void printProgramInfoLog(GLuint obj);

    public:
        ///<summary>
        /// Constructorul clasei, creat pentru a initializa implicit elementele la instantierea clasei
        ///</summary>
        ObjectModelInterface();

        ~ObjectModelInterface();

        /// Declaram clasele abstracte, care trebuie implementate diferit in clasele copil

        /// Afiseaza obiectul creat
        virtual void display() = 0; //display la obiectul creat

        /// Initializeaza shaderele
        virtual void init() = 0; //initializari shadere

        virtual void update(float deltaTime) = 0;
    };

} // ObjectModel

#endif //BLOXORZ_SPG_OBJECTMODELINTERFACE_H