#include "peelrenderer.h"

#include <stdexcept>

#define glewGetContext() glewContext

PeelRenderer::PeelRenderer(GLEWContext *_glewContext) : glewContext(_glewContext), expectedGLContext(QGLContext::currentContext()), bufferSize(make_vector(0U, 0U)), buffer(0), colorTex(0), depthTex(0), prevDepthTex(0), peelProgram(0), nopeelProgram(0) {
	if (!GLEW_ARB_texture_rectangle
	 || !GLEW_ARB_window_pos
	 || !GLEW_ARB_occlusion_query
	 || !GLEW_EXT_blend_func_separate
	 || !GLEW_ARB_depth_texture
	 || !GLEW_ARB_framebuffer_object
	 || !GLEW_ARB_depth_texture
	 || !GLEW_ARB_shader_objects
	 || !GLEW_ARB_shading_language_100
	 || !GLEW_ARB_fragment_shader
	 || !GLEW_ARB_vertex_shader) {
		std::cerr << "WARNING: some of the extensions required for depth peeling are not present, so it will be disabled." << std::endl;
		throw std::runtime_error("Missing extensions required for depth peeling.");
	}
}

PeelRenderer::~PeelRenderer() {
	assert(QGLContext::currentContext() == expectedGLContext);

	//Clean up peeling state:
	if (buffer) {
		glDeleteFramebuffers(1, &buffer);
		buffer = 0;
	}
	if (colorTex) {
		glDeleteTextures(1, &colorTex);
		colorTex = 0;
	}
	if (depthTex) {
		glDeleteTextures(1, &depthTex);
		depthTex = 0;
	}
	if (prevDepthTex) {
		glDeleteTextures(1, &prevDepthTex);
		prevDepthTex = 0;
	}
	if (peelProgram) {
		glDeleteObjectARB(peelProgram);
		peelProgram = 0;
	}
	if (nopeelProgram) {
		glDeleteObjectARB(nopeelProgram);
		nopeelProgram = 0;
	}

}

namespace {
void gl_errors(std::string const &where) {
	GLuint err;
	while ((err = glGetError()) != GL_NO_ERROR) {
	std::cerr << "(in " << where << ") OpenGL error #" << err
	          << ": " << gluErrorString(err) << std::endl;
	}
}
}

namespace {
	//convenience function: grab the error log of a shader:
	std::string shader_log(GLhandleARB shader, GLEWContext *glewContext) {
		assert(glewContext);
		GLint len = 0;
		glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &len);
		vector< GLchar > log;
		log.resize(len + 1, GLchar('\0'));
		GLint written = 0;
		glGetInfoLogARB(shader, len, &written, &(log[0]));
		assert(written <= len);
		std::string out = "";
		for (unsigned int i = 0; i < log.size() && log[i] != '\0'; ++i) {
			out += log[i];
		}
		return out;
	}

	GLhandleARB load_program(const char *frag, GLEWContext *glewContext) {
		assert(glewContext);
		GLhandleARB shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		GLint len = strlen(frag);
		glShaderSourceARB(shader, 1, &frag, &len);
		glCompileShaderARB(shader);
		{ //check shader:
			GLint val = 0;
			glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &val);
			std::string log = shader_log(shader, glewContext);
			if (val == 0) {
				std::cerr << "ERROR: failed compiling shader." << std::endl;
				std::cerr << "Log:\n" << log << std::endl;
			} else if (log != "") {
				std::cerr << "WARNING: peel shader compiled, but produced messages:\n" << log << std::endl;
			}
		}
		GLhandleARB program = glCreateProgramObjectARB();
		glAttachObjectARB(program, shader);
		glLinkProgramARB(program);
		{ //check program:
			GLint val = 0;
			glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &val);
			std::string log = shader_log(program, glewContext);
			if (val == 0) {
				std::cerr << "ERROR: failed linking program." << std::endl;
				std::cerr << "Log:\n" << log << std::endl;
			} else if (log != "") {
				std::cerr << "WARNING: peel program linked, but produced messages:\n" << log << std::endl;
			}
		}

		glDeleteObjectARB(shader);

		return program;
	}
}

void PeelRenderer::render(Vector3f bgColor, Geometry const &geometry) {
	assert(QGLContext::currentContext() == expectedGLContext);
		//std::cout << "Painting with context " << context() << "." << std::endl;
	//std::cout << " valid:" << context()->isValid() << std::endl;

	gl_errors("(before depth peeling)");
	//viewport is {x,y,w,h} in window.
	//We're querying here to get the width and height.
	GLint viewport[4] = {0,0,0,0};
	glGetIntegerv(GL_VIEWPORT, viewport);
	if (viewport[2] != GLint(bufferSize.x) || viewport[3] != GLint(bufferSize.y)) {
		bufferSize.x = viewport[2];
		bufferSize.y = viewport[3];
		std::cerr << "(re-)Allocating RenderThread peel textures for " << bufferSize << "." << std::endl;

		//Since the buffer has changed size, (re-)init textures:
		if (colorTex == 0) {
			glGenTextures(1, &colorTex);
		}
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, colorTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, bufferSize.x, bufferSize.y, 
			0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

		if (depthTex == 0) {
			glGenTextures(1, &depthTex);
		}
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, depthTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT, bufferSize.x, bufferSize.y, 
			0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

		if (prevDepthTex == 0) {
			glGenTextures(1, &prevDepthTex);
		}
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, prevDepthTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT, bufferSize.x, bufferSize.y, 
			0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

		gl_errors("(depth peeling setup)");
	}

	if (bufferSize.x == 0 || bufferSize.y == 0) {
		//nothing to render.
		return;
	}

	if (buffer == 0) {
		glGenFramebuffers(1, &buffer);
	}

	if (peelProgram == 0) {
		const char *peel_frag =
		"#extension GL_ARB_texture_rectangle : enable \n"
		"uniform sampler2DRect min_depth; \n"
		" \n"
		"void main() { \n"
		"	float depth = texture2DRect(min_depth, gl_FragCoord.xy).x; \n"
		"	if (gl_FragCoord.z <= depth) { \n"
		"		discard; \n"
		"	} \n"
		"	//Premultiply alpha to make compositing easier later: \n"
		"	gl_FragColor = vec4(gl_Color.xyz * gl_Color.w, gl_Color.w); \n"
		"} \n";
		peelProgram = load_program(peel_frag, glewContext);
		gl_errors("compiling peel program.");
	}

	if (nopeelProgram == 0) {
		const char *nopeel_frag =
		"void main() { \n"
		"	//Premultiply alpha to make compositing easier later: \n"
		"	gl_FragColor = vec4(gl_Color.xyz * gl_Color.w, gl_Color.w); \n"
		"} \n";


		nopeelProgram = load_program(nopeel_frag, glewContext);
		gl_errors("compiling nopeel program.");
	}

	GLuint query = 0;
	glGenQueriesARB(1, &query);

	GLint base_draw_framebuffer = 0;
	GLint base_read_framebuffer = 0;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &base_draw_framebuffer);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &base_read_framebuffer);

	//Render depth layers, front-to-back, up to MaxPasses layers:
	const unsigned int MaxPasses = 20;
	for (unsigned int pass = 0; pass < MaxPasses; ++pass) {
		//---------- setup framebuffer ----------
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer);
		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,bufferSize.x,bufferSize.y);

		//Set up the proper depth-n-such attachments:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, colorTex, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE_ARB, depthTex, 0);

		{ //check:
			GLenum ret = glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER );
			if (ret != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << "WARNING: FRAMEBUFFER not complete!" << std::endl;
			}
		}
		gl_errors("(depth framebuffer setup)");

		//clear the framebuffer:
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set up peeling program to reject (close) stuff we've already rendered
		if (pass == 0) {
			glUseProgramObjectARB(nopeelProgram);
		} else {
			glUseProgramObjectARB(peelProgram);
			glUniform1iARB(glGetUniformLocationARB(peelProgram, "min_depth"), 0);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, prevDepthTex);
		}

		if (pass + 1 < MaxPasses) {
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
		} else {
			//If we've peeled all we can peel, just render in some order and
			//hope for the best:
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
		}

		bool do_query = (pass != 0);

		//When actually peeling, remember how many fragments were used:
		if (do_query) {
			glBeginQueryARB(GL_SAMPLES_PASSED_ARB, query);
		}
		//---------- draw scene --------
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);

		{ //actual rendering!
			glEnable(GL_LIGHTING);
			//Check that Vertex and Triangle have proper size:
			assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3));
			assert(sizeof(Triangle) == sizeof(GLuint) * 3);

			glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].position));
			glNormalPointer(GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].normal));
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);

			for (std::vector< Group >::const_iterator g = geometry.groups.begin(); g != geometry.groups.end(); ++g) {
				assert(g->color);
				Color c = *(g->color);
				if (g->ensureVisible && c.a < 0.05)
					c.a = 0.05;
				glColor4f(c.r, c.g, c.b, c.a);
				glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
							   GL_UNSIGNED_INT, &(geometry.triangles[g->triangle_begin].v1));
			}


			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);

			glDisable(GL_LIGHTING);
		}

		if (do_query) {
			glEndQueryARB(GL_SAMPLES_PASSED_ARB);
		}

		//Done drawing scene; detach framebuffer:
		glPopAttrib();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, base_draw_framebuffer); //detach framebuffer
		gl_errors("(depth framebuffer render)");

		//--------------------------

		glUseProgramObjectARB(0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);


		if (do_query) {
			GLuint count = 0;
			glGetQueryObjectuivARB(query, GL_QUERY_RESULT_ARB, &count);

			//std::cout << "On pass " << pass << ", generated " << count << " fragments." << std::endl;
			//if we're no longer rendering any fragments, skip the pixel copy:
			if (count == 0) {
				break;
			}
		}

		//swap out depth textures, now that we've rendered a new one:
		std::swap(prevDepthTex, depthTex);


		//Copy pixels over to visible framebuffer:
		glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer);

		//copy pixels to screen:
		glWindowPos2iARB(0,0);
		if (pass == 0) {
			//First pass, we copy:
			glDisable(GL_BLEND);
		} else {
			//subsequent passes get added in:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
		}
		glDisable(GL_DEPTH_TEST);
		//TODO: possibly use alpha test here to save some fill?
		glCopyPixels(0,0,bufferSize.x,bufferSize.y,GL_COLOR);

		glDisable(GL_BLEND);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, base_read_framebuffer);

		gl_errors("(copy framebuffer)");
	}

	glDeleteQueriesARB(1, &query);

	//final pass -- render background color behind everything else.
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
	glColor3f(bgColor.r, bgColor.g, bgColor.b);
	glVertex2f(-1.1f,-1.1f);
	glVertex2f( 1.1f,-1.1f);
	glVertex2f( 1.1f, 1.1f);
	glVertex2f(-1.1f, 1.1f);
	glEnd();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
