
#include "openglwidget.h" 
 
OpenGLWidget :: OpenGLWidget(QWidget *parent=0) : QGLWidget(parent)
{
        resolution = HIGH_RESOLUTION;
        cur_active_subcane = NO_SUBCANES;
        mode = 1;  
        mesh = new Mesh(NULL);
        triangles = mesh->getMesh(resolution);
        num_triangles = mesh->getNumMeshTriangles(resolution);
}

void OpenGLWidget :: initializeGL()
{
        theta = -PI/2.0;
        fee = PI/4.0;
        rho = 3.0;

        light_position[0] = 0.0;
        light_position[1] = 0.0;
        light_position[2] = 1000.0;
        light_position[3] = 0.0;

        look_at_loc[0] = look_at_loc[1] = 0;
        look_at_loc[2] = 0.5;

        glEnable(GL_LIGHTING);

        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

        glEnable(GL_LIGHT0);
}
 
void OpenGLWidget :: zeroCanes()
{
        mesh->setCane(NULL);
        updateTriangles();
        paintGL();
} 

void OpenGLWidget :: paintGL() 
{
        int i;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
                
        glBegin(GL_TRIANGLES);
        for (i = 0; i < num_triangles; ++i)
        {
                drawTriangle(&(triangles[i]));
        }
        glEnd();

        swapBuffers();  
}  
 
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

void OpenGLWidget :: setFocusCane(Cane* c)
{
        mesh->setCane(c);
        paintGL();
} 


void OpenGLWidget :: updateResolution(int new_resolution)
{
        resolution = new_resolution;
        triangles = mesh->getMesh(new_resolution);
        num_triangles = mesh->getNumMeshTriangles(new_resolution);
}

void OpenGLWidget :: updateIlluminatedSubcane(int new_ill_subcane)
{
        cur_active_subcane = new_ill_subcane;
}

void OpenGLWidget :: updateTriangles()
{
        triangles = mesh->getMesh(resolution);
        num_triangles = mesh->getNumMeshTriangles(resolution);
} 

void OpenGLWidget :: setMode(int m)
{
        this->mode = m;
        if (mode == 4)
        {
                cur_active_subcane = 0;        
                updateIlluminatedSubcane(cur_active_subcane);
        }
        else
        {
                updateIlluminatedSubcane(NO_SUBCANES);
        }
}

void OpenGLWidget :: advanceActiveSubcane()
{
        if (mode == 4)
        {
                mesh->advanceActiveSubcane(&cur_active_subcane);
                mesh->setIlluminatedSubcane(cur_active_subcane);
                paintGL();
        }
}

void OpenGLWidget :: updateCamera()
{
        eye_loc[0] = look_at_loc[0] + rho*sin(fee)*cos(theta);
        eye_loc[1] = look_at_loc[1] + rho*sin(fee)*sin(theta);
        eye_loc[2] = look_at_loc[2] + rho*cos(fee);
        glLoadIdentity();
        gluLookAt(
        eye_loc[0], eye_loc[1], eye_loc[2],
        look_at_loc[0], look_at_loc[1], look_at_loc[2],
        0.0, 0.0, 1.0);   // up vector
}

void OpenGLWidget :: mousePressEvent (QMouseEvent* e)
{
        //----Init new location
        gNewX = e->x();
        gNewY = e->y();

        if (mode != 1) // keep things high res if in look mode
                updateResolution(LOW_RESOLUTION);
        paintGL();
}

void OpenGLWidget :: mouseReleaseEvent (QMouseEvent* e)
{
        updateResolution(HIGH_RESOLUTION);
        paintGL();
}

void OpenGLWidget :: mouseMoveEvent (QMouseEvent* e)
{
        float relX, relY;
        float newFee;
        float windowWidth, windowHeight;
        int gOldX, gOldY;

        windowWidth = this->width();
        windowHeight = this->height();

        //----calculate change in mouse x
        gOldX = gNewX;
        gNewX = e->x();
        relX = (gNewX - gOldX) / windowWidth;

        //----calculate change in mouse y
        gOldY = gNewY;
        gNewY = e->y();
        relY = (gNewY - gOldY) / windowHeight;

        //----calculate change in mouse r and theta

        if (mode == LOOK_MODE)
        {
                //rotate eye position around lookat point
                //only rotate in one direction at a time
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
                mesh->stretchCane(-10.0*relY, 200);
                updateTriangles();
        }
        else if (mode == BUNDLE_MODE)
        {
                mesh->moveCane(cur_active_subcane, 
                        relX * cos(theta + PI / 2.0) + relY * cos(theta), 
                        relX * sin(theta + PI / 2.0) + relY * sin(theta));
                updateTriangles();
        }
        else if (mode == SQUAREOFF_MODE)
        {
                mesh->squareoffCane(-(relY * 500.0 * PI / 180.0), 200);
                updateTriangles();
        }

        paintGL();
}


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
        glColor3f(t->c.r, t->c.g, t->c.b);
        glBegin(GL_TRIANGLES);
        glVertex3f(t->v1.x, t->v1.y, t->v1.z);
        glVertex3f(t->v2.x, t->v2.y, t->v2.z);
        glVertex3f(t->v3.x, t->v3.y, t->v3.z);
        glEnd();
}

void OpenGLWidget :: addCane(Cane* c)
{
        setMode(BUNDLE_MODE);
        mesh->addCane(c, &cur_active_subcane);
        updateTriangles();
        paintGL();
}

Cane* OpenGLWidget :: getCane()
{
        return mesh->getCane();
}


