/*
This class is the QT GUI object that does 3D rendering.
It also responds to mouse clicks and mouse movement within
its extent. As this object handles mouse clicks and movement,
it is involved in modifying the cane.
*/

#include "niceviewwidget.h"


namespace {
void gl_errors(string const &where) {
	GLuint err;
	while ((err = glGetError()) != GL_NO_ERROR) {
	cerr << "(in " << where << ") OpenGL error #" << err
		 << ": " << gluErrorString(err) << endl;
	}
}
}



NiceViewWidget :: NiceViewWidget(int cameraMode, QWidget *parent) : QGLWidget(QGLFormat(QGL::AlphaChannel | QGL::DoubleBuffer | QGL::DepthBuffer), parent), peelRenderer(NULL)
{
	leftMouseDown = false;
	bgColor = QColor(200, 200, 200);
	geometry = NULL;
	this->cameraMode = cameraMode;

	switch (cameraMode)
	{
		case PULLPLAN_MODE:
			theta = -PI/2.0;
			phi = PI/2;
			rho = 11.0; 
			lookAtLoc[0] = 0.0;
			lookAtLoc[1] = 0.0;
			lookAtLoc[2] = 5.0;
			break;
		case PICKUPPLAN_MODE:
			theta = -PI/2.0;
			phi = PI/2;
			// rho set in resizeGL() b/c it depends on window size 
			lookAtLoc[0] = 0.0;
			lookAtLoc[1] = 0.0;
			lookAtLoc[2] = 0.0;
			break;
		case PIECE_MODE:
			theta = -PI/2.0;
			phi = PI/2;
			rho = 16.0; 
			lookAtLoc[0] = 0.0;
			lookAtLoc[1] = 0.0;
			lookAtLoc[2] = 0.0;
			break;
	}

	mouseLocX = 0;
	mouseLocY = 0;

	selectBuffer = NULL;

	initializeGLCalled = false;

	geometry = NULL;
}

NiceViewWidget :: ~NiceViewWidget()
{
	//Deallocate all the depth peeling resources we may have created:
	makeCurrent();

	if (selectBuffer) {
		delete selectBuffer;
		selectBuffer = NULL;
	}

	if (peelRenderer) {
		GLEWContext *ctx = peelRenderer->glewContext;
		delete peelRenderer;
		peelRenderer = NULL;
		delete ctx;
	}
}

void NiceViewWidget :: initializeGL()
{
	initializeGLCalled = true;
	assert(!peelRenderer);
	// set up glew:
	GLEWContext *glewContext = new GLEWContext;
#define glewGetContext() glewContext
	GLenum err = glewInit();
#undef glewGetContext
	if (err != GLEW_OK) {
		std::cerr << "WARNING: Failure initializing glew: " << glewGetErrorString(err) << std::endl;
		std::cerr << " ... we will continue, but code that uses extensions will cause a crash" << std::endl;
		delete glewContext;
		glewContext = NULL;
	} else {
		try {
			peelRenderer = new PeelRenderer(glewContext);
		} catch (...) {
			std::cerr << "Caught exception constructing peelRenderer, will fall back to regular rendering." << std::endl;
			peelRenderer = NULL;
			delete glewContext;
			glewContext = NULL;
		}
	}
	// For shadow/lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	gl_errors("NiceViewWidget::initializeGL");
}

QImage NiceViewWidget :: renderImage() {
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


/*
Handles the drawing of a triangle mesh.
The triangles array is created and lives in the
Mesh object, and the NiceViewWidget object simply
receives a pointer to this array.
*/
void NiceViewWidget :: paintGL()
{
	setGLMatrices();

	if (peelRenderer && geometry) {
		peelRenderer->render(make_vector< float >(bgColor.redF(), bgColor.greenF(), bgColor.blueF()), *geometry);
	} else {
		this->qglClearColor(bgColor);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		paintWithoutDepthPeeling();
	}
}

void NiceViewWidget :: paintWithoutDepthPeeling()
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	if (geometry) {
		//glEnable(GL_LIGHTING);
		//Check that Vertex and Triangle have proper size:
		assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3));
		assert(sizeof(Triangle) == sizeof(GLuint) * 3);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry->vertices[0].position));
		glNormalPointer(GL_FLOAT, sizeof(Vertex), &(geometry->vertices[0].normal));
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		for (std::vector< Group >::const_iterator g = geometry->groups.begin(); g != geometry->groups.end(); ++g) {
			assert(g->color);
			Color* c = g->color;
			glColor4f(c->r, c->g, c->b, c->a);
			glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
				GL_UNSIGNED_INT, &(geometry->triangles[g->triangle_begin].v1));
		}

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		//glDisable(GL_LIGHTING);
	}

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	//called automatically: swapBuffers();
}


/*
Calls if the NiceViewWidget object is resized (in the GUI sense).
*/
void NiceViewWidget :: resizeGL(int width, int height)
{
	if (this->cameraMode == PICKUPPLAN_MODE)
	{
		rho = 11.5;	
		if (width < height)
		{
			rho *= height;
			rho /= width;
		}
	}

	glViewport(0, 0, width, height);
	if (this->width() != width || this->height() != height) {
		std::cerr << "resizeGL(" << width << ", " << height << ") called while this->width,height are (" << this->width() << ", " << this->height() << "). This may mess up aspect ratio." << std::endl;
	}
	//I think paintGL will get called now...
}

void NiceViewWidget :: zoomIn()
{
	rho *= 0.8;
	update();
}

void NiceViewWidget :: zoomOut()
{
	rho *= 1.2;
	update();
}


/*
Called to set up projection and modelview matrices
*/
void NiceViewWidget :: setGLMatrices()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLint viewport[4] = {0,0,0,0};
	glGetIntegerv(GL_VIEWPORT, viewport);

	float w = viewport[2];
	float h = viewport[3];

	if (cameraMode == PIECE_MODE)
	{
		gluPerspective(45.0, w / h, 0.01, 100.0);
	}
	else if (cameraMode == PULLPLAN_MODE) 
	{
		float a = h / w;
		float s = 2.2f / rho;
		glScalef(a * s, s,-0.01);
	}
	else // pickup plan mode
	{
		float a = h / w;
		float s = 2.2f / rho;
		glScalef(a * s, s,-0.01);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector3f eyeLoc = eyePosition();

	gluLookAt(eyeLoc[0], eyeLoc[1], eyeLoc[2],
		  lookAtLoc[0], lookAtLoc[1], lookAtLoc[2],
		  0.0, 0.0, 1.0);
}

/*
Currently catches all mouse press events
(left and right buttons, etc.).
*/
void NiceViewWidget :: mousePressEvent (QMouseEvent* e)
{
	// Update instance variables for mouse location
	mouseLocX = e->x();
	mouseLocY = e->y();

	if (e->button() == Qt::LeftButton)
	{
		leftMouseDown = true;
	}
}

void NiceViewWidget :: setGeometry(Geometry* g)
{
	geometry = g;
	update();
}

Vector3f NiceViewWidget :: eyePosition()
{
	Vector3f loc;
	loc.x = lookAtLoc[0] + rho*sin(phi)*cos(theta);
	loc.y = lookAtLoc[1] + rho*sin(phi)*sin(theta);
	loc.z = lookAtLoc[2] + rho*cos(phi);
	return loc;
}

/*
Currently catches all mouse release events
(left and right buttons, etc.).
*/
void NiceViewWidget :: mouseReleaseEvent (QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
		leftMouseDown = false;
}

/*
This method is an event handler called when the mouse
is moved *and a button is down*. Depending on the mode,
the cane (and view of the cane) changes. This is
part of the mode feature.
*/
void NiceViewWidget :: mouseMoveEvent (QMouseEvent* e)
{
	float relX, relY;
	float windowWidth, windowHeight;
	int oldMouseLocX, oldMouseLocY;

	windowWidth = this->width();
	windowHeight = this->height();

	// Calculate how much mouse moved
	oldMouseLocX = mouseLocX;
	mouseLocX = e->x();
	relX = (mouseLocX - oldMouseLocX) / windowWidth;
	oldMouseLocY = mouseLocY;
	mouseLocY = e->y();
	relY = (mouseLocY - oldMouseLocY) / windowHeight;

	if (cameraMode == PICKUPPLAN_MODE)
		return;

	if (leftMouseDown)
	{
		theta -= (relX * 100.0 * PI / 180.0);
		if (cameraMode == PIECE_MODE)
			phi = MIN(PI-0.0001, MAX(0.0001, phi - (relY * 100.0 * PI / 180.0)));
		update();
	}

}

void NiceViewWidget :: wheelEvent(QWheelEvent *e)
{
	if (cameraMode == PICKUPPLAN_MODE || cameraMode == PULLPLAN_MODE)
		return;

	if (e->delta() > 0)
	{
		zoomIn();
	}
	else if (e->delta() < 0)
	{
		zoomOut();
	}
}

void NiceViewWidget :: zoom(float z)
{
	if (cameraMode == PICKUPPLAN_MODE)
		return;

	this->rho+=z;
	update();
}
