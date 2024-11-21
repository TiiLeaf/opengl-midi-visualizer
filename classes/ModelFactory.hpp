#ifndef MODEL_FACTORY_HPP
#define MODEL_FACTORY_HPP

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "../helpers/openGlHelpers.cpp"

#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

class ModelFactory {
private:
    static void readFace(std::vector<std::string> tokens, std::vector<std::array<float, 3>>& v, std::vector<std::array<float, 2>>& vt, std::vector<std::array<float, 3>>& vn, std::vector<float>& vertices) {
        size_t triangleCount = tokens.size() - 3;

        for (size_t i = 0; i < triangleCount; i++) {
            readCorner(tokens[1], v, vt, vn, vertices);
            readCorner(tokens[2 + i], v, vt, vn, vertices);
            readCorner(tokens[3 + i], v, vt, vn, vertices);
        }
    }

    static void readCorner(std::string description, std::vector<std::array<float, 3>>& v, std::vector<std::array<float, 2>>& vt, std::vector<std::array<float, 3>>& vn, std::vector<float>& vertices) {
        std::vector<std::string> v_vt_vn = split(description, "/");

        long posIndex = std::stol(v_vt_vn[0]) - 1;
        vertices.push_back(v.at(posIndex)[0]);
        vertices.push_back(v.at(posIndex)[1]);
        vertices.push_back(v.at(posIndex)[2]);

        long texcoordIndex = std::stol(v_vt_vn[1]) - 1;
        vertices.push_back(vt.at(texcoordIndex)[0]);
        vertices.push_back(vt.at(texcoordIndex)[1]);

        long normalIndex = std::stol(v_vt_vn[2]) - 1;
        vertices.push_back(vn.at(normalIndex)[0]);
        vertices.push_back(vn.at(normalIndex)[1]);
        vertices.push_back(vn.at(normalIndex)[2]);
    }

    static void addVertexToData(std::vector<float>& vertices, float pos[], float u, float v, float nx, float ny, float nz) {
        vertices.push_back(pos[0]);
        vertices.push_back(pos[1]);
        vertices.push_back(pos[2]);
        vertices.push_back(u);
        vertices.push_back(v);
        vertices.push_back(nx);
        vertices.push_back(ny);
        vertices.push_back(nz);
    }

public:
    static Model* fromObj(const char* fileName) {
        std::vector<std::array<float, 3>> v; //vertex positions
        std::vector<std::array<float, 2>> vt; //texcoords
        std::vector<std::array<float, 3>> vn; //vertex normals
        std::vector<float> vertices;

        size_t vertexCount = 0;
        size_t texcoordCount = 0;
        size_t normalCount = 0;
        size_t triangleCount = 0;

        std::ifstream file;
        std::string line;
        std::vector<std::string> tokens;

        //read the file once to reserve space for each vector
        file.open(fileName);
        if (!file.is_open()) {
            gShouldExit = true;
            std::cout << "Could not open " << fileName << "!" << std::endl;
            return nullptr;
        }

        while (std::getline(file, line)) {
            tokens = split(line, " ");

            if (!tokens[0].compare("v")) {
                vertexCount += 1;
            } else if (!tokens[0].compare("vt")) {
                texcoordCount += 1;
            } else if (!tokens[0].compare("vn")) {
                normalCount += 1;
            } else if (!tokens[0].compare("f")) {
                triangleCount += tokens.size() - 3;
            }
        }
        file.close();

        //reserve space for each vector
        v.reserve(vertexCount);
        vt.reserve(texcoordCount);
        vn.reserve(normalCount);
        vertices.reserve(triangleCount * 3 * 8);

        //read throigh the file again and store the actual data in the allocated space
        file.open(fileName);
        while (std::getline(file, line)) {
            tokens = split(line, " ");

            if (!tokens[0].compare("v")) {
                v.push_back({ std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]) });
            } else if (!tokens[0].compare("vt")) {
                vt.push_back({ std::stof(tokens[1]), std::stof(tokens[2]) });
            } else if (!tokens[0].compare("vn")) {
                vn.push_back({ std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]) });
            } else if (!tokens[0].compare("f")) {
                //read face
                readFace(tokens, v, vt, vn, vertices);
            }
        }
        file.close();

        Model* newModel = new Model;
        newModel->setVertexData(vertices);
        return newModel;
    }

    static Model* fromLampShade(float bottomR, float topR, float h) {
        std::vector<float> vertexData;

        const float twoPi = 6.2831855f;
        const size_t res = 16;

        for (size_t i = 0; i < res; i++) {
            //find the corners of this part of the cylinder
            float theta = i * twoPi / res;
            float nextTheta = (i+1) * twoPi / res;

            float topLeftCorner[3] = { sin(theta) * topR, h, cos(theta) * topR };
            float topRightCorner[3] = { sin(nextTheta) * topR, h, cos(nextTheta) * topR };
            float bottomLeftCorner[3] = { sin(theta) * bottomR, 0, cos(theta) * bottomR };
            float bottomRightCorner[3] = { sin(nextTheta) * bottomR, 0, cos(nextTheta) * bottomR };

            //create two triangles using the corners for the outside of the cylinder
            addVertexToData(vertexData,
                topLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );

            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomRightCorner, //x,y,z
                1.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
        }

        const float newH = h * 1.5;
        const float newR = topR * 1.1;
        for (size_t i = 0; i < res; i++) {
            //find the corners of this part of the cylinder
            float theta = i * twoPi / res;
            float nextTheta = (i+1) * twoPi / res;

            float topLeftCorner[3] = { sin(theta) * newR, newH, cos(theta) * newR };
            float topRightCorner[3] = { sin(nextTheta) * newR, newH, cos(nextTheta) * newR };
            float bottomLeftCorner[3] = { sin(theta) * topR, h, cos(theta) * topR };
            float bottomRightCorner[3] = { sin(nextTheta) * topR, h, cos(nextTheta) * topR };

            //create two triangles using the corners for the outside of the cylinder
            addVertexToData(vertexData,
                topLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );

            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomRightCorner, //x,y,z
                1.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
        }

        Model* newModel = new Model;
        newModel->setVertexData(vertexData);
        return newModel;
    }

    static Model* fromLampPost(float r, float h) {
        std::vector<float> vertexData;

        const float twoPi = 6.2831855f;
        const size_t res = 6;
        const float baseHeight = 0.25f;
        const float poleRadius = r * 0.1;

        //float bottomCenter[] = {0.0f, 0.0f, 0.0f};
        float topCenter[] = {0.0f, baseHeight * 2.0f, 0.0f};

        //construct cylinder for the base of the lamp
        for (size_t i = 0; i < res; i++) {
            //find the corners of this part of the cylinder
            float theta = i * twoPi / res;
            float nextTheta = (i+1) * twoPi / res;

            float topLeftCorner[3] = { sin(theta) * r, baseHeight, cos(theta) * r };
            float topRightCorner[3] = { sin(nextTheta) * r, baseHeight, cos(nextTheta) * r };
            float bottomLeftCorner[3] = { sin(theta) * r, 0, cos(theta) * r };
            float bottomRightCorner[3] = { sin(nextTheta) * r, 0, cos(nextTheta) * r };

            //create two triangles using the corners for the outside of the cylinder
            addVertexToData(vertexData,
                topLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );

            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomRightCorner, //x,y,z
                1.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );

            //put a cap on the top of the cylinder
            addVertexToData(vertexData,
                topLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
            addVertexToData(vertexData,
                topCenter, //x,y,z
                0.0f, 0.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );

            /*put a cap on the bottom of the cylinder
            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
            addVertexToData(vertexData,
                bottomRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
            addVertexToData(vertexData,
                bottomCenter, //x,y,z
                0.0f, 0.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
            */
        }

        //construct a cylinder for the pole of the lamp
        topCenter[1] += h;
        for (size_t i = 0; i < res; i++) {
            //find the corners of this part of the cylinder
            float theta = i * twoPi / res;
            float nextTheta = (i+1) * twoPi / res;

            float topLeftCorner[3] = { sin(theta) * poleRadius, h, cos(theta) * poleRadius };
            float topRightCorner[3] = { sin(nextTheta) * poleRadius, h, cos(nextTheta) * poleRadius };
            float bottomLeftCorner[3] = { sin(theta) * poleRadius, baseHeight, cos(theta) * poleRadius };
            float bottomRightCorner[3] = { sin(nextTheta) * poleRadius, baseHeight, cos(nextTheta) * poleRadius };

            //create two triangles using the corners for the outside of the cylinder
            addVertexToData(vertexData,
                topLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );

            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomRightCorner, //x,y,z
                1.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            
            //put a cap on the top of the cylinder
            addVertexToData(vertexData,
                topLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
            addVertexToData(vertexData,
                topCenter, //x,y,z
                0.0f, 0.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
        }
        
        Model* newModel = new Model;
        newModel->setVertexData(vertexData);
        return newModel;
    }

    static Model* fromCenteredCuboid(float w, float h, float d) {
        float sx = w / 2;
        float sy = h / 2;
        float sz = d / 2;

        std::vector<float> cuboidVertexData = {
             sx,  sy, -sz, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
             sx, -sy, -sz, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            -sx, -sy, -sz, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            -sx, -sy, -sz, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            -sx,  sy, -sz, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
             sx,  sy, -sz, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,

            -sx, -sy,  sz, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
             sx, -sy,  sz, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
             sx,  sy,  sz, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
             sx,  sy,  sz, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            -sx,  sy,  sz, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            -sx, -sy,  sz, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

            -sx,  sy,  sz, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -sx,  sy, -sz, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -sx, -sy, -sz, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -sx, -sy, -sz, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -sx, -sy,  sz, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -sx,  sy,  sz, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,

             sx, -sy, -sz, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
             sx,  sy, -sz, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
             sx,  sy,  sz, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
             sx,  sy,  sz, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
             sx, -sy,  sz, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
             sx, -sy, -sz, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,

            -sx, -sy, -sz, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
             sx, -sy, -sz, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
             sx, -sy,  sz, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
             sx, -sy,  sz, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            -sx, -sy,  sz, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            -sx, -sy, -sz, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,

             sx,  sy,  sz, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
             sx,  sy, -sz, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            -sx,  sy, -sz, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            -sx,  sy, -sz, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            -sx,  sy,  sz, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
             sx,  sy,  sz, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f
        };

        Model* newModel = new Model;
        newModel->setVertexData(cuboidVertexData);
        return newModel;
    }

    static Model* fromAnchoredCuboid(float w, float h, float d) {
        std::vector<float> cuboidVertexData = {
            w, h, 0, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            w, 0, 0, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            0, 0, 0, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            0, 0, 0, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            0, h, 0, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            w, h, 0, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,

            0, 0, d, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
            w, 0, d, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
            w, h, d, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            w, h, d, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            0, h, d, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            0, 0, d, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

            0, h, d, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            0, h, 0, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            0, 0, 0, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            0, 0, 0, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            0, 0, d, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            0, h, d, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,

            w, 0, 0, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            w, h, 0, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            w, h, d, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            w, h, d, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            w, 0, d, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            w, 0, 0, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,

            0, 0, 0, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            w, 0, 0, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            w, 0, d, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            w, 0, d, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            0, 0, d, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            0, 0, 0, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,


            w, h, d, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
            w, h, 0, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            0, h, 0, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            0, h, 0, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            0, h, d, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
            w, h, d, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f
        };

        Model* newModel = new Model;
        newModel->setVertexData(cuboidVertexData);
        return newModel;
    }

    static Model* fromBlackKey(float w, float h, float d, float o) {
        std::vector<float> cuboidVertexData = {
            w, h, 0, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            w, 0, 0, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            0, 0, 0, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            0, 0, 0, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            0, h, 0, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            w, h, 0, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,

            0, 0, d, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
            w, 0, d, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
            w, h, d - o, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            w, h, d - o, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            0, h, d - o, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            0, 0, d, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

            0, h, d - o, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            0, h, 0, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            0, 0, 0, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            0, 0, 0, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            0, 0, d, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            0, h, d - o, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,

            w, 0, 0, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            w, h, 0, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            w, h, d - o, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            w, h, d - o, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            w, 0, d, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            w, 0, 0, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,

            0, 0, 0, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            w, 0, 0, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            w, 0, d, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            w, 0, d, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            0, 0, d, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            0, 0, 0, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,

            w, h, d - o, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
            w, h, 0, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            0, h, 0, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            0, h, 0, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            0, h, d - o, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
            w, h, d - o, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f
        };

        Model* newModel = new Model;
        newModel->setVertexData(cuboidVertexData);
        return newModel;
    }

    static Model* fromLampBulb(float r, float h) {
        std::vector<float> vertexData;

        const float twoPi = 6.2831855f;
        const size_t res = 6;
        float bottomCenter[] = {0.0f, 0.0f, 0.0f};
        float topCenter[] = {0.0f, (r * 0.5f) + r + h, 0.0f};

        for (size_t i = 0; i < res; i++) {
            //find the corners of this part of the cylinder
            float theta = i * twoPi / res;
            float nextTheta = (i+1) * twoPi / res;

            float topLeftCorner[3] = { sin(theta) * r, r + h, cos(theta) * r };
            float topRightCorner[3] = { sin(nextTheta) * r, r + h, cos(nextTheta) * r };
            float bottomLeftCorner[3] = { sin(theta) * r, r, cos(theta) * r };
            float bottomRightCorner[3] = { sin(nextTheta) * r, r, cos(nextTheta) * r };

            //create two triangles using the corners for the outside of the cylinder
            addVertexToData(vertexData,
                topLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );

            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomRightCorner, //x,y,z
                1.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );

            //put a cap on the bottom of the cylinder
            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
            addVertexToData(vertexData,
                bottomRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
            addVertexToData(vertexData,
                bottomCenter, //x,y,z
                0.0f, 0.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
        }
        
        for (size_t i = 0; i < res; i++) {
            //find the corners of this part of the cylinder
            float theta = i * twoPi / res;
            float nextTheta = (i+1) * twoPi / res;

            float topLeftCorner[3] = { sin(theta) * r * 0.5f, (r * 0.5f) + r + h, cos(theta) * r * 0.5f};
            float topRightCorner[3] = { sin(nextTheta) * r * 0.5f, (r * 0.5f) + r + h, cos(nextTheta) * r * 0.5f};
            float bottomLeftCorner[3] = { sin(theta) * r, r + h, cos(theta) * r };
            float bottomRightCorner[3] = { sin(nextTheta) * r, r + h, cos(nextTheta) * r };

            //create two triangles using the corners for the outside of the cylinder
            addVertexToData(vertexData,
                topLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );

            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );
            addVertexToData(vertexData,
                bottomRightCorner, //x,y,z
                1.0f, 0.0f, //u,v
                1.0f, 0.0f, 0.0f //todo normal
            );

            //put a cap on the top of the cylinder
            addVertexToData(vertexData,
                topLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
            addVertexToData(vertexData,
                topCenter, //x,y,z
                0.0f, 0.0f, //u,v
                0.0f, 0.0f, -1.0f //normal
            );
        }

     
        Model* newModel = new Model;
        newModel->setVertexData(vertexData);
        return newModel;
    }
};

#endif