
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
	glEnable(GL_DEPTH_TEST);
}

void renderWithoutDepthPeeling(const Geometry& geometry)
{
	// This method has a lot in common with PeelRenderer::render()
	// It's effectively a weak skeleton of that one, with a new trick thrown in.
	// The goal here is to render without using real transparency testing
	// while still looking ok; in particular no funny M.C. Escher-like artifacts.
	//
	// The trick we use is to map all the actual alphas into two groups:
	// opaque (a = 1.0) and nearly transparent (a <= 0.1). Everything with 
	// alpha more than 0.1 is made opaque and everything else keeps its originally alpha.
	// We then render all opaque things first using regular occlusion, 
	// i.e. no blending and CULL_FACE and DEPTH_TEST on. Then we render
	// all transparent things (in arbitrary/uncontrollable) order with BLEND on.
	// 
	// The "trick" is meant to fix the lack of good z-order of transparent things.
	// Intuitively, the alpha mapping enforces the following rule:
	// "Transparent things are so transparent that the user can't
	// tell if they're rendered in the wrong order." 

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Check that Vertex and Triangle have proper size:
	assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3));
	assert(sizeof(Triangle) == sizeof(GLuint) * 3);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].position));
	glNormalPointer(GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].normal));
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	// make a pass on opaque things
	for (std::vector< Group >::const_iterator g = geometry.groups.begin(); g != geometry.groups.end(); ++g)
	{
		Color c = g->color;
		if (c.a < 0.1)
			continue;
		glColor3f(c.r, c.g, c.b);
		glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
			GL_UNSIGNED_INT, &(geometry.triangles[g->triangle_begin].v1));
	}


	// make a pass on mandatory transparent things
	glEnable(GL_CULL_FACE); // Note we cull face
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
	for (std::vector< Group >::const_iterator g = geometry.groups.begin(); g != geometry.groups.end(); ++g)
	{
		Color c = g->color;
		if (c.a >= 0.1)
			continue;
		if (g->ensureVisible)
			c.a = MAX(c.a, 0.1);
		if (c.a < 0.01)
			continue;
		// Here we have to fake the color adjustment that occurs in depth peeling.
		// We need to use a blend function that doesn't just do something additive,
		// so we mimic depth peeling and use GL_ONE_MINUS_DST_ALPHA for the incoming color adjustments.
		// But adjusting color based on existing alpha causes rendering order to matter;
		// it gives weird artifacts caused by drawing some away-facing triangles first,
		// and others second, giving canes a vertical "seam". So we cull back faces (GL_CULL_FACE). 
		// Finally, this results in a brighter appearance than in depth peeling, since
		// back faces have away-pointing normals that give them darker colors than toward-facing
		// normals due to shading. We fix this by adjusting the color of the toward-facing triangle
		// by a small amount matching the (assumed and approximate) contribution that the 
		// normal + shading has on the away-facing triangle, and then *not* rendering the back face.
		//
		// We also "premultiply by alpha" for an unknown reason as done in PeelRenderer::render()
		// In the end, it makes nearly invisible stuff match the color you'd expect, so seems good!
		//
		// Equations are as follows, given a cane with color c:
		//
		//       (front face with shading)    (back face with shading and ONE_MINUS_DST_ALPHA approx)
		// red =     c.r * c.a * 1.2       +                c.r * c.a * -0.2 * (1.0 - (c.a + 0.1)) 
		//     = c.r * c.a * (1.02 + 0.2 * c.a) 
		//
		//         (front face)   (back face with ONE_MINUS_DST_ALPHA approx)
		// alpha =      c.a     +                (1.0 - (c.a + 0.1)) * c.a 
		//       = c.a * (1.9 - c.a) 
		
		float color_coeff = c.a * (1.02 + 0.2 * c.a);
		float alpha_coeff = (1.9 - c.a);
		glColor4f(c.r * color_coeff, c.g * color_coeff, c.b * color_coeff, c.a * alpha_coeff);		
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


