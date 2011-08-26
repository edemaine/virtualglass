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
	void enterEvent(QEvent* event);
	Cane* getCane();
	void newLibraryIndex(int index);

signals:
	void modeChangedSig(int mode);
	void mouseOver(LibraryCaneWidget*);
	void newIndex(int index);
	void addCane(Cane* c);
	void requestDelete(Cane* c);

public slots:
	void deleteRequestAccepted(Cane* c);

private:
	Cane* cane;
	Model* model;
};

#endif
