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
	void mouseMoveEvent(QMouseEvent* event);
	void enterEvent(QEvent* event);
	Cane* getCane();

signals:
	void modeChangedSig(int mode);
	void mouseOver(LibraryCaneWidget*);

private:
	Cane* cane;
	Model* model;
};

#endif
