#ifndef NICEVIEWWIDGET_H
#define NICEVIEWWIDGET_H

#include "primitives.h"
#include "geometry.h"
#include "peelrenderer.h"

class QGLFramebufferObject;

class NiceViewWidget : public QGLWidget
{
	Q_OBJECT

	public:
		enum CameraMode
		{
			PULLPLAN_CAMERA_MODE,
			PICKUPPLAN_CAMERA_MODE,
			PIECE_CAMERA_MODE 
		};
		
		NiceViewWidget(enum CameraMode cameraMode, QWidget* parent=0);
		virtual ~NiceViewWidget();
		Vector3f eyePosition();
		void setGeometry(Geometry* g);

	protected:
		void initializeGL();
		void initializePeel();
		void destructPeel();
		void paintGL();
		void resizeGL(int width, int height);
		void mousePressEvent(QMouseEvent* e);
		void mouseReleaseEvent(QMouseEvent* e);
		void mouseMoveEvent(QMouseEvent* e);
		void wheelEvent(QWheelEvent* e);

	private:
		bool leftMouseDown;
		Geometry *geometry;
		enum CameraMode cameraMode;
		float lookAtLoc[3];
		float theta, phi, rho;
		int mouseLocX, mouseLocY;
		bool initializeGLCalled;
		PeelRenderer *peelRenderer;

		void setGLMatrices();
};

#endif


