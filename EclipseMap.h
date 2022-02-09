#ifndef ECLIPSEMAP_H
#define ECLIPSEMAP_H

#include <vector>
#include <GL/glew.h>
#include <iostream>
#include "glm/glm/ext.hpp"
#include "Shader.h"
#include <vector>
#include "glm/glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <jpeglib.h>
#include <GL/glew.h>

#define PI 3.14159265359
using namespace std;


class EclipseMap {
private:
    float heightFactor = 80;
    float textureOffset = 0;
    float orbitDegree = 0;
    glm::vec3 lightPos = glm::vec3(0, 4000, 0);
    bool pKeyPressed = false;
    // DISPLAY SETTINGS
    enum displayFormatOptions {
        windowed = 1, fullScreen = 0
    };
    const char *windowName = "Ceng477 - HW3";
    int defaultScreenWidth = 1000;
    int defaultScreenHeight = 1000;
    int screenWidth = defaultScreenWidth;
    int screenHeight = defaultScreenHeight;
    int displayFormat = displayFormatOptions::windowed;
    // CAMERA SETTINGS
    float projectionAngle = 45;
    float aspectRatio = 1;
    float near = 0.1;
    float far = 10000;
    float startPitch = 180;
    float startYaw = 90;
    float startSpeed = 0;
    float pitch = startPitch;
    float yaw = startYaw;
    float speed = startSpeed;
    glm::vec3 cameraStartPosition = glm::vec3(0, 4000, 4000);
    glm::vec3 cameraStartDirection = glm::vec3(0, -1, -1); //GAZE
    glm::vec3 cameraStartUp = glm::vec3(0, 0, 1);
    glm::vec3 cameraUp = cameraStartUp;
    glm::vec3 cameraPosition = cameraStartPosition;
    glm::vec3 cameraDirection = cameraStartDirection;
public:
    unsigned int textureColor;
    unsigned int textureGrey;
    unsigned int VAO;
    unsigned int VBO, EBO;
    float imageHeight;
    float imageWidth;
    float radius = 600;
    int horizontalSplitCount = 250;
    int verticalSplitCount = 125;

    unsigned int moonTextureColor;
    unsigned int moonVAO;
    unsigned int moonVBO, moonEBO;
    float moonImageHeight;
    float moonImageWidth;
    float moonRadius = 162;

    vector<float> worldVertices;
    vector<unsigned int> worldIndices;

    vector<float> moonVertices;
    vector<unsigned int> moonIndices;

    GLFWwindow *openWindow(const char *windowName, int width, int height);

    void Render(const char *coloredTexturePath, const char *greyTexturePath, const char *moonTexturePath);

    void handleKeyPress(GLFWwindow *window);

    void initColoredTexture(const char *filename, GLuint shader);

    void initGreyTexture(const char *filename, GLuint shader);

    void initMoonColoredTexture(const char *filename, GLuint shader);

    /******* Functions that we write *****/
    void Update_camera(GLuint ID);

    void Update_uniform_variables(GLuint ID);

    void Configure_Buffers_Moon();

    void Configure_Buffers_World();

    pair<vector<float>, vector<unsigned int> > generateSphereVerticesAndIndices(float radius, int horizontalSplitCount, int verticalSplitCount, float startx , float starty ,float startz );

    float angle         = 2 * PI * 0.5  / horizontalSplitCount;  
    float angle_moon    = -0.02 ;
    glm::mat4 I         = glm::mat4(1.0f);
    glm::mat4 ModelMoon = glm::mat4(1.0f);
    glm::mat4 ModelWorld= glm::mat4(1.0f);
    glm::mat4 SelfRot   = glm::mat4(1.0f);
    glm::mat4 rotation  = glm::rotate(I , angle ,glm::vec3(0,0,1));
    glm::mat4 rot_moon  = glm::rotate(I , angle_moon ,glm::vec3(0,0,1));
};

#endif
