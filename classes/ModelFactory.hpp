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
};

#endif