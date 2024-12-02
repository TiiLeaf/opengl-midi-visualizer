#ifndef OPENGL_HELPERS_CPP
#define OPENGL_HELPERS_CPP

#include <vector>
#include <string>

void drawAxes() {
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(3, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 3, 0);
    glColor3f(0, 0, 1);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, 3);
    glEnd();
    glColor3f(1, 1, 1);
}

/*
    Update the perspective projection to handle aspect ratio changes
*/
void setProjection(float aspectRatio) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60, aspectRatio, 0.1, 100);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

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

template <typename T> int indexOf(std::vector<T> vec, T val) {
   for (size_t i = 0; i < vec.size(); i++) {
      if (vec.at(i) == val)
         return (int)i;
   }
   return -1;
}

void reverse(void* x, const int n) {
   char* ch = (char*)x;
   for (int k = 0; k < n / 2; k++) {
      char tmp = ch[k];
      ch[k] = ch[n - 1 - k];
      ch[n - 1 - k] = tmp;
   }
}

void fatal(const char* format, ...) {
   va_list args;
   va_start(args, format);
   vfprintf(stderr, format, args);
   va_end(args);
   exit(1);
}

unsigned int loadBmpFile(const char* file) {
   //  Open file
   FILE* f = fopen(file, "rb");
   if (!f) fatal("Cannot open file %s\n", file);
   //  Check image magic
   unsigned short magic;
   if (fread(&magic, 2, 1, f) != 1) fatal("Cannot read magic from %s\n", file);
   if (magic != 0x4D42 && magic != 0x424D) fatal("Image magic not BMP in %s\n", file);
   //  Read header
   unsigned int dx, dy, off, k; // Image dimensions, offset and compression
   unsigned short nbp, bpp;   // Planes and bits per pixel
   if (fseek(f, 8, SEEK_CUR) || fread(&off, 4, 1, f) != 1 ||
      fseek(f, 4, SEEK_CUR) || fread(&dx, 4, 1, f) != 1 || fread(&dy, 4, 1, f) != 1 ||
      fread(&nbp, 2, 1, f) != 1 || fread(&bpp, 2, 1, f) != 1 || fread(&k, 4, 1, f) != 1)
      fatal("Cannot read header from %s\n", file);
   //  Reverse bytes on big endian hardware (detected by backwards magic)
   if (magic == 0x424D) {
      reverse(&off, 4);
      reverse(&dx, 4);
      reverse(&dy, 4);
      reverse(&nbp, 2);
      reverse(&bpp, 2);
      reverse(&k, 4);
   }
   //  Check image parameters
   unsigned int max;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, (int*)&max);
   if (dx<1 || dx>max) fatal("%s image width %d out of range 1-%d\n", file, dx, max);
   if (dy<1 || dy>max) fatal("%s image height %d out of range 1-%d\n", file, dy, max);
   if (nbp != 1)  fatal("%s bit planes is not 1: %d\n", file, nbp);
   if (bpp != 24) fatal("%s bits per pixel is not 24: %d\n", file, bpp);
   if (k != 0)    fatal("%s compressed files not supported\n", file);
#ifndef GL_VERSION_2_0
   //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
   for (k = 1;k < dx;k *= 2);
   if (k != dx) fatal("%s image width not a power of two: %d\n", file, dx);
   for (k = 1;k < dy;k *= 2);
   if (k != dy) fatal("%s image height not a power of two: %d\n", file, dy);
#endif

   //  Allocate image memory
   unsigned int size = 3 * dx * dy;
   unsigned char* image = (unsigned char*)malloc(size);
   if (!image) fatal("Cannot allocate %d bytes of memory for image %s\n", size, file);
   //  Seek to and read image
   if (fseek(f, off, SEEK_SET) || fread(image, size, 1, f) != 1) fatal("Error reading data from image %s\n", file);
   fclose(f);
   //  Reverse colors (BGR -> RGB)
   for (k = 0;k < size;k += 3) {
      unsigned char temp = image[k];
      image[k] = image[k + 2];
      image[k + 2] = temp;
   }

   //  Generate 2D texture
   unsigned int texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   //  Copy image
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dx, dy, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
   if (glGetError()) fatal("Error in glTexImage2D %s %dx%d\n", file, dx, dy);
   //  Scale linearly when image size doesn't match
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   //  Free image memory
   free(image);
   //  Return texture name
   return texture;
}

#endif