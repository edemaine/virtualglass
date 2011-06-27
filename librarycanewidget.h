#ifndef LIBRARYCANEWIDGET_H
#define LIBRARYCANEWIDGET_H

#include <QtGui>
#include <QObject>
#include "openglwidget.h"
#include "cane.h"
#include "model.h"

class LibraryCaneWidget : public QLabel
{
	Q_OBJECT

public:
	LibraryCaneWidget(OpenGLWidget* o, Model* m, QWidget* parent);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	Cane* getCane();

signals:
	void modeChangedSig(int mode);

private:
	Cane* cane;
	Model* model;
};

#endif
