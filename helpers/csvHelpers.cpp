#ifndef CSV_HELPERS_HPP
#define CSV_HELPERS_HPP
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "./openGlHelpers.hpp"
#include "../classes/model.hpp"
int num = 0;
float noteNameToXPos(std::string noteName, std::vector<std::string> pianoLayout, std::vector<Model>& keys) {
    if (noteName.substr(0, 2) == "B-") {
        noteName[0] = 'A';
        noteName[1] = '#';
        //noteName[2] = (std::stoi(std::string(1, noteName[2])) + 1) + '0'; //wacky cast to char without using ascii
    } else if (noteName.substr(0, 2) == "E-") {
        noteName[0] = 'D';
        noteName[1] = '#';
        //noteName[2] = (std::stoi(std::string(1, noteName[2])) + 1) + '0'; //wacky cast to char without using ascii
    }
    int keyIndex = -1;
    for (size_t i = 0; i < pianoLayout.size(); i++) {
        if (pianoLayout.at(i) == noteName) {
            keyIndex = i;
            break;
        }
    }
    num += 1;
    if (num <= 9) {
        std::cout << noteName << std::endl;
    }
    return keys[keyIndex].pos[0];
}
std::vector<Model> readNotesCsv(const char* fileName, std::vector<std::string> pianoLayout, std::vector<Model>& keys) {
    std::ifstream file;
    std::string line;
    std::vector<std::string> tokens;
    std::vector<Model> notes;
    //read the file once to reserve space for each vector
    file.open(fileName);
    if (!file.is_open()) {
        gShouldExit = true;
        std::cout << "Could not open " << fileName << "!" << std::endl;
        return notes;
    }
    std::getline(file, line);
    while (std::getline(file, line)) {
        tokens = split(line, ",");
        std::string noteName = tokens.at(1);
        std::string startTimeToken = tokens.at(2);
        std::string durationToken = tokens.at(3);
        double startTime = std::stod(startTimeToken);
        double duration;
        std::vector<std::string> durationDescription = split(durationToken, "/");
        if (durationDescription.size() > 1) {
            duration = std::stod(durationDescription.at(0)) / std::stod(durationDescription.at(1));
        } else {
            duration = std::stod(durationToken);
        }
        Model newNote;
        newNote.setVertexData(createRectangularPrisimData(0.07, duration, 0.07));
        newNote.setTextureHandle(1);
        newNote.pos[0] = noteNameToXPos(noteName, pianoLayout, keys);
        newNote.pos[1] = startTime + (duration * 0.5f) + 3.5f;
        newNote.pos[2] = 2.2f;
        notes.push_back(newNote);
    }
    file.close();
    return notes;
}
#endif