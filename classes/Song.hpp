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
        float color[3];
    };

    private:
        std::vector<Note> _notes;
        std::vector<int> _noteStatuses;
        double _songProgress = 0.0f;

        std::string standardizeNoteName(std::string noteName) {
            if (noteName.substr(0, 2) == "B-") {
                noteName[0] = 'A';
                noteName[1] = '#';
            } else if (noteName.substr(0, 2) == "E-") {
                noteName[0] = 'D';
                noteName[1] = '#';
            }

            return noteName;
        }


    public:
        ~Song() {
            for (size_t i = 0; i < _notes.size(); i++) {
                delete _notes.at(i).model;
            }
        }

        void addNotesFromCsv(const char* fileName, Piano* piano) {
            _noteStatuses.clear();
            for (size_t i = 0; i < piano->getLayout().size(); i++) {
                _noteStatuses.push_back(0);
            }

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
                float noteOffsetZ = 1.6f;
                float brightness = 1.15f;
                if (noteName.find('#') != std::string::npos) {
                    noteWidth = piano->getBlackKeyWidth();
                    noteOffsetZ -= 0.45f;
                    brightness = 0.8f;
                }

                Note newNote;
                Model* noteModel = ModelFactory::fromNote(noteWidth, duration, noteWidth * 0.75f);
                noteModel->setTextureHandle(gTextureHandles::NOTE);
                noteModel->pos[0] = piano->getKeyX(noteName);
                noteModel->pos[1] = startTime + 3.18f;
                noteModel->pos[2] = noteOffsetZ;

                float r = ((float)indexOf(piano->getLayout(), noteName) / (float)piano->getLayout().size());
                float g = 0.2f;
                float b = 1.0f - r;
                newNote.color[0] = r * brightness;
                newNote.color[1] = g * brightness;
                newNote.color[2] = b * brightness;

                newNote.model = noteModel;
                newNote.noteIndex = indexOf(piano->getLayout(), noteName);
                newNote.startTime = startTime;
                newNote.endTime = startTime + duration;

                _notes.push_back(newNote);
            }
            
        }

        void draw() {
            for (size_t i = 0; i < _notes.size(); i++) {
                Note note = _notes.at(i);
                if (note.model->pos[1] > 20.0f || note.model->pos[1] < -10.0f)
                    continue;
                
                /*if (_noteStatuses.at(note.noteIndex) == 1) {
                    note.model->drawNote(note.color[0] * 3.0f, note.color[1] * 3.0f, note.color[2] * 3.0f);
                } else {*/
                    note.model->drawNote(note.color[0], note.color[1], note.color[2]);
                //}
            }

        }

        void update(double deltaTime) {
            _songProgress += (deltaTime / 1000.0) * (124.0 / 60.0);
            if (_songProgress > 202.5f) {
                exit(0);
            }

            std::fill(_noteStatuses.begin(), _noteStatuses.end(), 0);
            for (size_t i = 0; i < _notes.size(); i++) {
                Note note = _notes.at(i);
                note.model->pos[1] = (3.18 + (note.startTime - _songProgress));
                if (note.startTime - _songProgress <= 0.0 && note.endTime > 0.0) {
                   //_noteStatuses[note.noteIndex] = 1;
                }
            }
        }

        std::vector<int> getNoteStatuses() {
            return _noteStatuses;
        }
};

#endif