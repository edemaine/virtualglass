
#ifndef GLASSOPENGL_H
#define GLASSOPENGL_H

#include <string>

class Geometry;

using std::string;

namespace GlassOpenGL
{

void errors(string const &where);

void initialize();

void renderWithoutDepthPeeling(const Geometry& geometry);

}


#endif

