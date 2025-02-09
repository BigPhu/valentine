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
    // --- Constructor initialized fields ---

    bool isFirstRender = true;
    
    char* outputBuffer;
    float* zBuffer;
    
    unsigned int screenWidth;
    unsigned int screenHeight;
    unsigned int screenArea;
    
    float horizontalScale;
    float verticalScale;
    
    char background;
    const char* gradient;
    unsigned int gradientSize;
    
    unsigned int animationDelay;
    float scanStep;
    
    // --- Function modifiable fields ---

    unsigned int lengthOfVertexArray = 0;
    Vec3<float>* vertexArray;

    unsigned int lengthOfTriangleArray = 0;
    int* triangleArray;

    unsigned int lengthOfNormalArray = 0;
    Vec3<float>* normalArray;

    float distanceFromCam = 5.0f;
    
    float thetaX = 0.0f;
    float thetaY = 0.0f;
    float thetaZ = 0.0f;
    
    float angleX = 0.0f;
    float angleY = 0.0f;
    float angleZ = 0.0f;

    Vec3<float> translateDirection = Vec3<float>(0.0f, 0.0f, 0.0f);
    
    Vec3<float> lightDirection = Vec3<float>(0.0f, 0.0f, -1.0f);
    unsigned int lightIntensity = 10;

public:
    Renderer(const unsigned int& screenWidth, const unsigned int& screenHeight, 
             const float& horizontalScale, const float& verticalScale, 
             const char& background, const char* gradient, 
             const unsigned int& animationDelay, const float& scanStep) 
        : screenWidth(screenWidth), screenHeight(screenHeight), horizontalScale(horizontalScale), 
          verticalScale(verticalScale), background(background), gradient(gradient), 
          animationDelay(animationDelay), scanStep(scanStep),
          vertexArray(nullptr), triangleArray(nullptr), normalArray(nullptr),
          outputBuffer(nullptr), zBuffer(nullptr) {

        if (screenWidth == 0 || screenHeight == 0) {
           throw std::invalid_argument("Renderer Constructor Error: Invalid screen sizes!");
        } 
        if (gradient == nullptr) {
           throw std::invalid_argument("Renderer Constructor Error: Invalid gradient!");
        }

        this->screenArea = screenWidth * screenHeight;
        this->outputBuffer = new char[this->screenArea];
        this->zBuffer = new float[this->screenArea];

        this->gradientSize = std::strlen(gradient);

        resetBuffers();
    }

    ~Renderer() {
        delete[] this->vertexArray;
        delete[] this->triangleArray;
        delete[] this->normalArray;

        delete[] this->outputBuffer;
        delete[] this->zBuffer;

        vertexArray = nullptr;
        triangleArray = nullptr;
        normalArray = nullptr;
        outputBuffer = nullptr;
        zBuffer = nullptr;
    }

    void vertex(const unsigned int& lengthOfVertexArray, Vec3<float>* vertexArray) {
        if (lengthOfVertexArray == 0 || vertexArray == nullptr) {
            throw std::invalid_argument("Renderer Vertex Error: Invalid vertex array!");
        }

        if (this->vertexArray != nullptr) {
            delete[] this->vertexArray;
        }

        this->lengthOfVertexArray = lengthOfVertexArray;
        this->vertexArray = new Vec3<float>[lengthOfVertexArray];
        for (int i = 0; i < lengthOfVertexArray; i++) {
            this->vertexArray[i] = vertexArray[i];
        }

        this->isFirstRender = true;
    }
    
    void triangle(const unsigned int& lengthOfTriangleArray, int* triangleArray) {
        if (lengthOfTriangleArray == 0 || lengthOfTriangleArray % 3 != 0 || triangleArray == nullptr) {
            throw std::invalid_argument("Renderer Triangle Error: Invalid triangle array!");
        }
        
        if (this->triangleArray != nullptr) {
            delete[] this->triangleArray;
        }
        
        this->lengthOfTriangleArray = lengthOfTriangleArray;
        this->triangleArray = new int[lengthOfTriangleArray];
        for (int i = 0; i < lengthOfTriangleArray; i++) {
            this->triangleArray[i] = triangleArray[i];
        }

        this->isFirstRender = true;
    }

    void normal(const unsigned int& lengthOfNormalArray, Vec3<float>* normalArray ) {
        if (lengthOfNormalArray == 0 || this->lengthOfTriangleArray / lengthOfNormalArray != 3 || normalArray == nullptr) {
            throw std::invalid_argument("Renderer Normal Error: Invalid normal array!");
        }
        
        if (this->normalArray != nullptr) {
            delete[] this->normalArray;
        }
        
        this->lengthOfNormalArray = lengthOfNormalArray;
        this->normalArray = new Vec3<float>[lengthOfNormalArray];
        for (int i = 0; i < lengthOfNormalArray; i++) {
            this->normalArray[i] = normalArray[i];
        }

        this->isFirstRender = true;
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
        if (this->isFirstRender) {
            handleFirstRender();
        }

        for (int i = 0; i < this->lengthOfTriangleArray; i += 3) {
            Vec3<float> v0 = this->vertexArray[this->triangleArray[i]];
            Vec3<float> v1 = this->vertexArray[this->triangleArray[i + 1]];
            Vec3<float> v2 = this->vertexArray[this->triangleArray[i + 2]];

            v0.rotate(this->thetaX, this->thetaY, this->thetaZ);
            v1.rotate(this->thetaX, this->thetaY, this->thetaZ);
            v2.rotate(this->thetaX, this->thetaY, this->thetaZ);

            v0 += this->translateDirection;
            v1 += this->translateDirection;
            v2 += this->translateDirection;

            renderTriangle(v0, v1, v2, i / 3);
        }

        printBuffer();
        this->thetaX += this->angleX;
        this->thetaY += this->angleY;
        this->thetaZ += this->angleZ;

        std::this_thread::sleep_for(std::chrono::milliseconds(this->animationDelay));
    }

    void resetBuffers() {
        for (int y = 0; y < this->screenHeight; y++) {
            for (int x = 0; x < this->screenWidth; x++) {
                int index = x + y * this->screenWidth;
                this->outputBuffer[index] = this->background;
                this->zBuffer[index] = 0;
            }
        }
    }

    void printBuffer() {
        std::string output;
        output.reserve(this->screenArea + this->screenHeight);
        
        for (int y = 0; y < this->screenHeight; y++) {
            for (int x = 0; x < this->screenWidth; x++) {
                output += this->outputBuffer[x + y * this->screenWidth];
            }
            output += '\n';
        }

        std::cout << "\x1b[H" << output;
    }

private:
    void handleFirstRender() {
        if (this->vertexArray == nullptr) {
            throw std::runtime_error("Unintialized Vertex Array: Try using vertex() before rendering");
        } else if (this->triangleArray == nullptr) {
            throw std::runtime_error("Unintialized Triangle Array: Try using triangle() before rendering");
        } else if (this->normalArray == nullptr) {
            normalArray = new Vec3<float>[this->lengthOfTriangleArray / 3];

            for (int i = 0; i < this->lengthOfTriangleArray; i+= 3) {
                Vec3<float> v0 = vertexArray[this->triangleArray[i]];
                Vec3<float> v1 = vertexArray[this->triangleArray[i + 1]];
                Vec3<float> v2 = vertexArray[this->triangleArray[i + 2]];

                normal(v0, v1, v2, i / 3);
            }
        }

        this->isFirstRender = false;
    }

    void normal(const Vec3<float>& v0, const Vec3<float>& v1, const Vec3<float>& v2, const int& triangleId) {        
        if (this->normalArray == nullptr) {
            this->normalArray = new Vec3<float>[this->lengthOfTriangleArray / 3];
        } 

        this->normalArray[triangleId] = (v1 - v0) ^ (v2 - v1);
        this->normalArray[triangleId].normalize();
    }

    void setPixel(int x, int y, char color) {
        if (x >= 0 && x < this->screenWidth && y >= 0 && y < this->screenHeight) {
            this->outputBuffer[x + y * this->screenWidth] = color;
        }
    }

    void renderTriangle(Vec3<float>& v0, Vec3<float>& v1, Vec3<float>& v2, const int& normalId) {
        Triangle<float> triangle(v0, v1, v2);
        Vec3<float> normal(this->normalArray[normalId]);
        normal.rotate(this->thetaX, this->thetaY, this->thetaZ);

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
        float ooz = 1 / (vertex.z + this->distanceFromCam);
        int xp = (int)((this->screenWidth / 2) + (this->horizontalScale * vertex.x * ooz));
        int yp = (int)((this->screenHeight / 2) - (this->verticalScale * vertex.y * ooz));
        int index = xp + yp * this->screenWidth;

        float L = (normal) * this->lightDirection;
        if (ooz > this->zBuffer[index] && L > 0) {
            this->zBuffer[index] = ooz;
            int lightLevel = std::min((float)this->gradientSize - 1, L * this->lightIntensity);
            setPixel(xp, yp, this->gradient[lightLevel]);
        }
    }
};
