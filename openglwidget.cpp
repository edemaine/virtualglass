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

OpenGLWidget :: OpenGLWidget(QWidget *parent, Model* _model) : QGLWidget(parent)
{
	//history = new CaneHistory();

	shiftButtonDown = false;
	rightMouseDown = false;
	controlButtonDown = false;
	deleteButtonDown = false;
	isOrthographic = false;

	bgColor = QColor(0,0,0);

	tableForm = NULL;
	stretchInput = NULL;
	twistInput = NULL;
	flattenInput = NULL;
	rectangleInput = NULL;

	model = _model;
	geometry = NULL;
	resolution = HIGH_RESOLUTION;

	showAxes = true;
	showGrid = false;
	lockView = false;

	lookAtLoc[0] = 0.0f;
	lookAtLoc[1] = 0.0f;
	lookAtLoc[2] = 0.5f;

	theta = -PI/2.0;
	fee = PI/4.0;
	rho = 3.0;

	mouseLocX = 0;
	mouseLocY = 0;

	setMouseTracking(true);
	updateTriangles();
}

Model* OpenGLWidget :: getModel()
{
	return this->model;
}

void OpenGLWidget :: caneChanged()
{
	updateTriangles();
	update();
	model->getHistory()->setBusy(false);
}

void OpenGLWidget :: setShiftButtonDown(bool state)
{
	shiftButtonDown = state;
}

void OpenGLWidget :: setControlButtonDown(bool state)
{
	controlButtonDown = state;
}

void OpenGLWidget :: setDeleteButtonDown(bool state)
{
	deleteButtonDown = state;
}

void OpenGLWidget :: initializeGL()
{
	// For shadow/lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc (GL_LEQUAL, 0.5);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
}

void OpenGLWidget :: setBgColor(QColor color)
{
	this->qglClearColor(color);
	bgColor = color;
	update();
}

int OpenGLWidget :: getSubcaneUnderMouse(int mouseX, int mouseY)
{
	makeCurrent();
	setGLMatrices();

	geometry = model->getGeometry(LOW_RESOLUTION);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Check that Vertex and Triangle have proper size:
	assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3));
	assert(sizeof(Triangle) == sizeof(GLuint) * 3);

	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry->vertices[0].position));
	glEnableClientState(GL_VERTEX_ARRAY);
	for (std::vector< Group >::const_iterator g = geometry->groups.begin(); g != geometry->groups.end(); ++g) {
		glColor4ubv(reinterpret_cast< const GLubyte * >(&(g->tag)));
		glDrawElements(GL_TRIANGLES, g->size * 3,
					   GL_UNSIGNED_INT, &(geometry->triangles[g->begin].v1));
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	uint32_t c = 0;
	glReadPixels(mouseX, this->height() - mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &c);

	glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);

	updateTriangles();

	return (int)c;
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

	//return result;
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
	glDisable(GL_LIGHTING);

	if (showAxes)
		drawAxes();

	if (showGrid)
		drawGrid();

	drawSnaps();

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
			glColor3f(c.r, c.g, c.b);
			glColor4f(c.r, c.g, c.b, c.a);
			glDrawElements(GL_TRIANGLES, g->size * 3,
						   GL_UNSIGNED_INT, &(geometry->triangles[g->begin].v1));
		}

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		glDisable(GL_LIGHTING);
	}

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

void OpenGLWidget :: drawSnapPoints()
{

	float max=0.04;
	for (int i=0;i<=model->snapPointCount(SNAP_POINT);i++)
	{
		glPushMatrix();
		glTranslatef(model->snapPoint(SNAP_POINT,i).x,model->snapPoint(SNAP_POINT,i).y,0);
		for (int j=1;j<=5;j++)
		{
			glColor3f((1-bgColor.redF())/j,(1-bgColor.greenF())/j,(1-bgColor.blueF())/j);
			//glColor3f(1.0/j,1.0/j,1.0/j);
			glBegin(GL_LINES);
			glVertex3f(0,0,max*(j-1));
			glVertex3f(0,0,max*j);
			glEnd();

		}

		glColor3f((1-bgColor.redF())*3/4,(1-bgColor.greenF())*3/4,(1-bgColor.blueF())*3/4);
		//glColor3f(0.75,0.75,0.75);
		drawCircle(model->snapPointRadius(SNAP_POINT,i));

		glPopMatrix();
	}

}

void OpenGLWidget :: drawSnapCircles()
{

	float max=0.015;
	for (int i=0;i<=model->snapPointCount(SNAP_CIRCLE);i++)
	{
		glPushMatrix();
		glTranslatef(model->snapPoint(SNAP_CIRCLE,i).x,model->snapPoint(SNAP_CIRCLE,i).y,0);
		for (int j=1;j<=5;j++)
		{
			glColor3f((1-bgColor.redF())/j,(1-bgColor.greenF())/j,(1-bgColor.blueF())/j);
			//glColor3f(1.0/j,1.0/j,1.0/j);
			glBegin(GL_LINES);
			glVertex3f(0,0,max*(j-1));
			glVertex3f(0,0,max*j);
			glEnd();

		}
		glColor3f((1-bgColor.redF())*3/4,(1-bgColor.greenF())*3/4,(1-bgColor.blueF())*3/4);
		//glColor3f(0.75,0.75,0.75);

		drawCircle(model->snapPointRadius(SNAP_CIRCLE,i));
		drawCircle(model->snapPointRadius(SNAP_CIRCLE,i)*0.7);
		drawCircle(model->snapPointRadius(SNAP_CIRCLE,i)*1.3);

		glPopMatrix();
	}

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

void OpenGLWidget :: drawSnapLines()
{

	float max=0.015;
	for (int i=0;i<=model->snapPointCount(SNAP_LINE);i++)
	{
		glPushMatrix();
		glTranslatef(model->snapPoint(SNAP_LINE,i).x,model->snapPoint(SNAP_LINE,i).y,0);
		for (int j=1;j<=5;j++)
		{
			glColor3f((1-bgColor.redF())/j,(1-bgColor.greenF())/j,(1-bgColor.blueF())/j);
			//glColor3f(1.0/j,1.0/j,1.0/j);
			glBegin(GL_LINES);
			glVertex3f(0,0,max*(j-1));
			glVertex3f(0,0,max*j);
			glEnd();

		}
		glPopMatrix();

		glPushMatrix();
		glTranslatef(model->snapPoint2(SNAP_LINE,i).x,model->snapPoint2(SNAP_LINE,i).y,0);
		for (int j=1;j<=5;j++)
		{
			glColor3f((1-bgColor.redF())/j,(1-bgColor.greenF())/j,(1-bgColor.blueF())/j);
			//glColor3f(1.0/j,1.0/j,1.0/j);
			glBegin(GL_LINES);
			glVertex3f(0,0,max*(j-1));
			glVertex3f(0,0,max*j);
			glEnd();

		}
		glPopMatrix();

		glColor3f((1-bgColor.redF())*3/4,(1-bgColor.greenF())*3/4,(1-bgColor.blueF())*3/4);
		//glColor3f(0.75,0.75,0.75);
		Point p1 = model->snapPoint(SNAP_LINE,i);
		Point p2 = model->snapPoint2(SNAP_LINE,i);
		drawSegment(p1,p2);
		Point v = p2-p1;
		v = p2-p1;
		v.z = -v.x;
		v.x = v.y;
		v.y = v.z;
		v.z = 0;
		Point dist=normalize(v)*0.15;
		drawSegment(p1+dist,p2+dist);
		drawSegment(p1-dist,p2-dist);
	}

}

void OpenGLWidget :: drawSnaps()
{
	drawSnapPoints();
	drawSnapLines();
	drawSnapCircles();
}

void OpenGLWidget :: switchProjection()
{
	isOrthographic=!isOrthographic;
	update();
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

void OpenGLWidget :: toggleGrid()
{
	setGrid(!showGrid);
}

void OpenGLWidget :: setAxes(bool show)
{
	showAxes = show;
	update();
}

void OpenGLWidget :: setGrid(bool show)
{
	showGrid = show;
	update();
}

void OpenGLWidget :: lockTable()
{
	lockView = !lockView;
	update();
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
Called to set up projection and modelview matrices
*/
void OpenGLWidget :: setGLMatrices()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float w = width();
	float h = height();
	if (isOrthographic) {
		if (w > h) {
			float a = h / w;
			float s = 1.0f / rho;
			glScalef(a * s, s,-0.01);
		} else {
			float a = w / h;
			float s = 1.0f / rho;
			glScalef(s, a * s,-0.01);
		}
	} else {
		gluPerspective(45.0, w / h, 0.01, 100.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float eyeLoc[3];
	eyeLoc[0] = lookAtLoc[0] + rho*sin(fee)*cos(theta);
	eyeLoc[1] = lookAtLoc[1] + rho*sin(fee)*sin(theta);
	eyeLoc[2] = lookAtLoc[2] + rho*cos(fee);
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
		rightMouseDown = true;
	} else
	{
		model->setActiveSubcane(getSubcaneUnderMouse(mouseLocX, mouseLocY));
		if (deleteButtonDown)
		{
			if (!model->deleteActiveCane())
			{
				if (model->getMode() == SNAP_MODE || model->getMode() == SNAP_CIRCLE_MODE || model->getMode() == SNAP_LINE_MODE)
				model->deleteSnapPoint(getClickedPlanePoint(mouseLocX,mouseLocY));
			}
		}
		else if (model->getMode() == SNAP_MODE)
		{
			Point p = getClickedPlanePoint(mouseLocX,mouseLocY);
			model->addSnapPoint(SNAP_POINT,p);
			emit operationInfoSig(QString("Snap Point: %1, %2").arg(p.x).arg(p.y),2000);
		} else if (model->getMode() == SNAP_LINE_MODE)
		{
			Point p = getClickedPlanePoint(mouseLocX,mouseLocY);
			model->addSnapPoint(SNAP_LINE,p);
			emit operationInfoSig(QString("Snap Line: %1, %2").arg(p.x).arg(p.y),2000);
		} else if (model->getMode() == SNAP_CIRCLE_MODE)
		{
			Point p = getClickedPlanePoint(mouseLocX,mouseLocY);
			model->addSnapPoint(SNAP_CIRCLE,p);
			emit operationInfoSig(QString("Snap Circle: %1, %2").arg(p.x).arg(p.y),2000);
		}
	}
	// Change as part of dual mode feature
	updateResolution(LOW_RESOLUTION);

	update();
}

/*
Currently catches all mouse release events
(left and right buttons, etc.).
*/
void OpenGLWidget :: mouseReleaseEvent (QMouseEvent* e)
{
	// Change as part of dual mode feature
	updateResolution(HIGH_RESOLUTION);

	//check if cane is in a snap, and finalize it if true
	if (model->getActiveSubcane()!=-1)
	{
		if (model->getActiveSnapMode()!=NO_SNAP)
		{
			// do stuff!
			Point p,loc,p1,p2,dist,a,b;
			switch(model->getActiveSnapMode())
			{
			case SNAP_POINT:
				p=model->snapPoint(SNAP_POINT,model->getActiveSnapIndex());
				//model->moveCaneTo(p.x,p.y);
				break;
			case SNAP_LINE:
				loc=model->getCane()->subcaneLocations[model->getActiveSubcane()];
				p1 = model->snapPoint(SNAP_LINE,model->getActiveSnapIndex());
				p2 = model->snapPoint2(SNAP_LINE,model->getActiveSnapIndex());
				a = loc-p1;
				b = p2-p1;
				p = (a*b/(b*b))*b + p1;
				//model->moveCaneTo(p.x,p.y);
				break;
			case SNAP_CIRCLE:
				loc=model->getCane()->subcaneLocations[model->getActiveSubcane()];
				p=model->snapPoint(SNAP_CIRCLE,model->getActiveSnapIndex());
				dist=loc-p;
				dist = dist*model->snapPointRadius(SNAP_CIRCLE,model->getActiveSnapIndex())/length(dist) + p;
				//model->moveCaneTo(dist.x,dist.y);
				break;
			}
			model->clearActiveSnap(false);
		}
		//check if cane is in a snap, and finalize it if true
		model->setActiveSubcane(-1);
	}

	if (e->button() == Qt::RightButton){
		rightMouseDown = false;
	} else if (model->getMode() == SNAP_MODE || model->getMode() == SNAP_LINE_MODE || model->getMode() == SNAP_CIRCLE_MODE)
	{
		Point p = model->finalizeSnapPoint();
		emit operationInfoSig(QString("Snap Point: %1, %2").arg(p.x).arg(p.y),2000);
	}
	update();
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

	//Point planePoint = getClickedPoint(oldMouseLocX,oldMouseLocY);
	//emit operationInfoSig(QString("Plane Point: %1, %2, %3").arg(planePoint.x).arg(planePoint.y).arg(planePoint.z),2000);

	/*
	Do something depending on mode.
	All modes except LOOK_MODE involve modifying the cane
	itself, while LOOK_MODE moves the camera.

	All of the calls to model->*Cane() are functions of relX/relY,
	but the constants involved are determined by experiment,
	i.e. how much twist `feels' reasonable for moving the mouse
	an inch.
	*/
	if (rightMouseDown && !lockView)
	{
		// Rotate camera position around look-at location.

		theta -= (relX * 500.0 * PI / 180.0);
		newFee = fee - (relY * 500.0 * PI / 180.0);
		if (newFee > 0.0f && newFee < PI)
			fee = newFee;
		update();
		return;
	}


	switch (model->getMode())
	{
	case LOOK_MODE:
		if (!lockView)
		{
			// Rotate camera position around look-at location.
			theta -= (relX * 500.0 * PI / 180.0);
			newFee = fee - (relY * 500.0 * PI / 180.0);
			if (newFee > 0.0f && newFee < PI)
				fee = newFee;
			update();
		}
		break;
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
			if (controlButtonDown)
			{
				model->pullActiveCane(relX * PI, -5.0*relY);
			}
			else
			{
				model->pullCane(relX * PI, -5.0*relY);
			}
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

		//model->setActiveSubcane(getSubcaneUnderMouse(oldMouseLocX, oldMouseLocY));
		if (e->buttons() & 0x00000001) // if left mouse button is down
		{
			if (shiftButtonDown)
			{
				relY *=5;
				model->moveCane(-relY);
			} else {
				relX *= 5; // tone it down
				relY *= 5; // tone it down
				model->moveCane(relX * cos(theta + PI / 2.0) + relY * cos(theta),
								relX * sin(theta + PI / 2.0) + relY * sin(theta));
			}
		}
		if (model->getActiveSubcane() != -1 && model->getCane()) {
			emit operationInfoSig(QString("Moved X Direction %1, Y Direction %2").arg(model->getCane()->subcaneLocations[model->getActiveSubcane()].x).arg(model->getCane()->subcaneLocations[model->getActiveSubcane()].y),1000);
		}
		break;
	case CASING_MODE:
		model->changeCaneCasing(-relX);
		break;
	case FLATTEN_MODE:
		if (controlButtonDown)
		{
			model->flattenActiveCane(relX, theta + PI / 2.0, -relY);
		}
		else
		{
			model->flattenCane(relX, theta + PI / 2.0, -relY);
		}
		emit operationInfoSig(QString("Squished with %1, Flattened into rectangle with %2").arg(model->getCane()->amts[0]).arg(model->getCane()->amts[2]),1000);
		break;
	case SNAP_MODE:
	case SNAP_LINE_MODE:
	case SNAP_CIRCLE_MODE:
		{
			Point p = getClickedPlanePoint(mouseLocX,mouseLocY);
			model->modifySnapPoint(p);
			//emit operationInfoSig(QString("Snap Point: %1, %2").arg(p.x).arg(p.y),2000);
			update();
			break;
		}
	default:
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

/*

*/
void OpenGLWidget :: setCamera(float theta, float fee)
{
	this->theta = theta;
	this->fee = fee;
	update();
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

void OpenGLWidget :: saveRawFile(std::string const &filename)
{
	model->saveRawFile(filename);
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
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(1,0,0);
	for (int angle=0;angle<=360;angle+=10){
		glVertex3f(0.95,0.05*cos(angle*PI/180.),0.05*sin(angle*PI/180.));
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
		glVertex3f(0.05*sin(angle*PI/180.),-0.95,0.05*cos(angle*PI/180.));
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1-bgColor.redF(),1-bgColor.greenF(),bgColor.blueF());
	glVertex3f(0,0,0);
	glVertex3f(0,0,1.2);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0,0,1.2);
	for (int angle=0;angle<=360;angle+=10){
		glVertex3f(0.05*cos(angle*PI/180.),0.05*sin(angle*PI/180.),1.15);
	}
	glEnd();
}

void OpenGLWidget::processPull()
{
	model->pullCane(twistInput->value(),stretchInput->value());
	emit operationInfoSig(QString("Twisted %1 Revolutions Per Viewable Length, Pulled %1").arg(model->getCane()->amts[0] / PI / 2,model->getCane()->amts[1]),1000);
}

void OpenGLWidget::processFlatten()
{
	model->flattenCane(rectangleInput->value(), theta + PI / 2.0, flattenInput->value());
	emit operationInfoSig(QString("Squished with %1, Flattened into rectangle with %1").arg(model->getCane()->amts[0],model->getCane()->amts[2]),1000);
}

void OpenGLWidget::exactInput()
{
	if (model->getMode() == PULL_MODE)
	{
		QDialog* tableBox = new QDialog(NULL);
		tableForm = new QFormLayout(tableBox->window());
		stretchInput = new QDoubleSpinBox;//= new QLineEdit;
		tableForm->addRow("Stretch Factor",stretchInput);
		twistInput = new QDoubleSpinBox;
		tableForm->addRow("Twist Factor",twistInput);

		QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		connect(buttons,SIGNAL(accepted()),tableBox,SLOT(accept()));
		connect(buttons,SIGNAL(accepted()),this,SLOT(processPull()));
		connect(buttons,SIGNAL(rejected()),tableBox,SLOT(reject()));

		tableForm->addRow(buttons);

		tableBox->setLayout(tableForm);

		tableBox->exec();
	} else if (model->getMode() == FLATTEN_MODE)
	{
		QDialog* tableBox = new QDialog(NULL);
		tableForm = new QFormLayout(tableBox->window());
		rectangleInput = new QDoubleSpinBox;
		tableForm->addRow("Rectangular Factor",rectangleInput);
		flattenInput = new QDoubleSpinBox;
		tableForm->addRow("Flatten Factor",flattenInput);

		QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		connect(buttons,SIGNAL(accepted()),tableBox,SLOT(accept()));
		connect(buttons,SIGNAL(accepted()),this,SLOT(processFlatten()));
		connect(buttons,SIGNAL(rejected()),tableBox,SLOT(reject()));

		tableForm->addRow(buttons);

		tableBox->setLayout(tableForm);

		tableBox->exec();
	}
}

