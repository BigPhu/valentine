#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>

#include "geometry.hpp"

class Renderer {
private:
    bool isFirstRender = true;

    int lengthOfVertexArray = 0;
    Vec3<float>* vertexArray = nullptr;

    int lengthOfTriangleArray = 0;
    int* triangleArray = nullptr;

    bool isNormalInialized = false;
    Vec3<float>* normalArray = nullptr;
    
    char** outputBuffer = nullptr;
    float** zBuffer = nullptr;
    
    int screenWidth = 0;
    int screenHeight = 0;
    
    float horizontalScale = 0.0f;
    float verticalScale = 0.0f;
    
    char background = ' ';
    const char* gradient = nullptr;
    int gradientSize = 0;
    float distanceFromCam = 0.0f;
    
    float thetaX = 0.0f;
    float thetaY = 0.0f;
    float thetaZ = 0.0f;
    
    float angleX = 0.0f;
    float angleY = 0.0f;
    float angleZ = 0.0f;

    Vec3<float> translateDirection = Vec3<float>(0.0f, 0.0f, 0.0f);
    
    Vec3<float> lightDirection = Vec3<float>(0.0f, 0.0f, 0.0f);
    int lightIntensity = 0;
    
    int animationDelay = 0;
    float scanStep = 0.0f;

    void handleFirstRender() {
        if (vertexArray == nullptr) {
            throw std::runtime_error("Unintialized Vertex Array: Try using vertex() before rendering");
        } else if (triangleArray == nullptr) {
            throw std::runtime_error("Unintialized Triangle Array: Try using triangle() before rendering");
        } else if (!isNormalInialized) {
            if (normalArray != nullptr) {
                delete[] normalArray;
            }

            normalArray = new Vec3<float>[lengthOfTriangleArray / 3];
            for (int i = 0; i < lengthOfTriangleArray; i+= 3) {
                Vec3<float> v0 = vertexArray[triangleArray[i]];
                Vec3<float> v1 = vertexArray[triangleArray[i + 1]];
                Vec3<float> v2 = vertexArray[triangleArray[i + 2]];

                normal(v0, v1, v2, i / 3);
            }
        }

        isFirstRender = false;
    }

    void normal(const Vec3<float>& v0, const Vec3<float>& v1, const Vec3<float>& v2, const int& triangleId) {        
        if (normalArray == nullptr) {
            normalArray = new Vec3<float>[lengthOfTriangleArray / 3];
        } 

        normalArray[triangleId] = (v1 - v0) ^ (v2 - v1);
        normalArray[triangleId].normalize();
        isNormalInialized = true;
    }

    void setPixel(int x, int y, char color) {
        if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
            outputBuffer[x][y] = color;
        }
    }

    void renderTriangle(Vec3<float>& v0, Vec3<float>& v1, Vec3<float>& v2, const int& normalId) {
        Triangle<float> triangle(v0, v1, v2);
        Vec3<float> normal(normalArray[normalId]);
        normal.rotate(thetaX, thetaY, thetaZ);

        float minX = std::min(v0.x, std::min(v1.x, v2.x));
        float maxX = std::max(v0.x, std::max(v1.x, v2.x));

        float minY = std::min(v0.y, std::min(v1.y, v2.y));
        float maxY = std::max(v0.y, std::max(v1.y, v2.y));

        for (float x = minX; x < maxX; x += scanStep) {
            for (float y = minY; y < maxY; y += scanStep) {
                Vec3<float> vertex(x, y, triangle.getZFrom(x, y));

                if (triangle.containsVertex(vertex)) {
                    projectVertex(vertex, normal);
                }
            }
        }
    }

    void projectVertex(const Vec3<float>& vertex, const Vec3<float>& normal) {
        float ooz = 1 / (vertex.z + distanceFromCam);
        int xp = (int)((screenWidth / 2) + (horizontalScale * vertex.x * ooz));
        int yp = (int)((screenHeight / 2) - (verticalScale * vertex.y * ooz));

        float L = (normal) * lightDirection;
        if (ooz > zBuffer[xp][yp] && L > 0) {
            zBuffer[xp][yp] = ooz;
            int lightLevel = std::min((float)gradientSize - 1, L * lightIntensity);
            setPixel(xp, yp, gradient[lightLevel]);
        }
    }

public:

    Renderer() {
        screenWidth = 80;
        screenHeight = 22;
        horizontalScale = 50;
        verticalScale = 20;

        outputBuffer = new char*[screenWidth];
        zBuffer = new float*[screenWidth];
        for (int x = 0; x < screenWidth; x++) {
            outputBuffer[x] = new char[screenHeight];
            zBuffer[x] = new float[screenHeight];
        }
        resetBuffers();

        background = ' ';
        gradient = ".,-~:;=!*#$@";
        gradientSize = strlen(gradient);

        animationDelay = 10;
        scanStep = 0.07;

        thetaX = 0.0f;
        thetaY = 0.0f;
        thetaZ = 0.0f;
        angleX = 0.04f;
        angleY = 0.04f;
        angleZ = 0.04f;

        distanceFromCam = 5.0f;
        lightDirection = Vec3<float>(0.0f, 0.0f, -1.0f);
        lightDirection.normalize();
        lightIntensity = 11;
    }

    Renderer(int screenWidth, int screenHeight, float horizontalScale, float verticalScale, 
            const char& background, const char* gradient, const int& animationDelay, const float& scanStep) {
        if (screenWidth <= 0 || screenHeight <= 0) {
            throw std::invalid_argument("Renderer Constructor Error: Invalid screen sizes!");
        } else if (gradient == nullptr) {
            throw std::invalid_argument("Renderer Constructor Error: Invalid gradient!");
        }

        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        this->horizontalScale = horizontalScale;
        this->verticalScale = verticalScale;

        this->background = background;

        this->gradient = gradient;
        this->gradientSize = strlen(gradient);
        
        this->animationDelay = animationDelay;
        this->scanStep = scanStep;

        outputBuffer = new char*[screenWidth];
        zBuffer = new float*[screenWidth];
        for (int x = 0; x < screenWidth; x++) {
            outputBuffer[x] = new char[screenHeight];
            zBuffer[x] = new float[screenHeight];
        }
        resetBuffers();
    }

    ~Renderer() {
        delete[] vertexArray;
        delete[] triangleArray;
        delete[] normalArray;

        for (int x = 0; x < screenWidth; x++) {
            delete[] outputBuffer[x];
            delete[] zBuffer[x];
        }
        delete[] outputBuffer;
        delete[] zBuffer;
    }

    void vertex(const int& lengthOfVertexArray, Vec3<float>* vertexArray) {
        if (lengthOfVertexArray == 0 || vertexArray == nullptr) {
            throw std::invalid_argument("Renderer Vertex Error: Invalid vertex array!");
        }

        if (this->vertexArray) {
            delete[] this->vertexArray;
        }

        this->lengthOfVertexArray = lengthOfVertexArray;
        this->vertexArray = new Vec3<float>[this->lengthOfVertexArray];
        for (int i = 0; i < this->lengthOfVertexArray; i++) {
            this->vertexArray[i] = vertexArray[i];
        }

        isNormalInialized = false;
        isFirstRender = true;
    }
    
    void triangle(const int& lengthOfTriangleArray, int* triangleArray) {
        if (lengthOfTriangleArray == 0 || lengthOfTriangleArray % 3 != 0 || triangleArray == nullptr) {
            throw std::invalid_argument("Renderer Triangle Error: Invalid triangle array!");
        }
        
        if (this->triangleArray != nullptr) {
            delete[] this->triangleArray;
        }
        
        this->lengthOfTriangleArray = lengthOfTriangleArray;
        this->triangleArray = new int[this->lengthOfTriangleArray];
        for (int i = 0; i < this->lengthOfTriangleArray; i++) {
            this->triangleArray[i] = triangleArray[i];
        }

        this->isNormalInialized = false;
        isFirstRender = true;
    }

    void normal(const int& lengthOfNormalArray, Vec3<float>* normalArray ) {
        if (lengthOfNormalArray == 0 || lengthOfNormalArray / lengthOfNormalArray != 0 || normalArray == nullptr) {
            throw std::invalid_argument("Renderer Normal Error: Invalid normal array!");
        }
        
        if (this->triangleArray != nullptr) {
            delete[] this->normalArray;
        }
        
        this->triangleArray = new int[lengthOfNormalArray];
        for (int i = 0; i < lengthOfNormalArray; i++) {
            this->triangleArray[i] = triangleArray[i];
        }

        this->isNormalInialized = true;
        isFirstRender = true;
    }

    void light(float distanceFromCam, const Vec3<float>& lightDirection, int lightIntensity) {
        this->distanceFromCam = distanceFromCam;
        this->lightDirection = lightDirection;
        this->lightDirection.normalize();
        this->lightIntensity = lightIntensity;
    }

    void rotation(float angleX, float angleY, float angleZ) {
        this->angleX = angleX;
        this->angleY = angleY;
        this->angleZ = angleZ;
    }

    void translation(const Vec3<float>& translateDirection) {
        this->translateDirection = translateDirection;
    }

    void render() {
        if (isFirstRender) {
            handleFirstRender();
        }

        for (int i = 0; i < lengthOfTriangleArray; i += 3) {
            Vec3<float> v0 = vertexArray[triangleArray[i]];
            Vec3<float> v1 = vertexArray[triangleArray[i + 1]];
            Vec3<float> v2 = vertexArray[triangleArray[i + 2]];

            v0.rotate(thetaX, thetaY, thetaZ);
            v1.rotate(thetaX, thetaY, thetaZ);
            v2.rotate(thetaX, thetaY, thetaZ);

            v0 += translateDirection;
            v1 += translateDirection;
            v2 += translateDirection;

            renderTriangle(v0, v1, v2, i / 3);
        }

        printBuffer();
        thetaX += angleX;
        thetaY += angleY;
        thetaZ += angleZ;

        std::this_thread::sleep_for(std::chrono::milliseconds(animationDelay));
    }


    void resetBuffers() {
        for (int y = 0; y < screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                outputBuffer[x][y] = background;
                zBuffer[x][y] = 0;
            }
        }
    }

    void printBuffer() {
        std::cout << "\x1b[H";
        for (int y = 0; y < screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                std::cout << outputBuffer[x][y];
            }
            std::cout << '\n';
        }
    }
};
