//
// Created by hgallegos on 5/10/2022.
//

#ifndef LEARNOPENGL_OBJETO_H
#define LEARNOPENGL_OBJETO_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "shader_m.h"
using namespace std;
using namespace glm;

class Objeto {
public:
    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec2> textureCoords;
    vector<GLuint> indices;
    GLuint indices_size;
    GLuint vao, vbo_position;
    mat4 model;
    vec3 centro;
    float traslacion;
    float escala = 20;
    vec3 rotacion;

    bool visible=true;
    bool mueve = true;

    GLint POSITION_ATTRIBUTE=0, NORMAL_ATTRIBUTE=1, TEXCOORD0_ATTRIBUTE=8;
    Objeto() {
        traslacion = 0;
    }
    vec3 cal_normal(vec3 v1, vec3 v2, vec3 v3){
        return glm::cross(v2-v1, v3-v2);
    }
    virtual GLuint setup()=0;
    virtual void display(Shader &sh)=0;
};

class Esfera:public Objeto{
public:
    //vec3 centro;
    float radius;
    int slices, stacks;
    Esfera() {
        escala = 0.5;
        centro = vec3(0.0);
    }
    Esfera(vec3 _centro) {
        escala = 0.5;
        centro = _centro;
    }
    Esfera(vec3 _centro, float _radius, int _slices, int _stacks) {
        escala = 0.5;
        centro = _centro;
        radius = _radius;
        slices = _slices;
        stacks = _stacks;
    }
    GLuint setup();

    void display(Shader &sh);
};

class Plano: public Objeto{
public:
    vec3 normal;
    float d;
    Plano(vec3 _normal, float _d){ normal=_normal; d=_d; }
    void display(Shader &sh){}
};

class Piramide: public Objeto{
public:
    float rotacion;
    Piramide() { escala = 1; rotacion = 0; }
    GLuint setup();
    void display(Shader &sh);
};
class Cubo: public Objeto{
public:
    Cubo() { escala = 1; }
    GLuint setup() override;
    void display(Shader &sh) override;
};

class Curva : public Objeto {
public:
    Curva() { escala = 1; }
    GLuint setup() override;
    void display(Shader &sh) override;
};

class Superficie: public Objeto{
public:
    Superficie() { escala = 1; }
    GLuint setup() override;
    void display(Shader &sh) override;
};
#endif //LEARNOPENGL_OBJETO_H
