EXE=midiVis

# Main target
all: $(EXE)

#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW -DSDL2 -std=c++17
LIBS=-lmingw32 -lSDL2main -lSDL2 -mwindows -lSDL2_mixer -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r|sed -E 's/(.).*/\1/'|tr 12 21)
CFLG=-O3 -Wall -Wno-deprecated-declarations -DRES=$(RES) -DSDL2 -std=++c17
LIBS=-lSDL2main -lSDL2 -lSDL2_mixer -framework Cocoa -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall -DSDL2 -std=c++17
LIBS=-lSDL2 -lSDL2_mixer -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Dependencies
midiVis.o: midiVis.cpp ./helpers/sdlHelpers.cpp ./helpers/openglHelpers.cpp ./helpers/csvHelpers.cpp ./classes/Camera.hpp ./classes/Model.hpp ./classes/ModelFactory.hpp ./classes/Object.hpp ./classes/Piano.hpp ./classes/Lamp.hpp ./classes/Ground.hpp ./classes/Song.hpp

# Compile rules
.c.o:
	gcc -c $(CFLG)  $<
.cpp.o:
	g++ -c $(CFLG)  $<

#  Link
midiVis:midiVis.o
	g++ $(CFLG) -o $@ $^  $(LIBS)

#  Clean
clean:
	$(CLEAN)
