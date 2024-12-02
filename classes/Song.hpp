#ifndef SONG_HPP
#define SONG_HPP

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "../helpers/openGlHelpers.cpp"
#include "../helpers/globals.h"
#include "./Model.hpp"
#include "./Piano.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Song {
    struct Note {
        Model* model;
        int noteIndex;
        double startTime;
        double endTime;
    };

    private:
        std::vector<Note> _notes;

        std::string standardizeNoteName(std::string noteName) {
            if (noteName.substr(0, 2) == "B-") {
                noteName[0] = 'A';
                noteName[1] = '#';
                //noteName[2] = (std::stoi(std::string(1, noteName[2])) + 1) + '0'; //wacky cast to char without using ascii
            } else if (noteName.substr(0, 2) == "E-") {
                noteName[0] = 'D';
                noteName[1] = '#';
                //noteName[2] = (std::stoi(std::string(1, noteName[2])) + 1) + '0'; //wacky cast to char without using ascii
            }

            return noteName;
        }

    public:
        void addNotesFromCsv(const char* fileName, Piano* piano) {
            std::ifstream file;

            file.open(fileName);
            if (!file.is_open()) {
                gShouldExit = true; //todo: this is no longer a safe exit condition, since this method is called after `buildScene`, new condition pls
                std::cout << "Could not open " << fileName << "!" << std::endl;
                return;
            }

            std::string line;
            std::vector<std::string> tokens;
            std::getline(file, line);
            while (std::getline(file, line)) {
                tokens = split(line, ",");
                std::string noteName = standardizeNoteName(tokens.at(1));
                std::string strStartTime = tokens.at(2);
                std::string strDuration = tokens.at(3);
                double startTime = std::stod(strStartTime);
                double duration;

                std::vector<std::string> durationDescription = split(strDuration, "/");
                if (durationDescription.size() > 1) {
                    duration = std::stod(durationDescription.at(0)) / std::stod(durationDescription.at(1));
                } else {
                    duration = std::stod(strDuration);
                }

                float noteWidth = piano->getWhiteKeyWidth();
                float noteOffsetZ = 0.0f;
                if (noteName.find('#') != std::string::npos) {
                    noteWidth = piano->getBlackKeyWidth();
                    noteOffsetZ -= 0.15f;
                }

                Note newNote;
                Model* noteModel = ModelFactory::fromAnchoredCuboid(noteWidth, duration, noteWidth);
                noteModel->pos[0] = piano->getKeyX(noteName);
                noteModel->pos[1] = startTime + 3.18f; //todo: BAD HARDCODE, GET FROM PIANO->
                noteModel->pos[2] = noteOffsetZ;

                newNote.model = noteModel;
                newNote.noteIndex = indexOf(piano->getLayout(), noteName); //todo: 2n instead of n for no reason, can use previously calculated index isntead
                newNote.startTime = startTime;
                newNote.endTime = startTime + duration;

                _notes.push_back(newNote);
            }
            
        }
};

#endif