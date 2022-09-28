//----------------------------------------------------------------------------------------
/**
 * \file    main.cpp
 * \author  Josef Bacík
 * \date    2020
 * \brief   responsible for communication with GPU, init objects and shaders and draws objects
 */
 //----------------------------------------------------------------------------------------

#ifndef __RENDER_STUFF_H
#define __RENDER_STUFF_H

#include "load.h"

// defines geometry of object in the scene (space ship, ufo, asteroid, etc.)
// geometry is shared among all instances of the same object type
typedef struct MeshGeometry {
    GLuint vertexBufferObject;   // identifier for the vertex buffer object
    GLuint elementBufferObject;  // identifier for the element buffer object
    GLuint vertexArrayObject;    // identifier for the vertex array object
    unsigned int numTriangles;         // number of triangles in the mesh
    // material
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    GLuint texture;
    GLuint texture2;
} MeshGeometry;

// parameters of individual objects in the scene (e.g. position, size, speed, etc.)
typedef struct Object {
    glm::vec3 position;
    glm::vec3 direction;
    float     speed;
    float     size;

    bool destroyed;

    float startTime;
    float currentTime;

} Object;

typedef struct HeliObject : public Object {
    glm::vec3 initPosition;
    glm::vec3 dirForReflector;
    float speed;
    float startTime;
    float currentTime;
    float viewAngle;
} HeliObject;

typedef struct Fire : public Object {
    int textureFrames;
    float frameDuration;
} Fire;

typedef struct BannerObject : public Object {
    float speed;
    float startTime;
    float currentTime;
} BannerObject;

typedef struct _commonShaderProgram {
    // identifier for the program
    GLuint program;
    // vertex attributes locations
    GLint pos;
    GLint color;
    GLint normal;
    GLint texCoord;
    // uniforms locations
    GLint PVMmatrix;
    GLint Vmatrix;
    GLint Mmatrix;
    GLint normalMatrix;
    GLint time;//elapsed time in seconds

    // material 
    GLint diffuse;
    GLint ambient;
    GLint specular;
    GLint shininess;
    // texture
    GLint useTexture;
    GLint texSampler;
    // reflector related uniforms
    GLint reflectorPosition;
    GLint reflectorDirection;

    GLint sunOn;
    GLint flashOn;
    GLint flashOnShader;
    GLint fogOn;

    GLint pointlightposition;
    GLint pointlightison;
} SCommonShaderProgram;


glm::vec3 checkBounds(const glm::vec3& position, float objectSize = 1.0f);

void drawIsland(Object* island, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawFireplace(Object* fireplace, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawHeli(HeliObject* helicopter, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawPalm(Object* palm, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawFire(Fire* fire, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawCube(Object* cube, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawBanner(BannerObject* banner, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawRock(Object* rock, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawBench(Object* bench, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawSkybox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawWater(Object* water, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void initBannerGeometry(MeshGeometry** geometry);
glm::mat4 alignObject(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up);
void initializeShaders();
void cleanupShaderPrograms();

void changeFog();
void enableFog();
void disableFog();

void initializeModels();
void cleanupModels();

#endif // __RENDER_STUFF_H
