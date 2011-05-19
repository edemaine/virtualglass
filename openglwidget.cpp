
#include "openglwidget.h" 
 
// Gives you a random bit back that
// has probability `a' of being a 1
int prob_bit(float a)
{
        return ((rand() % ((int) (1.0/(a)))) == 0);
}
 
OpenGLWidget :: OpenGLWidget(QWidget *parent=0) : QGLWidget(parent)
{
        mesh = NULL; 
        num_mesh_elements = 0; 
        mode = 1;  
        mousePressed = 0;

        for(int i = 0; i < 32*4; ++i)
        { 
                glass_stipple_data[i] = 0;
                for (int j = 0; j < 8; ++j)
                {
                        glass_stipple_data[i] = glass_stipple_data[i] | (prob_bit(GLASS_STIPPLE_ALPHA) << j);
                }
        }

}

void OpenGLWidget :: initializeGL()
{
        theta = -PI/2.0;
        fee = PI/4.0;
        rho = 3.0;

        gLeftDown = 0;
        gShiftLeftDown = 0;
        gAltLeftDown = 0;

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
        cane = NULL;
        updateCane();
        updateCamera();
        paintGL();
} 

void OpenGLWidget :: paintGL() 
{
        int i;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
                
        glBegin(GL_TRIANGLES);
        for (i = 0; i < num_mesh_elements; ++i)
        {
                drawTriangle(&(mesh[i]));
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
        cane = c;
        updateCane();
        updateCamera();
        paintGL();
} 


void OpenGLWidget :: updateCane()
{
        int resolution, illuminated_subcane;

        if (mousePressed)
                resolution = LOW_RESOLUTION;
        else
                resolution = HIGH_RESOLUTION;

        if (mode == 4)
                illuminated_subcane = cur_active_subcane;
        else
                illuminated_subcane = NO_SUBCANES; 

        convert_to_mesh(cane, &mesh, &num_mesh_elements, illuminated_subcane, 0, resolution);
} 

float OpenGLWidget :: abs(float v)
{
        if (v < 0)
                return -v;
        else
                return v;
}

void OpenGLWidget :: setMode(int m)
{
        this->mode = m;
        if (mode == 4)
        {
                cur_active_subcane = 0;        
        }
}

void OpenGLWidget :: advanceActiveSubcane()
{
        if (mode == 4)
        {
                cur_active_subcane++;
                cur_active_subcane %= cane->num_subcanes;
                updateCane();
                updateCamera();
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

        mousePressed = 1;

        updateCane();
        updateCamera();
        paintGL();
}

void OpenGLWidget :: mouseReleaseEvent (QMouseEvent* e)
{
        mousePressed = 0;

        updateCane();
        updateCamera();
        paintGL();
}

void OpenGLWidget :: mouseMoveEvent (QMouseEvent* e)
{
        float relX, relY;
        float curCaneX, curCaneY;
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
        }
        else if (mode == TWIST_MODE) 
        {
                if (cane == NULL)
                        return;
                cane->twist((relX * 500.0 * PI / 180.0));
        }
        else if (mode == STRETCH_MODE)
        {
                if (cane == NULL)
                        return;
                cane->stretch(-10.0*relY, 200);
        }
        else if (mode == BUNDLE_MODE)
        {
                if (cane == NULL)
                        return; 
                cane->createBundle();
                curCaneX = cane->subcane_locs[cur_active_subcane].x; 
                curCaneY = cane->subcane_locs[cur_active_subcane].y; 
                curCaneX += relX * cos(theta + PI / 2.0) + relY * cos(theta); 
                curCaneY += relX * sin(theta + PI / 2.0) + relY * sin(theta); 
                cane->subcane_locs[cur_active_subcane].x = curCaneX;
                cane->subcane_locs[cur_active_subcane].y = curCaneY;
        }
        else if (mode == SQUAREOFF_MODE)
        {
                if (cane == NULL)
                        return; 
                cane->squareoff(-(relY * 500.0 * PI / 180.0), 200);
        }

        //update global variable camera position relative to lookat point
        updateCane();
        updateCamera();
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

        // define the triangle
        glNormal3d(norm.x, norm.y, norm.z);
        if (t->c.a == 1.0)
        {
                glColor3f(t->c.r, t->c.g, t->c.b);
                glBegin(GL_TRIANGLES);
                glVertex3f(t->v1.x, t->v1.y, t->v1.z);
                glVertex3f(t->v2.x, t->v2.y, t->v2.z);
                glVertex3f(t->v3.x, t->v3.y, t->v3.z);
                glEnd();
        } 
        else if (t->c.a == GLASS_STIPPLE_ALPHA)
        {
                glEnable(GL_POLYGON_STIPPLE);
                glPolygonStipple(glass_stipple_data);
                glColor3f(t->c.r, t->c.g, t->c.b);
                glBegin(GL_TRIANGLES); 
                glVertex3f(t->v1.x, t->v1.y, t->v1.z);
                glVertex3f(t->v2.x, t->v2.y, t->v2.z);
                glVertex3f(t->v3.x, t->v3.y, t->v3.z);
                glEnd(); 
                glDisable(GL_POLYGON_STIPPLE); 
        }
        else // Generate new stipple data by hand
        {
                glEnable(GL_POLYGON_STIPPLE);
                GLubyte stipple_data[128];

                for(int i = 0; i < 128; ++i)
                { 
                        stipple_data[i] = 0;
                        for (int j = 0; j < 8; ++j)
                        {
                                stipple_data[i] = stipple_data[i] | (prob_bit(t->c.a) << j);
                        }
                }
                glPolygonStipple(stipple_data);

                glColor3f(t->c.r, t->c.g, t->c.b);
                glBegin(GL_TRIANGLES); 
                glVertex3f(t->v1.x, t->v1.y, t->v1.z);
                glVertex3f(t->v2.x, t->v2.y, t->v2.z);
                glVertex3f(t->v3.x, t->v3.y, t->v3.z);
                glEnd(); 
                glDisable(GL_POLYGON_STIPPLE); 
        }



}

void OpenGLWidget :: addCane(Cane* c)
{
        setMode(BUNDLE_MODE);
        if (cane == NULL)
        {
                cane = c->deepCopy();
        }
        else
        {
                cane->add(c, &cur_active_subcane);
        }
        updateCane();
        updateCamera();
        paintGL();
}

Cane* OpenGLWidget :: getCane()
{
        return cane;
}


