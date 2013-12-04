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
			GLASSCOLOR_CAMERA_MODE,
			PULLPLAN_CAMERA_MODE,
			PICKUPPLAN_CAMERA_MODE,
			PIECE_CAMERA_MODE 
		};
		
		NiceViewWidget(enum CameraMode cameraMode, QWidget* parent=0);
		virtual ~NiceViewWidget();
		Vector3f eyePosition();
		void setGeometry(Geometry* g);
		void resetCamera();

	protected:
		void initializeGL();
		void initializePeel();
		void destructPeel();
		void paintGL();
		void resizeGL(int width, int height);
		bool event(QEvent* e);

	private:
		static const int max_rho = 30;
		static const int min_rho = 1;
		static const int rho_step = 1;

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


