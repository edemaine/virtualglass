/*
This class is the QT GUI object that does 3D rendering.
It also responds to mouse clicks and mouse movement within
its extent. As this object handles mouse clicks and movement,
it is involved in modifying the cane.

Features:

MODES:
The OpenGLWidget object has a mode for each type of modification
the user can make to the cane (in addition to a LOOK_MODE, which
changes the orientation of the cane only). Changing modes is handled by
other parts of the GUI.
*/

#include "openglwidget.h"

namespace {
void gl_errors(string const &where) {
	GLuint err;
	while ((err = glGetError()) != GL_NO_ERROR) {
	cerr << "(in " << where << ") OpenGL error #" << err
		 << ": " << gluErrorString(err) << endl;
	}
}
}

OpenGLWidget :: OpenGLWidget(QWidget *parent, Model* _model) : QGLWidget(QGLFormat(QGL::AlphaChannel | QGL::DoubleBuffer | QGL::DepthBuffer), parent)
{
	shiftButtonDown = false;
	rightMouseDown = false;
	deleteButtonDown = false;

	bgColor = QColor(100, 100, 100);


	model = _model;
	geometry = NULL;

	showAxes = true;
	show2D = false;

	lookAtLoc[0] = 0.0f;
	lookAtLoc[1] = 0.0f;
	lookAtLoc[2] = 0.5f;

	theta = -PI/2.0;
	fee = PI/4.0;
	rho = 3.0;

	mouseLocX = 0;
	mouseLocY = 0;

	selectBuffer = NULL;

	initializeGLCalled = false;

	peelEnable = true;
	peelInitContext = NULL;
	peelBufferSize = make_vector(0U, 0U);
	peelBuffer = 0;
	peelColorTex = 0;
	peelDepthTex = 0;
	peelPrevDepthTex = 0;
	peelProgram = 0;
	nopeelProgram = 0;

	setMouseTracking(true);
	updateTriangles();
}

OpenGLWidget :: ~OpenGLWidget()
{
	//Deallocate all the depth peeling resources we may have created:
	makeCurrent();

	if (selectBuffer) {
		delete selectBuffer;
		selectBuffer = NULL;
	}

	if (peelBuffer) {
		glDeleteFramebuffers(1, &peelBuffer);
		peelBuffer = 0;
	}
	if (peelColorTex) {
		glDeleteTextures(1, &peelColorTex);
		peelColorTex = 0;
	}
	if (peelDepthTex) {
		glDeleteTextures(1, &peelDepthTex);
		peelDepthTex = 0;
	}
	if (peelPrevDepthTex) {
		glDeleteTextures(1, &peelPrevDepthTex);
		peelPrevDepthTex = 0;
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

Model* OpenGLWidget :: getModel()
{
	return this->model;
}

void OpenGLWidget :: caneChanged()
{
	updateTriangles();
	repaint();
}

void OpenGLWidget :: setShiftButtonDown(bool state)
{
	shiftButtonDown = state;
}


void OpenGLWidget :: projectionChanged()
{
	update();
}

void OpenGLWidget :: setDeleteButtonDown(bool state)
{
	deleteButtonDown = state;
}

void OpenGLWidget :: checkDepthPeel()
{
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "WARNING: Failure initializing glew: " << glewGetErrorString(err) << std::endl;
		std::cerr << " ... we will continue, but code that uses extensions will cause a crash" << std::endl;
		peelEnable = false;
	}
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
		std::cerr << "WARNING: some of the extensions required for depth peeling are not present." << std::endl;
		peelEnable = false;
	}
}

bool OpenGLWidget :: peelEnabled()
{
	return peelEnable;
}

void OpenGLWidget :: initializeGL()
{
	initializeGLCalled = true;
	// set up glew:
	checkDepthPeel();
	emit updatePeelButton(peelEnable);
	// For shadow/lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	gl_errors("OpenGLWidget::initializeGL");
}

void OpenGLWidget :: setBgColor(QColor color)
{
	this->qglClearColor(color);
	bgColor = color;
	update();
}

QImage OpenGLWidget :: renderImage() {
	if (!initializeGLCalled) {
		initializeGL();
	}
	makeCurrent();
	QGLFramebufferObject fb(300, 300, QGLFramebufferObject::Depth);
	fb.bind();
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0,0,300,300);
	paintGL();
	glPopAttrib();
	fb.release();
	return fb.toImage();
}


int OpenGLWidget :: getSubcaneUnderMouse(int mouseX, int mouseY)
{
	makeCurrent();
	setGLMatrices();
	GLint viewport[4] = {0,0,0,0};
	glGetIntegerv(GL_VIEWPORT, viewport);
	if (mouseX < 0 || mouseX >= viewport[2] || mouseY < 0 || mouseY >= viewport[3]) {
		return -1;
	}

	if (selectBuffer) {
		if (selectBuffer->size().width() != viewport[2] || selectBuffer->size().height() != viewport[3]) {
			delete selectBuffer;
			selectBuffer = NULL;
		}
	}

	if (!selectBuffer) {
		selectBuffer = new QGLFramebufferObject(viewport[2], viewport[3], QGLFramebufferObject::Depth, GL_TEXTURE_2D, GL_RGBA8);
	}

	assert(selectBuffer->isValid());

	selectBuffer->bind();
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0,0,viewport[2],viewport[3]);

	geometry = model->getGeometry();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Check that Vertex and Triangle have proper size:
	assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3));
	assert(sizeof(Triangle) == sizeof(GLuint) * 3);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry->vertices[0].position));
	glEnableClientState(GL_VERTEX_ARRAY);
	for (std::vector< Group >::const_iterator g = geometry->groups.begin(); g != geometry->groups.end(); ++g) {
		glColor4ubv(reinterpret_cast< const GLubyte * >(&(g->tag)));
		glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
					   GL_UNSIGNED_INT, &(geometry->triangles[g->triangle_begin].v1));
	}
	glDisableClientState(GL_VERTEX_ARRAY);

	GLubyte c[4] = {0, 0, 0, 0};
	glReadPixels(mouseX, this->height() - mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &c);

	glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);

	glPopAttrib();
	selectBuffer->release();

	updateTriangles();

	gl_errors("getSubcaneUnderMouse");

	if (((int) c[0]) == 255)
			return -1;
	return ((int) c[0]);
}

Point OpenGLWidget :: getClickedPlanePoint(int mouseLocX, int mouseLocY)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;
	GLdouble posX1, posY1, posZ1;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = mouseLocX;
	winY = viewport[3] - mouseLocY;

	glReadPixels( mouseLocX, int(winY), 1.0, 1.0, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	//gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	gluUnProject( winX, winY, 0.0f, modelview, projection, viewport, &posX, &posY, &posZ);
	gluUnProject( winX, winY, 1.0f, modelview, projection, viewport, &posX1, &posY1, &posZ1);

	Point result, result1;
	result.x=posX;
	result.y=posY;
	result.z=posZ;
	result1.x=posX1;
	result1.y=posY1;
	result1.z=posZ1;
	Point dir = result1-result;
	return dir*abs(result.z/dir.z)+result;
}

Point OpenGLWidget :: getClickedPlanePoint(int mouseLocX, int mouseLocY, float zHeight)
{ // given mouse window coordinates and a z-plane, return the point of intersection
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;
	GLdouble posX1, posY1, posZ1;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = mouseLocX;
	winY = viewport[3] - mouseLocY;

	glReadPixels( mouseLocX, int(winY), 1.0, 1.0, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	//gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	gluUnProject( winX, winY, 0.0f, modelview, projection, viewport, &posX, &posY, &posZ);
	gluUnProject( winX, winY, 1.0f, modelview, projection, viewport, &posX1, &posY1, &posZ1);

	Point result, result1;
	result.x=posX;
	result.y=posY;
	result.z=posZ;
	result1.x=posX1;
	result1.y=posY1;
	result1.z=posZ1;
	Point dir = result1-result;
	if (0<=zHeight && zHeight<result.z)
		return dir*abs((result.z-zHeight)/dir.z)+result;
	else
		return dir*abs((result.z)/dir.z)+result;
}

Point OpenGLWidget :: getClickedCanePoint(int activeSubcane, int mouseLocX, int mouseLocY)
{ // given mouse window coordinates, and an active subcane, returns an approximate point of click on cane
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;
	GLdouble posX1, posY1, posZ1;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = mouseLocX;
	winY = viewport[3] - mouseLocY;

	glReadPixels( mouseLocX, int(winY), 1.0, 1.0, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	//gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	gluUnProject( winX, winY, 0.0f, modelview, projection, viewport, &posX, &posY, &posZ);
	gluUnProject( winX, winY, 1.0f, modelview, projection, viewport, &posX1, &posY1, &posZ1);

	Point result, result1;
	result.x=posX;
	result.y=posY;
	result.z=posZ;
	result1.x=posX1;
	result1.y=posY1;
	result1.z=posZ1;
	Point dir = result1-result;
	dir =  dir*abs(result.z/dir.z)+result; // x-y plane point
	Point xy = getModel()->getCane()->subcaneLocations[activeSubcane];
	xy.z = 0; //
	Point resultSansZ = result;
	resultSansZ.z = 0;
	xy.z = (length(xy-dir)+.25)*result.z/length(result-dir);
	return xy;
}

namespace {
	//convenience function: grab the error log of a shader:
	string shader_log(GLhandleARB shader) {
		GLint len = 0;
		glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &len);
		vector< GLchar > log;
		log.resize(len + 1, GLchar('\0'));
		GLint written = 0;
		glGetInfoLogARB(shader, len, &written, &(log[0]));
		assert(written <= len);
		string out = "";
		for (unsigned int i = 0; i < log.size() && log[i] != '\0'; ++i) {
			out += log[i];
		}
		return out;
	}

	GLhandleARB load_program(const char *frag) {
		GLhandleARB shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		GLint len = strlen(frag);
		glShaderSourceARB(shader, 1, &frag, &len);
		glCompileShaderARB(shader);
		{ //check shader:
			GLint val = 0;
			glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &val);
			string log = shader_log(shader);
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
			string log = shader_log(program);
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


/*
Draws the scene layer-by-layer using the technique known as "depth peeling"
Note: you must have already called makeCurrent() and set up the camera.
(This is meant to be a helper used from inside paintGL.)
*/
void OpenGLWidget :: paintWithDepthPeeling()
{
	//std::cout << "Painting with context " << context() << "." << std::endl;
	//std::cout << " valid:" << context()->isValid() << std::endl;

	makeCurrent(); //Shouldn't need to call this?

	gl_errors("(before depth peeling)");
	//viewport is {x,y,w,h} in window.
	//We're querying here to get the width and height.
	GLint viewport[4] = {0,0,0,0};
	glGetIntegerv(GL_VIEWPORT, viewport);
	if (viewport[2] != GLint(peelBufferSize.x) || viewport[3] != GLint(peelBufferSize.y)) {
		if (!peelInitContext) {
			//Remember the first context we init'd stuff in,
			// this can change if someone tries to QPixmap::grabWidget
			// and that causes all sorts of bad stuff to happen.
			peelInitContext = context();
		}

		peelBufferSize.x = viewport[2];
		peelBufferSize.y = viewport[3];
		std::cerr << "(re-)Allocating OpenGLWidget textures for " << peelBufferSize << " window." << std::endl;

		//Since the buffer has changed size, (re-)init textures:
		if (peelColorTex == 0) {
			glGenTextures(1, &peelColorTex);
		}
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, peelColorTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, peelBufferSize.x, peelBufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

		if (peelDepthTex == 0) {
			glGenTextures(1, &peelDepthTex);
		}
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, peelDepthTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT, peelBufferSize.x, peelBufferSize.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

		if (peelPrevDepthTex == 0) {
			glGenTextures(1, &peelPrevDepthTex);
		}
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, peelPrevDepthTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT, peelBufferSize.x, peelBufferSize.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

		gl_errors("(depth peeling setup)");
	}
	//we had better be in the same context as init'd.
	//If this assert is failing, stop using QPixmap::grabWidget and
	// start using QPixmap::fromImage(widget->grabFrameBuffer())...
	assert(peelInitContext == context());

	if (peelBufferSize.x == 0 || peelBufferSize.y == 0) {
		//nothing to render.
		return;
	}

	if (peelBuffer == 0) {
		glGenFramebuffers(1, &peelBuffer);
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
		peelProgram = load_program(peel_frag);
		gl_errors("compiling peel program.");
	}

	if (nopeelProgram == 0) {
		const char *nopeel_frag =
		"void main() { \n"
		"	//Premultiply alpha to make compositing easier later: \n"
		"	gl_FragColor = vec4(gl_Color.xyz * gl_Color.w, gl_Color.w); \n"
		"} \n";


		nopeelProgram = load_program(nopeel_frag);
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
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, peelBuffer);
		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,peelBufferSize.x,peelBufferSize.y);

		//Set up the proper depth-n-such attachments:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, peelColorTex, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE_ARB, peelDepthTex, 0);

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
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, peelPrevDepthTex);
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

		if (showAxes)
			drawAxes();

		if (geometry) {
			glEnable(GL_LIGHTING);
			//Check that Vertex and Triangle have proper size:
			assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3));
			assert(sizeof(Triangle) == sizeof(GLuint) * 3);

			glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry->vertices[0].position));
			glNormalPointer(GL_FLOAT, sizeof(Vertex), &(geometry->vertices[0].normal));
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);

			for (std::vector< Group >::const_iterator g = geometry->groups.begin(); g != geometry->groups.end(); ++g) {
				assert(g->cane);
				Color c = g->cane->color;
				if (model && (int)g->tag == model->getActiveSubcane()) {
					c.xyz += make_vector(0.1f, 0.1f, 0.1f);
				}
				glColor4f(c.r, c.g, c.b, c.a);
				glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
							   GL_UNSIGNED_INT, &(geometry->triangles[g->triangle_begin].v1));
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
		std::swap(peelPrevDepthTex, peelDepthTex);


		//Copy pixels over to visible framebuffer:
		glBindFramebuffer(GL_READ_FRAMEBUFFER, peelBuffer);

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
		glCopyPixels(0,0,peelBufferSize.x,peelBufferSize.y,GL_COLOR);

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
	glColor3f(bgColor.redF(), bgColor.greenF(), bgColor.blueF());
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

/*
Handles the drawing of a triangle mesh.
The triangles array is created and lives in the
Mesh object, and the OpenGLWidget object simply
receives a pointer to this array.
*/
void OpenGLWidget :: paintGL()
{
	setGLMatrices();
	this->qglClearColor(bgColor);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (peelEnable) {
		paintWithDepthPeeling();
	} else {
		paintWithoutDepthPeeling();
	}
}

void OpenGLWidget :: paintWithoutDepthPeeling()
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	if (showAxes)
		drawAxes();

	if (geometry) {
		glEnable(GL_LIGHTING);
		//Check that Vertex and Triangle have proper size:
		assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3));
		assert(sizeof(Triangle) == sizeof(GLuint) * 3);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry->vertices[0].position));
		glNormalPointer(GL_FLOAT, sizeof(Vertex), &(geometry->vertices[0].normal));
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		for (std::vector< Group >::const_iterator g = geometry->groups.begin(); g != geometry->groups.end(); ++g) {
			assert(g->cane);
			Color c = g->cane->color;
			if (model && (int)g->tag == model->getActiveSubcane()) {
				c.xyz += make_vector(0.1f, 0.1f, 0.1f);
			}
			glColor4f(c.r, c.g, c.b, 1.0);
			glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
						   GL_UNSIGNED_INT, &(geometry->triangles[g->triangle_begin].v1));
		}



		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		glDisable(GL_LIGHTING);
	}

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	//called automatically: swapBuffers();
}

void drawCircle(float radius)
{
	glBegin(GL_LINE_LOOP);
	for (int angle=0;angle<=360;angle+=10){
		glVertex3f(radius*cos(angle*PI/180.),radius*sin(angle*PI/180.),0);
	}
	glEnd();
}

void drawDottedCircle(float radius)
{
	glBegin(GL_LINE_LOOP);
	for (int angle=0;angle<=360;angle+=10){
		glVertex3f(radius*cos(angle*PI/180.),radius*sin(angle*PI/180.),0);
	}
	glEnd();
}

void drawSegment(Point p1, Point p2)
{
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex3f(p1.x,p1.y,0);
	glVertex3f(p2.x,p2.y,0);
	glEnd();
	glPopMatrix();
}

/*
Calls if the OpenGLWidget object is resized (in the GUI sense).
*/
void OpenGLWidget :: resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	if (this->width() != width || this->height() != height) {
		std::cerr << "resizeGL(" << width << ", " << height << ") called while this->width,height are (" << this->width() << ", " << this->height() << "). This may mess up aspect ratio." << std::endl;
	}
	//I think paintGL will get called now...
}

void OpenGLWidget :: zoomIn()
{
	rho *= 0.8;
	update();
}

void OpenGLWidget :: zoomOut()
{
	rho *= 1.2;
	update();
}

void OpenGLWidget :: setTopView()
{
	setCamera(0.0,0.01);
}

void OpenGLWidget :: setSideView()
{
	setCamera(0.0, PI/2);
}

void OpenGLWidget :: setFrontView()
{
	setCamera(-PI/2, PI/2);
}

void OpenGLWidget :: toggleAxes()
{
	setAxes(!showAxes);
}

void OpenGLWidget :: setAxes(bool show)
{
	showAxes = show;
	update();
}

/*
Should be called any time the Mesh object is changed by
modifying the cane it contains.
*/
void OpenGLWidget :: updateTriangles()
{
	geometry = model->getGeometry();
}

/*
Called to set up projection and modelview matrices
*/
void OpenGLWidget :: setGLMatrices()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLint viewport[4] = {0,0,0,0};
	glGetIntegerv(GL_VIEWPORT, viewport);

	float w = viewport[2];
	float h = viewport[3];
	if (model->getProjection() == ORTHOGRAPHIC_PROJECTION || show2D) {
		if (w > h) {
			float a = h / w;
			float s = 2.2f / rho;
			glScalef(a * s, s,-0.01);
		} else {
			float a = w / h;
			float s = 2.2f / rho;
			glScalef(s, a * s,-0.01);
		}
	} else {
		gluPerspective(45.0, w / h, 0.01, 100.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float eyeLoc[3];

	if (show2D)
	{
		eyeLoc[0] = lookAtLoc[0] + rho*sin(0.0001)*cos(theta);
		eyeLoc[1] = lookAtLoc[1] + rho*sin(0.0001)*sin(theta);
		eyeLoc[2] = lookAtLoc[2] + rho*cos(0.0001);
	}
	else
	{
		eyeLoc[0] = lookAtLoc[0] + rho*sin(fee)*cos(theta);
		eyeLoc[1] = lookAtLoc[1] + rho*sin(fee)*sin(theta);
		eyeLoc[2] = lookAtLoc[2] + rho*cos(fee);
	}

	gluLookAt(eyeLoc[0], eyeLoc[1], eyeLoc[2],
			  lookAtLoc[0], lookAtLoc[1], lookAtLoc[2],
			  0.0, 0.0, 1.0);
}

/*
Currently catches all mouse press events
(left and right buttons, etc.).
*/
void OpenGLWidget :: mousePressEvent (QMouseEvent* e)
{
	// Update instance variables for mouse location
	mouseLocX = e->x();
	mouseLocY = e->y();

	if (e->button() == Qt::RightButton)
	{
		if (model->subcaneHasColorAndShape(getSubcaneUnderMouse(mouseLocX, mouseLocY)))
			emit caneChangeRequest(getSubcaneUnderMouse(mouseLocX, mouseLocY));
		else
			rightMouseDown = true;
		return;
	}

	// Only set a new active subcane if you're going to do something with it
	if (model->getMode() == BUNDLE_MODE || deleteButtonDown)
		model->setActiveSubcane(getSubcaneUnderMouse(mouseLocX, mouseLocY));
	if (deleteButtonDown)
	{
		model->deleteActiveCane();
	}

	update();
}

/*
Currently catches all mouse release events
(left and right buttons, etc.).
*/
void OpenGLWidget :: mouseReleaseEvent (QMouseEvent* e)
{
	//check if cane is in a snap, and finalize it if true
	if (model->getActiveSubcane() != -1)
	{
		model->setActiveSubcane(-1);
	}

	if (e->button() == Qt::RightButton)
		rightMouseDown = false;
	else
		model->slowGeometryUpdate();
}

/*
This method is an event handler called when the mouse
is moved *and a button is down*. Depending on the mode,
the cane (and view of the cane) changes. This is
part of the mode feature.
*/
void OpenGLWidget :: mouseMoveEvent (QMouseEvent* e)
{
	float relX, relY;
	float newFee;
	float windowWidth, windowHeight;
	int oldMouseLocX, oldMouseLocY;

	// only admit mouse moves without a button down if in
	// bundle mode (for illumination upon scrollover of cane)
	if (model->getMode() != BUNDLE_MODE && e->buttons() == 0)
		return;

	windowWidth = this->width();
	windowHeight = this->height();

	// Calculate how much mouse moved
	oldMouseLocX = mouseLocX;
	mouseLocX = e->x();
	relX = (mouseLocX - oldMouseLocX) / windowWidth;
	oldMouseLocY = mouseLocY;
	mouseLocY = e->y();
	relY = (mouseLocY - oldMouseLocY) / windowHeight;

	if (rightMouseDown)
	{
		theta -= (relX * 500.0 * PI / 180.0);
		if (!show2D)
		{
			newFee = fee - (relY * 500.0 * PI / 180.0);
			if (newFee > 0.0f && newFee < PI)
				fee = newFee;
		}

		update();
		return;
	}


	switch (model->getMode())
	{
	case PULL_MODE:
		if (shiftButtonDown)
		{
			if (abs(relX) > abs(relY))
			{
				model->pullCane(relX * PI, 0.0);
			}
			else
			{
				model->pullCane(0.0, -5.0*relY);
			}
		}
		else
		{
			model->pullCane(relX * PI, -5.0*relY);
		}
		emit operationInfoSig(QString("Twisted %1 Revolutions Per Viewable Length, Pulled %2").arg(model->getCane()->amts[0] / PI / 2).arg(model->getCane()->amts[1]),1000);
		break;
	case BUNDLE_MODE:
		/*
		How the parameters for moveCane() are calculated is not obvious.
		The idea is to make mouse X/Y correspond to the cane moving
		left-right/up-down *regardless* of where the camera is. This
		is why theta (the camera angle relative to the look-at point) is
		also involved.

		Essentially, the parameters convert the amount moved in X and Y
		(variables `relX' and `relY') to the amount moved in X and Y
		according to axes on which the cane lives.
		*/

		if (e->buttons() & 0x00000001) // if left mouse button is down
		{
			if (shiftButtonDown)
			{
				model->moveCane(0, 0, relX * 5);
			}
			else
			{
				relX *= 1.75 * rho; // tone it down
				relY *= 1.5 * rho; // tone it down
				model->moveCane(relX * cos(theta + PI / 2.0) + relY * cos(theta),
					relX * sin(theta + PI / 2.0) + relY * sin(theta), 0);
			}
		}
		if (model->getActiveSubcane() != -1 && model->getCane()) {
			emit operationInfoSig(QString("Moved X Direction %1, Y Direction %2").arg(
				model->getCane()->subcaneLocations[model->getActiveSubcane()].x).arg(
				model->getCane()->subcaneLocations[model->getActiveSubcane()].y),1000);
		}
		break;
	case FLATTEN_MODE:
		model->flattenCane(-relX, PI / 2, -relY);
		emit operationInfoSig(QString("Squished with %1, Flattened into rectangle with %2").arg(
			model->getCane()->amts[0]).arg(model->getCane()->amts[2]),1000);
		break;
	}

}

void OpenGLWidget :: wheelEvent(QWheelEvent *e)
{
	if (e->delta() > 0)
	{
		zoomIn();
		emit operationInfoSig(QString("Zoomed In: %1").arg(this->rho),1000);
	} else if (e->delta() < 0)
	{
		zoomOut();
		emit operationInfoSig(QString("Zoomed Out: %1").arg(this->rho),1000);
	}
}

void OpenGLWidget :: zoom(float z)
{
	this->rho+=z;
	update();
}

void OpenGLWidget :: setCamera(float theta, float fee)
{
	this->theta = theta;
	this->fee = fee;
	update();
}

void OpenGLWidget :: drawAxes()
{
	glBegin(GL_LINES);
	glColor3f(bgColor.redF(),1-bgColor.greenF(),1-bgColor.blueF());
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(1,0,0);
	for (int angle=0;angle<=360;angle+=10){
		glVertex3f(0.95,0.02*cos(angle*PI/180.),0.02*sin(angle*PI/180.));
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1-bgColor.redF(),bgColor.greenF(),1-bgColor.blueF());
	glVertex3f(0,0,0);
	glVertex3f(0,-1,0);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0,-1,0);
	for (int angle=0;angle<=360;angle+=10){
		glVertex3f(0.02*sin(angle*PI/180.),-0.95,0.02*cos(angle*PI/180.));
	}
	glEnd();


	glBegin(GL_LINES);
	glColor3f(1-bgColor.redF(),1-bgColor.greenF(),bgColor.blueF());
	glVertex3f(0,0,0);
	glVertex3f(0,0,1.2);
	glEnd();

	if (!show2D)
	{
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(0,0,1.2);
		for (int angle=0;angle<=360;angle+=10){
			glVertex3f(0.02*cos(angle*PI/180.),0.02*sin(angle*PI/180.),1.15);
		}
		glEnd();
	}
}

void OpenGLWidget :: toggle2D()
{
	show2D = !show2D;
	update();
}

void OpenGLWidget :: togglePeel() {
	peelEnable = !peelEnable;
	checkDepthPeel();
	emit updatePeelButton(peelEnable);
	update();
}

void OpenGLWidget :: processLibraryDelete(Cane* c)
{
	if (deleteButtonDown)
		emit acceptLibraryDelete(c);
}

bool OpenGLWidget :: is2D()
{
	return show2D;
}
