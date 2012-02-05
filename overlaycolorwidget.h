#ifndef OVERLAYCOLORWIDGET_H
#define OVERLAYCOLORWIDGET_H

#include <QtGui>
#include <QObject>
#include "pullplan.h"

class OverlayColorWidget : public QWidget
{
	Q_OBJECT

	public:
		OverlayColorWidget(Color** color, QWidget* parent=0);

	signals:
		void colorChanged(); 

	protected:
		void dropEvent(QDropEvent* de);
		void dragEnterEvent(QDragEnterEvent* event);
		void paintEvent(QPaintEvent* pe);

	private:
		Color** color;
};

#endif
