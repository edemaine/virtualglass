#ifndef LIBRARYCANEWIDGET_H
#define LIBRARYCANEWIDGET_H

#include <QtGui>
#include "openglwidget.h"
#include "cane.h"
#include "model.h"

class LibraryCaneWidget : public QLabel 
{
	public:
		LibraryCaneWidget(OpenGLWidget* w, Model* m, Cane* c, QWidget* parent);
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		Cane* getCane();

	private:
		Cane* cane;
		Model* model;
};

#endif
