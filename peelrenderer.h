#ifndef PEELRENDERER_HPP
#define PEELRENDERER_HPP

#include "glew.h"
#include <QGLContext>

class Geometry;

//Make sure you call all functions of PeelRenderer (including the constructor and destructor) while the same OpenGL context is current.
class PeelRenderer
{
	public:
		PeelRenderer(GLEWContext *glewContext);
		~PeelRenderer();
		void render(Geometry const &geometry);

		GLEWContext *glewContext;

		const QGLContext *expectedGLContext; //to keep you honest.
		Vector2ui bufferSize;
		GLuint buffer; //framebuffer
		GLuint colorTex; //color texture, stores current layer
		GLuint depthTex; //depth texture, stores current depth
		GLuint prevDepthTex; //stores previous depth
		GLhandleARB peelProgram; //program that rejects fragments based on depth
		GLhandleARB nopeelProgram; //program that premultiplies by alpha, but doesn't actually reject

};

#endif //PEELRENDERER_HPP
