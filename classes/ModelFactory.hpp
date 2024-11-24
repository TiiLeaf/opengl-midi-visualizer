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
    static Model* createModelWithVertexData(std::vector<float> vertices) {
        Model* newModel = new Model;
        newModel->setVertexData(vertices);
        return newModel;
    }

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

    static void computeNormal(float normal[], float a[], float b[], float c[]) {
        normal[0] = (b[1]-a[1])*(c[2]-a[2])-(b[2]-a[2])*(c[1]-a[1]);
        normal[1] = (b[2]-a[2])*(c[0]-a[0])-(b[0]-a[0])*(c[2]-a[2]);
        normal[2] = (b[0]-a[0])*(c[1]-a[1])-(b[1]-a[1])*(c[2]-a[0]);
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

    static std::vector<float> createCylinderVertexData(size_t res, float bottomR, float topR, float startY, float endY, bool hasBottomCap, float bottomCapOffset, bool hasTopCap, float topCapOffset) {
        std::vector<float> vertexData;

        const float twoPi = 6.2831855f;

        for (size_t i = 0; i < res; i++) {
            float theta = i * twoPi / res;
            float nextTheta = (i+1) * twoPi / res;
            
            //find the corners an imaginary quad between this step and the next step
            float topLeftCorner[3] = { sin(theta) * topR, endY, cos(theta) * topR };
            float topRightCorner[3] = { sin(nextTheta) * topR, endY, cos(nextTheta) * topR };
            float bottomLeftCorner[3] = { sin(theta) * bottomR, startY, cos(theta) * bottomR };
            float bottomRightCorner[3] = { sin(nextTheta) * bottomR, startY, cos(nextTheta) * bottomR };

            //create two triangles using the corners for the outside of the cylinder
            float normal[3];
            computeNormal(normal, topLeftCorner, topRightCorner, bottomLeftCorner);
            addVertexToData(vertexData,
                topLeftCorner, //x,y,z
                0.0f, 1.0f, //u,v
                normal[0], normal[1], normal[2] //normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                normal[0], normal[1], normal[2] //normal
            );
            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                normal[0], normal[1], normal[2] //normal
            );

            addVertexToData(vertexData,
                bottomLeftCorner, //x,y,z
                0.0f, 0.0f, //u,v
                normal[0], normal[1], normal[2] //normal
            );
            addVertexToData(vertexData,
                topRightCorner, //x,y,z
                1.0f, 1.0f, //u,v
                normal[0], normal[1], normal[2] //normal
            );
            addVertexToData(vertexData,
                bottomRightCorner, //x,y,z
                1.0f, 0.0f, //u,v
                normal[0], normal[1], normal[2] //normal
            );

            //put a cap on the top of the cylinder
            if (hasTopCap) {
                float topCenter[] = {0.0f, endY + topCapOffset, 0.0f};
                computeNormal(normal, topLeftCorner, topRightCorner, topCenter);
                addVertexToData(vertexData,
                    topLeftCorner, //x,y,z
                    0.0f, 1.0f, //u,v
                    normal[0], normal[1], normal[2] //normal
                );
                addVertexToData(vertexData,
                    topRightCorner, //x,y,z
                    1.0f, 1.0f, //u,v
                    normal[0], normal[1], normal[2] //normal
                );
                addVertexToData(vertexData,
                    topCenter, //x,y,z
                    0.0f, 0.0f, //u,v
                    0.0f, 1.0f, 0.0f //normal
                );
            }

            //put a cap on the bottom of the cylinder
            if (hasBottomCap) {
                float bottomCenter[] = {0.0f, startY + bottomCapOffset, 0.0f};
                computeNormal(normal, bottomLeftCorner, bottomRightCorner, bottomCenter);
                addVertexToData(vertexData,
                    bottomLeftCorner, //x,y,z
                    0.0f, 1.0f, //u,v
                    normal[0], normal[1], normal[2] //normal
                );
                addVertexToData(vertexData,
                    bottomRightCorner, //x,y,z
                    1.0f, 1.0f, //u,v
                    normal[0], normal[1], normal[2] //normal
                );
                addVertexToData(vertexData,
                    bottomCenter, //x,y,z
                    0.0f, 0.0f, //u,v
                    0.0f, -1.0f, 0.0f //normal
                );
            }
        }

        return vertexData;
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

        return createModelWithVertexData(vertices);
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

        return createModelWithVertexData(cuboidVertexData);
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

        return createModelWithVertexData(cuboidVertexData);
    }

    static Model* fromBlackKey(float w, float h, float d, float o) {
        std::vector<float> vertexData = {
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

        return createModelWithVertexData(vertexData);
    }

    static Model* fromLampPost(float baseRadius, float baseHeight, float postRadius, float postHeight) {
        std::vector<float> vertexData;
        std::vector<float> baseVertexData = createCylinderVertexData(6, baseRadius, baseRadius, 0, baseHeight, false, 0, true, baseHeight * 2);
        std::vector<float> postVertexData = createCylinderVertexData(6, postRadius, postRadius, baseHeight, baseHeight + postHeight, false, 0, true, 0.5f);

        vertexData.insert(vertexData.end(), baseVertexData.begin(), baseVertexData.end());
        vertexData.insert(vertexData.end(), postVertexData.begin(), postVertexData.end());

        return createModelWithVertexData(vertexData);
    }

    static Model* fromLampShade(float radius, float height) {
        std::vector<float> vertexData;
        std::vector<float> bottomVertexData = createCylinderVertexData(14, 0, radius, 0, height, false, 0, false, 0);
        std::vector<float> topVertexData = createCylinderVertexData(14, radius, radius * 1.1f, height, height * 1.5f, false, 0, false, 0);

        vertexData.insert(vertexData.end(), bottomVertexData.begin(), bottomVertexData.end());
        vertexData.insert(vertexData.end(), topVertexData.begin(), topVertexData.end());

        return createModelWithVertexData(vertexData);
    }

    static Model* fromLampBulb(float radius, float height) {
        std::vector<float> vertexData;
        std::vector<float> bottomVertexData = createCylinderVertexData(10, radius, radius, radius, height + radius, true, -radius, false, 0);
        std::vector<float> topVertexData = createCylinderVertexData(10, radius, radius * 0.5f, height + radius, height + radius + (radius * 0.5f), false, 0, true, 0);

        vertexData.insert(vertexData.end(), bottomVertexData.begin(), bottomVertexData.end());
        vertexData.insert(vertexData.end(), topVertexData.begin(), topVertexData.end());

        return createModelWithVertexData(vertexData);
    }

    static Model* fromSkybox() {
        std::vector<float> vertexData = {
             1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
             1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
             1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,

            -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
             1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
             1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
             1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

            -1.0f,  1.0f,  1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -1.0f,  1.0f,  1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,

             1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
             1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
             1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
             1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
             1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
             1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
        };

        return createModelWithVertexData(vertexData);
    }

    static Model* fromFloor(float r) {
        std::vector<float> vertexData;
        
        const float twoPi = 6.2831855f;
        const size_t res = 32;
        for (size_t i = 0; i < res; i++) {
            float theta = i * twoPi / res;
            float nextTheta = (i+1) * twoPi / res;

            float leftCorner[3] = { sin(theta) * r, 0.0f, cos(theta) * r };
            float rightCorner[3] = { sin(nextTheta) * r, 0.0f, cos(nextTheta) * r };
            float center[3] = {0.0f, 0.0f, 0.0f};

            addVertexToData(vertexData,
                leftCorner,
                (sin(theta) * 0.5f) + 0.5f, (cos(theta) * 0.5f) + 0.5f,
                0.1f, 1.0f, 0.0f
            );
            addVertexToData(vertexData,
                rightCorner,
                (sin(nextTheta) * 0.5f) + 0.5f, (cos(nextTheta) * 0.5f) + 0.5f,
                0.1f, 1.0f, 0.0f
            );
            addVertexToData(vertexData,
                center,
                0.5f, 0.5f,
                0.1f, 1.0f, 0.0f
            );
        }

        return createModelWithVertexData(vertexData);
    }
};

#endif