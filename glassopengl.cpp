
#include "glew.h"
#ifdef _WIN32
#  include <windows.h>
#endif
#include <iostream>
#include "geometry.h"
#include "glassopengl.h"
#include "globalbackgroundcolor.h"
#include "constants.h"

namespace GlassOpenGL
{

void errors(string const &where)
{
	GLuint err;
	while ((err = glGetError()) != GL_NO_ERROR) 
	{
		std::cerr << "(in " << where << ") OpenGL error #" << err
			<< ": " << gluErrorString(err) << std::endl;
	}
}

void initialize()
{
	// For shadow/lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);
}

void renderWithoutDepthPeeling(const Geometry& geometry)
{
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Check that Vertex and Triangle have proper size:
	assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3));
	assert(sizeof(Triangle) == sizeof(GLuint) * 3);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].position));
	glNormalPointer(GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].normal));
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// make a pass on opaque things
	for (std::vector< Group >::const_iterator g = geometry.groups.begin(); g != geometry.groups.end(); ++g)
	{
		Color c = g->color;
		if (c.a < 0.11) // 0.1 is the rounded up alpha for transparent things that *must* be seen
			continue;
		c.a = 0.8;
		glColor4f(c.r * c.a, c.g * c.a, c.b * c.a, c.a);
		glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
			GL_UNSIGNED_INT, &(geometry.triangles[g->triangle_begin].v1));
	}

	// make a pass on mandatory transparent things
	// this part assumes that there's only one mandatory transparent thing,
	// and that it's a polyhedron that contains everything else. 
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE); 
	for (std::vector< Group >::const_iterator g = geometry.groups.begin(); g != geometry.groups.end(); ++g)
	{
		Color c;
		c.r = c.g = c.b = 0.333;
		c.a = 0.1;
		glColor4f(c.r * c.a, c.g * c.a, c.b * c.a, c.a);
		glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
			GL_UNSIGNED_INT, &(geometry.triangles[g->triangle_begin].v1));
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	//final pass -- render background color behind everything else.
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_QUADS);
	glColor3f(GlobalBackgroundColor::color.r,
		GlobalBackgroundColor::color.g,
		GlobalBackgroundColor::color.b);
	glVertex2f(-1.1f,-1.1f);
	glVertex2f( 1.1f,-1.1f);
	glVertex2f( 1.1f, 1.1f);
	glVertex2f(-1.1f, 1.1f);
	glEnd();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

}

// On NVIDIA+integrated GPU machines, this tells Nvidia's Optimus driver
// to use the NVIDIA/discrete/high performance GPU.
// [http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf]
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
