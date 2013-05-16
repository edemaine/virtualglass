
// This class is the QT GUI object that does 3D rendering.
// It also responds to mouse clicks and mouse movement within
// its extent, which serve to change the camera view. 

#include "glew.h"
#ifdef _WIN32
#  include <windows.h>
#endif
#include <QMouseEvent>

#include "constants.h"
#include "niceviewwidget.h"
#include "globaldepthpeelingsetting.h"
#include "globalbackgroundcolor.h"
#include "glassopengl.h"

NiceViewWidget :: NiceViewWidget(enum CameraMode cameraMode, QWidget *parent) 
	: QGLWidget(QGLFormat(QGL::AlphaChannel | QGL::DoubleBuffer | QGL::DepthBuffer | QGL::SampleBuffers), parent), peelRenderer(NULL)
{
	this->setAttribute(Qt::WA_AcceptTouchEvents);

	leftMouseDown = false;

	geometry = NULL;
	this->cameraMode = cameraMode;

	switch (cameraMode)
	{
		case PULLPLAN_CAMERA_MODE:
			theta = -PI/2.0;
			phi = PI/2;
			rho = 11.0; 
			lookAtLoc[0] = 0.0;
			lookAtLoc[1] = 0.0;
			lookAtLoc[2] = 5.0;
			break;
		case PICKUPPLAN_CAMERA_MODE:
			theta = -PI/2.0;
			phi = PI/2;
			// rho set in resizeGL() b/c it depends on window size 
			lookAtLoc[0] = 0.0;
			lookAtLoc[1] = 0.0;
			lookAtLoc[2] = 0.0;
			break;
		case PIECE_CAMERA_MODE:
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

	initializeGLCalled = false;
}

NiceViewWidget :: ~NiceViewWidget()
{
	//Deallocate all the depth peeling resources we may have created:
	makeCurrent();

	if (peelRenderer) 
	{
		GLEWContext *ctx = peelRenderer->glewContext;
		delete peelRenderer;
		peelRenderer = NULL;
		delete ctx;
	}
}

void NiceViewWidget :: initializePeel()
{
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
	} else {
		try {
			peelRenderer = new PeelRenderer(glewContext);
		} catch (...) {
			std::cerr << "Caught exception constructing peelRenderer, will fall back to regular rendering." << std::endl;
			peelRenderer = NULL;
			delete glewContext;
		}
	}
} 

void NiceViewWidget :: initializeGL()
{
	if (initializeGLCalled)
		return;

	initializeGLCalled = true;

	GlassOpenGL::initialize();
	initializePeel();

	GlassOpenGL::errors("NiceViewWidget::initializeGL");
}

// Handles the drawing of a triangle mesh.
void NiceViewWidget :: paintGL()
{
	if (!geometry) 
		return;

	setGLMatrices();

	if (peelRenderer && GlobalDepthPeelingSetting::enabled())
		peelRenderer->render(*geometry);
	else 
		GlassOpenGL::renderWithoutDepthPeeling(*geometry);
}

// Called if the NiceViewWidget object is resized (in the GUI sense).
void NiceViewWidget :: resizeGL(int width, int height)
{
	if (this->cameraMode == PICKUPPLAN_CAMERA_MODE)
	{
		rho = 11.5;
		if (width < height)
		{
			rho *= height;
			rho /= width;
		}
	}

	glViewport(0, 0, width, height);
	if (this->width() != width || this->height() != height) 
	{
		std::cerr << "resizeGL(" << width << ", " << height << ") called while this->width,height are (" 
			<< this->width() << ", " << this->height() << "). This may mess up aspect ratio." << std::endl;
	}
}

void NiceViewWidget :: setGLMatrices()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLint viewport[4] = {0,0,0,0};
	glGetIntegerv(GL_VIEWPORT, viewport);

	float w = viewport[2];
	float h = viewport[3];

	switch (cameraMode)
	{
		case PIECE_CAMERA_MODE:
			gluPerspective(45.0, w / h, 0.1, 100.0);
			break;
		case PULLPLAN_CAMERA_MODE:
		case PICKUPPLAN_CAMERA_MODE:
			float a = h / w;
			float s = 2.2f / rho;
			glScalef(a * s, s, -0.01);
			break;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector3f eyeLoc = eyePosition();

	gluLookAt(eyeLoc[0], eyeLoc[1], eyeLoc[2],
		  lookAtLoc[0], lookAtLoc[1], lookAtLoc[2],
		  0.0, 0.0, 1.0);
}


bool NiceViewWidget :: event(QEvent* event)
{
	switch (event->type())
	{
		case QEvent::MouseButtonPress:
		{
			QMouseEvent* e = dynamic_cast<QMouseEvent*>(event);
			
			// In pickup plan mode, user does not move camera location, zoom, etc.
			// The widget is a passive `display' widget only, with an interactive layer
			// on top of it (PickupPlanEditorViewWidget), which we pass the event up to.
			if (cameraMode == PICKUPPLAN_CAMERA_MODE)
			{
				e->ignore(); 
				return true; 	
			}

			// Update instance variables for mouse location
			mouseLocX = e->x();
			mouseLocY = e->y();

			if (e->button() == Qt::LeftButton)
				leftMouseDown = true;
			return true;
		}
		case QEvent::MouseMove:
		{
			QMouseEvent* e = dynamic_cast<QMouseEvent*>(event);

			float relX, relY;
			int oldMouseLocX, oldMouseLocY;

			// Calculate how much mouse moved
			oldMouseLocX = mouseLocX;
			mouseLocX = e->x();
			relX = (mouseLocX - oldMouseLocX) / static_cast<float>(this->width());
			oldMouseLocY = mouseLocY;
			mouseLocY = e->y();
			relY = (mouseLocY - oldMouseLocY) / static_cast<float>(this->height());

			if (cameraMode == PICKUPPLAN_CAMERA_MODE)
				return true;

			if (leftMouseDown)
			{
				theta -= (relX * 100.0 * PI / 180.0);
				if (cameraMode == PIECE_CAMERA_MODE)
					phi = MIN(PI-0.0001, MAX(0.0001, phi - (relY * 100.0 * PI / 180.0)));
				update();
			}
			
			return true;
		}	
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent* e = dynamic_cast<QMouseEvent*>(event);

			if (e->button() == Qt::LeftButton)
				leftMouseDown = false;

			return true;
		}	
		case QEvent::Wheel:
		{
			QWheelEvent* e = dynamic_cast<QWheelEvent*>(event);

			switch (cameraMode)
			{
				case PULLPLAN_CAMERA_MODE:
					if (e->delta() > 0)
						rho *= 0.8;
					else if (e->delta() < 0)
					{
						if (rho*1.2 > 11.0)
							rho=11.0;
						else
							rho *= 1.2;
					}
					break;
				case PICKUPPLAN_CAMERA_MODE:
					break;	
				case PIECE_CAMERA_MODE:
				default:
					if (e->delta() > 0)
						rho *= 0.8;
					else if (e->delta() < 0)
						rho *= 1.2;
					break;
			}
			update();	

			return true;
		}
		case QEvent::TouchBegin:
		case QEvent::TouchUpdate:
		case QEvent::TouchEnd:
		{
			// this code comes largely from the Qt example at 
			// https://qt-project.org/doc/qt-4.8/touch-pinchzoom-graphicsview-cpp.html
			QTouchEvent* e = dynamic_cast<QTouchEvent*>(event);
			QList<QTouchEvent::TouchPoint> touchPoints = e->touchPoints();
			if (touchPoints.count() == 2) 
			{
				// determine scale factor
				const QTouchEvent::TouchPoint& tp1 = touchPoints.first();
				const QTouchEvent::TouchPoint& tp2 = touchPoints.last();
				rho *= QLineF(tp1.pos(), tp2.pos()).length() / QLineF(tp1.startPos(), tp2.startPos()).length();
				//if (e->touchPointStates() & Qt::TouchPointReleased) 
				//{
				//	totalScaleFactor *= currentScaleFactor;
				//	currentScaleFactor = 1;
				//}
			}
			return true;
		}
		default:
			return QGLWidget::event(event);
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


