/*
This class is the QT GUI object that does 3D rendering.
It also responds to mouse clicks and mouse movement within
its extent.

The actual computation of the cane mesh is handled by the 
Mesh object defined in mesh.h/cpp. This class only requests
a pointer to the mesh and draws it using normal OpenGL calls.

As this object handles mouse clicks and movement, it is 
involved in modifying the cane. For example, if the user
is in twist mode and moves the mouse horizontally, this
object tells the mesh object ``Hey, the cane has been twisted
by amount X''. 

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
other parts of the GUI, and a changed mode is indicated to the 
OpenGLWidget object by calling setMode(). Depending upon the mode,
mouse movement/clicks cause different behavior. For example, 
dragging the mouse horizontally while in TWIST_MODE causes the cane
to become twisted (left for CW, right for CCW), while moving the mouse
vertically in stretch mode causes the cane to stretch (up for increased
stretch, down for decreased).
*/

#include "openglwidget.h" 
 
OpenGLWidget :: OpenGLWidget(QWidget *parent=0) : QGLWidget(parent)
{
        resolution = HIGH_RESOLUTION;
        mode = LOOK_MODE;  
        mesh = new Mesh(NULL);
        updateTriangles();
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
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
}

/*
Resets the cane currently being operated on to ``nothing''.
This immediately causes the rendering to be updated to a blank
region (no triangles exist to be drawn).
*/ 
void OpenGLWidget :: zeroCanes()
{
        mesh->setCane(NULL);
        updateTriangles();
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
        int i;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        for (i = 0; i < num_triangles; ++i)
        {
                drawTriangle(&(triangles[i]));
        }
        glEnd();

        swapBuffers();  
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

/*
Sets the cane currently being interacted with to
a new cane. This is achieved by passing the cane off
to the Mesh object, getting a pointer back to a Triangle
array containing the mesh, and drawing it.
*/
void OpenGLWidget :: setFocusCane(Cane* c)
{
        mesh->setCane(c);
        updateTriangles();
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
        triangles = mesh->getMesh(resolution);
        num_triangles = mesh->getNumMeshTriangles(resolution);
} 

/*
Updates the mode. See constants.h for the various
modes available (named *_MODE).
*/
void OpenGLWidget :: setMode(int m)
{
        this->mode = m;

        // Special call when entering BUNDLE_MODE
        // because the mode causes the cane to
        // change appearance (subcane illumination, etc.).
        if (mode == BUNDLE_MODE)
        {
                mesh->startMoveMode(); 
        }
}

/*
Changes the current subcane undergoing change to
the next subcane in the cane.
*/
void OpenGLWidget :: advanceActiveSubcane()
{
        if (mode == BUNDLE_MODE)
        {
                mesh->advanceActiveSubcane();
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

        All of the calls to mesh->*Cane() are functions of relX/relY,
        but the constants involved are determined by experiment, 
        i.e. how much twist `feels' reasonable for moving the mouse 
        an inch.
        */
        if (mode == LOOK_MODE)
        {
                // Rotate camera position around look-at location.
                theta -= (relX * 500.0 * PI / 180.0);
                newFee = fee - (relY * 500.0 * PI / 180.0);
                if (newFee > 0.0f && newFee < PI)
                        fee = newFee;
                updateCamera();
        }
        else if (mode == TWIST_MODE) 
        {
                mesh->twistCane((relX * 500.0 * PI / 100.0));
                updateTriangles();
        }
        else if (mode == STRETCH_MODE)
        {
                mesh->stretchCane(-10.0*relY);
                updateTriangles();
        }
        else if (mode == BUNDLE_MODE)
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
                mesh->moveCane(relX * cos(theta + PI / 2.0) + relY * cos(theta), 
                        relX * sin(theta + PI / 2.0) + relY * sin(theta));
                updateTriangles();
        }
        else if (mode == SQUAREOFF_MODE)
        {
                mesh->squareoffCane(-(relY * 500.0 * PI / 180.0));
                updateTriangles();
        }

        paintGL();
}

/*
Convert a triangle in 3D to stuff OpenGL needs to draw it,
namely its normal vector. Assumes the triangle is specified 
in CCW order relative to its exterior.
*/
void OpenGLWidget :: drawTriangle(Triangle* t)
{
        Point vec1, vec2, norm;
        float norm_mag;

        // compute normal (going CCW around the face vertices)
        vec1.x = t->v2.x - t->v1.x;
        vec1.y = t->v2.y - t->v1.y;
        vec1.z = t->v2.z - t->v1.z;
        vec2.x = t->v3.x - t->v1.x;
        vec2.y = t->v3.y - t->v1.y;
        vec2.z = t->v3.z - t->v1.z;
        norm.x = vec1.y * vec2.z - vec1.z * vec2.y;
        norm.y = -(vec1.x * vec2.z - vec1.z * vec2.x);
        norm.z = vec1.x * vec2.y - vec1.y * vec2.x;
        norm_mag = sqrt(norm.x * norm.x + norm.y * norm.y + norm.z * norm.z);
        norm.x /= norm_mag;
        norm.y /= norm_mag;
        norm.z /= norm_mag;

        glNormal3d(norm.x, norm.y, norm.z);
        glColor4f(t->c.r, t->c.g, t->c.b, t->c.a);
        glBegin(GL_TRIANGLES);
        glVertex3f(t->v1.x, t->v1.y, t->v1.z);
        glVertex3f(t->v2.x, t->v2.y, t->v2.z);
        glVertex3f(t->v3.x, t->v3.y, t->v3.z);
        glEnd();
}

/*
Places an additional cane (given as an argument)
into the existing cane being operated on.
*/
void OpenGLWidget :: addCane(Cane* c)
{
        /*
        If there is no cane currently being operated on,
        start off by looking at it, otherwise start off by
        adjusting its location.
        */
        if (mesh->getCane() == NULL)
                setMode(LOOK_MODE); 
        else
                setMode(BUNDLE_MODE);
        mesh->addCane(c);
        updateTriangles();
        paintGL();
}

/*
Return a pointer to the cane curently being operated on.
DO NOT CHANGE THIS CANE, you will make the Mesh object cry.
*/
Cane* OpenGLWidget :: getCane()
{
        return mesh->getCane();
}


