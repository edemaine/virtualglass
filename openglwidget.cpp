/*
This class is the QT GUI object that does 3D rendering.
It also responds to mouse clicks and mouse movement within
its extent. As this object handles mouse clicks and movement,
it is involved in modifying the cane.

Features:

DUAL RESOLUTION:
Two mesh resolutions are used to give an improved interaction
experience. A LOW_RESOLUTION mode is used when the cane is undergoing
motion (as determined by whether the user currently is holding
the mouse down), while a HIGH_RESOLUTION mode is used otherwise.
The idea is to give good framerate during movement in trade for
a higher resolution (as a lower resolution is not as noticeable for
moving objects).

MODES:
The OpenGLWidget object has a mode for each type of modification
the user can make to the cane (in addition to a LOOK_MODE, which
changes the orientation of the cane only). Changing modes is handled by
other parts of the GUI.
*/

#include "openglwidget.h"

OpenGLWidget :: OpenGLWidget(QWidget *parent, Model* model) : QGLWidget(parent)
{
	rightMouseDown = false;
	shiftButtonDown = false;
	showAxes = true;
	showGrid = false;
	resolution = HIGH_RESOLUTION;
	this->model = model;
	history = new CaneHistory();
	updateTriangles();
}

Model* OpenGLWidget :: getModel()
{
	return this->model;
}

void OpenGLWidget :: caneChangedSlot()
{
	updateTriangles();
	paintGL();
}

void OpenGLWidget :: setShiftButtonDown(bool state)
{
	shiftButtonDown = state;
}

void OpenGLWidget :: initializeGL()
{
	// Camera location
	theta = -PI/2.0;
	fee = PI/4.0;
	rho = 3.0;

	// Camera look-at location
	lookAtLoc[0] = lookAtLoc[1] = 0;
	lookAtLoc[2] = 0.5;

	// For shadow/lighting
	lightPosition[0] = 0.0;
	lightPosition[1] = 0.0;
	lightPosition[2] = 1000.0;
	lightPosition[3] = 0.0;
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// For transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
}

void OpenGLWidget :: setBgColor(QColor color)
{
	this->qglClearColor(color);
	bgColor = color;
	paintGL();
}

/*
Handles the drawing of a triangle mesh.
The triangles array is created and lives in the
Mesh object, and the OpenGLWidget object simply
receives a pointer to this array.
*/
void OpenGLWidget :: paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (showAxes)
		drawAxes();

	if (showGrid)
		drawGrid();

	if (geometry) {
		//JIM sez: blend not workin' out at the moment
		glDisable(GL_BLEND);

		//Check that Vertex and Triangle have proper size:
		assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3 + 4));
		assert(sizeof(Triangle) == sizeof(GLuint) * 3);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry->vertices[0].position));
		glNormalPointer(GL_FLOAT, sizeof(Vertex), &(geometry->vertices[0].normal));
		glColorPointer(4, GL_FLOAT, sizeof(Vertex), &(geometry->vertices[0].color));
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glDrawElements(GL_TRIANGLES, geometry->triangles.size() * 3, GL_UNSIGNED_INT, &(geometry->triangles[0].v1));

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	swapBuffers();
}

void OpenGLWidget :: switchProjectionCommandSlot()
{
	isOrthographic=!isOrthographic;
}

/*
Calls if the OpenGLWidget object is resized (in the GUI sense).
*/
void OpenGLWidget :: resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)width / (float)height, 0.01, 10.0);
	glMatrixMode(GL_MODELVIEW);
	updateCamera();
	paintGL();
}

void OpenGLWidget :: zoomIn()
{
	rho *= 0.8;
	updateCamera();
	paintGL();
}

void OpenGLWidget :: zoomOut()
{
	rho *= 1.2;
	updateCamera();
	paintGL();
}

void OpenGLWidget :: topViewCommandSlot()
{
	setCamera(0.0,0.01);
}

void OpenGLWidget :: sideViewCommandSlot()
{
	setCamera(0.0, PI/2);
}

void OpenGLWidget :: frontViewCommandSlot()
{
	setCamera(-PI/2, PI/2);
}

void OpenGLWidget :: toggleAxesCommandSlot()
{
	showAxes = !showAxes;
	paintGL();
}

void OpenGLWidget :: toggleGridCommandSlot()
{
	showGrid = !showGrid;
	paintGL();
}


/*
Is called when the resolution of the rendered cane is changed.
Two resolution modes are available: LOW_RESOLUTION and HIGH_RESOLUTION.
The method simply updates the `resolution' instance variable and
requests the (possibly new) resolution of the cane from the Mesh
object, updating its Triangle array pointer.
*/
void OpenGLWidget :: updateResolution(int new_resolution)
{
	resolution = new_resolution;
	updateTriangles();
}

/*
Should be called any time the Mesh object is changed by
modifying the cane it contains.
*/
void OpenGLWidget :: updateTriangles()
{
	geometry = model->getGeometry(resolution);
}

/*
Changes the current subcane undergoing change to
the next subcane in the cane.
*/
void OpenGLWidget :: advanceActiveSubcane()
{
	if (model->getMode() == BUNDLE_MODE)
	{
		model->advanceActiveSubcaneSlot();
		paintGL();
	}
}

/*
Called after the camera changes location.
*/
void OpenGLWidget :: updateCamera()
{
	eyeLoc[0] = lookAtLoc[0] + rho*sin(fee)*cos(theta);
	eyeLoc[1] = lookAtLoc[1] + rho*sin(fee)*sin(theta);
	eyeLoc[2] = lookAtLoc[2] + rho*cos(fee);
	glLoadIdentity();
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

	if (e->button() == Qt::RightButton){
		rightMouseDown = true;
	}

	// Change as part of dual mode feature
	updateResolution(LOW_RESOLUTION);

	paintGL();
}

/*
Currently catches all mouse release events
(left and right buttons, etc.).
*/
void OpenGLWidget :: mouseReleaseEvent (QMouseEvent* e)
{
	// Change as part of dual mode feature
	updateResolution(HIGH_RESOLUTION);
	if (e->button() == Qt::RightButton){
		rightMouseDown = false;
	}
	paintGL();
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

	windowWidth = this->width();
	windowHeight = this->height();

	// Calculate how much mouse moved
	oldMouseLocX = mouseLocX;
	mouseLocX = e->x();
	relX = (mouseLocX - oldMouseLocX) / windowWidth;
	oldMouseLocY = mouseLocY;
	mouseLocY = e->y();
	relY = (mouseLocY - oldMouseLocY) / windowHeight;

	/*
	Do something depending on mode.
	All modes except LOOK_MODE involve modifying the cane
	itself, while LOOK_MODE moves the camera.

	All of the calls to model->*Cane() are functions of relX/relY,
	but the constants involved are determined by experiment,
	i.e. how much twist `feels' reasonable for moving the mouse
	an inch.
	*/
	if (rightMouseDown)
	{
		// Rotate camera position around look-at location.

		theta -= (relX * 500.0 * PI / 180.0);
		newFee = fee - (relY * 500.0 * PI / 180.0);
		if (newFee > 0.0f && newFee < PI)
			fee = newFee;
		updateCamera();
		paintGL();
		return;
	}

	if (model->getMode() == LOOK_MODE)
	{
		// Rotate camera position around look-at location.
		theta -= (relX * 500.0 * PI / 180.0);
		newFee = fee - (relY * 500.0 * PI / 180.0);
		if (newFee > 0.0f && newFee < PI)
			fee = newFee;
		updateCamera();
	}
	else if (model->getMode() == PULL_MODE)
	{
		if (shiftButtonDown)
		{
			if (abs(relX) > abs(relY))
				model->pullCane(relX * PI, 0.0);
			else
				model->pullCane(0.0, -5.0*relY);
		}
		else
			model->pullCane(relX * PI, -5.0*relY);
	}
	else if (model->getMode() == BUNDLE_MODE)
	{
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
		model->moveCane(relX * cos(theta + PI / 2.0) + relY * cos(theta),
						relX * sin(theta + PI / 2.0) + relY * sin(theta));
	}
	else if (model->getMode() == FLATTEN_MODE)
	{
		model->flattenCane(relX, theta + PI / 2.0, -relY);
	}
}

void OpenGLWidget :: wheelEvent(QWheelEvent *e)
{
	if (e->delta() > 0)
	{
		zoomIn();
	} else if (e->delta() < 0)
	{
		zoomOut();
	}
}

/*

*/
void OpenGLWidget :: setCamera(float theta, float fee)
{
	this->theta = theta;
	this->fee = fee;
	updateCamera();
	paintGL();
}

Point OpenGLWidget :: getCameraPoint()
{

	Point result;
	result.z=rho*cos(fee);
	result.x=rho*cos(theta)*sin(fee);
	result.y=rho*sin(theta)*sin(fee);
	return result;
}

Vector3f OpenGLWidget :: getCameraDirection()
{
	Point camPoint = -getCameraPoint()/rho;
	return Vector3f(camPoint);
}


void OpenGLWidget :: saveObjFile(std::string const &filename)
{
	model->saveObjFile(filename);
}

/*

*/
void OpenGLWidget :: drawGrid()
{
	//return;
	glBegin(GL_LINES);
	glColor3f(1-bgColor.redF(),1-bgColor.greenF(),1-bgColor.blueF());
	float max=1;
	float resolution=10;
	for (float i=-max;i<=max;i+=max/resolution)
	{
		glVertex3f(i,0,-max);
		glVertex3f(i,0,max);
		glVertex3f(-max,0,i);
		glVertex3f(max,0,i);
	}
	glEnd();
}

/*

*/
void OpenGLWidget :: drawAxes()
{
	glBegin(GL_LINES);
	glColor3f(bgColor.redF(),1-bgColor.greenF(),1-bgColor.blueF());
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glColor3f(1-bgColor.redF(),bgColor.greenF(),1-bgColor.blueF());
	glVertex3f(0,0,0);
	glVertex3f(0,-1,0);
	glColor3f(1-bgColor.redF(),1-bgColor.greenF(),bgColor.blueF());
	glVertex3f(0,0,0);
	glVertex3f(0,0,1.2);
	glEnd();
}


