#ifndef OPENGL_HELPERS_HPP
#define OPENGL_HELPERS_HPP

#include <GL/glew.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "./globals.h"

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>

/*
    Update the perspective projection to handle aspect ratio changes
*/
void setProjection(float aspectRatio) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(66, aspectRatio, 0.1, 30);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*
    Split a string into a vector based on a delimiter, used to parse .obj files
*/
std::vector<std::string> split(std::string line, std::string delimiter) {
    std::vector<std::string> result;

    size_t pos = 0;
    std::string token;
    while ((pos = line.find(delimiter)) != std::string::npos) {
        token = line.substr(0, pos);
        result.push_back(token);
        line.erase(0, pos + delimiter.length());
    }
    result.push_back(line);

    return result;
}

/*
    Add the vertex data for a specific corner of a face to the verticies vector
*/
void readCorner(std::string description, std::vector<std::array<float, 3>>& v, std::vector<std::array<float, 2>>& vt, std::vector<std::array<float, 3>>& vn, std::vector<float>& vertices) {
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

/*
    Adds the vertex data for all corners of a triangle or quad using readCorner
*/
void readFace(std::vector<std::string> tokens, std::vector<std::array<float, 3>>& v, std::vector<std::array<float, 2>>& vt, std::vector<std::array<float, 3>>& vn, std::vector<float>& vertices) {
    size_t triangleCount = tokens.size() - 3;

    for (size_t i = 0; i < triangleCount; i++) {
        readCorner(tokens[1], v, vt, vn, vertices);
        readCorner(tokens[2 + i], v, vt, vn, vertices);
        readCorner(tokens[3 + i], v, vt, vn, vertices);
    }
}

/*
    Reads a .obj file and returns a vector of vertex data
    x, y, z, u, v, nx, ny, nx, ...
*/
std::vector<float> loadObjFile(const char* fileName) {
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
        return vertices;
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
            v.push_back({std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])});
        } else if (!tokens[0].compare("vt")) {
            vt.push_back({std::stof(tokens[1]), std::stof(tokens[2])});
        } else if (!tokens[0].compare("vn")) {
            vn.push_back({std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])});
        } else if (!tokens[0].compare("f")) {
            //read face
            readFace(tokens, v, vt, vn, vertices);
        }
    }
    file.close();

    return vertices;
}

//
//  Check for OpenGL errors and print to stderr
//
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

//
//  Print message to stderr and exit
//
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

//
//  Reverse n bytes
//
static void Reverse(void* x,const int n)
{
   char* ch = (char*)x;
   for (int k=0;k<n/2;k++)
   {
      char tmp = ch[k];
      ch[k] = ch[n-1-k];
      ch[n-1-k] = tmp;
   }
}

/*
    Load texture from bitmap file and return a handle to it.
*/
unsigned int loadBmpFile(const char* file) {
   //  Open file
   FILE* f = fopen(file,"rb");
   if (!f) Fatal("Cannot open file %s\n",file);
   //  Check image magic
   unsigned short magic;
   if (fread(&magic,2,1,f)!=1) Fatal("Cannot read magic from %s\n",file);
   if (magic!=0x4D42 && magic!=0x424D) Fatal("Image magic not BMP in %s\n",file);
   //  Read header
   unsigned int dx,dy,off,k; // Image dimensions, offset and compression
   unsigned short nbp,bpp;   // Planes and bits per pixel
   if (fseek(f,8,SEEK_CUR) || fread(&off,4,1,f)!=1 ||
       fseek(f,4,SEEK_CUR) || fread(&dx,4,1,f)!=1 || fread(&dy,4,1,f)!=1 ||
       fread(&nbp,2,1,f)!=1 || fread(&bpp,2,1,f)!=1 || fread(&k,4,1,f)!=1)
     Fatal("Cannot read header from %s\n",file);
   //  Reverse bytes on big endian hardware (detected by backwards magic)
   if (magic==0x424D)
   {
      Reverse(&off,4);
      Reverse(&dx,4);
      Reverse(&dy,4);
      Reverse(&nbp,2);
      Reverse(&bpp,2);
      Reverse(&k,4);
   }
   //  Check image parameters
   unsigned int max;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,(int*)&max);
   if (dx<1 || dx>max) Fatal("%s image width %d out of range 1-%d\n",file,dx,max);
   if (dy<1 || dy>max) Fatal("%s image height %d out of range 1-%d\n",file,dy,max);
   if (nbp!=1)  Fatal("%s bit planes is not 1: %d\n",file,nbp);
   if (bpp!=24) Fatal("%s bits per pixel is not 24: %d\n",file,bpp);
   if (k!=0)    Fatal("%s compressed files not supported\n",file);
#ifndef GL_VERSION_2_0
   //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
   for (k=1;k<dx;k*=2);
   if (k!=dx) Fatal("%s image width not a power of two: %d\n",file,dx);
   for (k=1;k<dy;k*=2);
   if (k!=dy) Fatal("%s image height not a power of two: %d\n",file,dy);
#endif

   //  Allocate image memory
   unsigned int size = 3*dx*dy;
   unsigned char* image = (unsigned char*) malloc(size);
   if (!image) Fatal("Cannot allocate %d bytes of memory for image %s\n",size,file);
   //  Seek to and read image
   if (fseek(f,off,SEEK_SET) || fread(image,size,1,f)!=1) Fatal("Error reading data from image %s\n",file);
   fclose(f);
   //  Reverse colors (BGR -> RGB)
   for (k=0;k<size;k+=3)
   {
      unsigned char temp = image[k];
      image[k]   = image[k+2];
      image[k+2] = temp;
   }

   //  Sanity check
   ErrCheck("LoadTexBMP");
   //  Generate 2D texture
   unsigned int texture;
   glGenTextures(1,&texture);
   glBindTexture(GL_TEXTURE_2D,texture);
   //  Copy image
   glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
   if (glGetError()) Fatal("Error in glTexImage2D %s %dx%d\n",file,dx,dy);
   //  Scale linearly when image size doesn't match
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   //  Free image memory
   free(image);
   //  Return texture name
   return texture;
}

std::vector<float> createRectangularPrisimData(float w, float h, float d) {
    float sx = w / 2;
    float sy = h / 2;
    float sz = d / 2;

    return {
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
}

#endif