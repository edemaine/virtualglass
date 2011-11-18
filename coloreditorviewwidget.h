
#ifndef COLOREDITORVIEWWIDGET_H
#define COLOREDITORVIEWWIDGET_H

#include <QtGui>
#include <string>
#include <iostream>
#include "pullplan.h"
#include "purecolorlibrarywidget.h"

using namespace std;

class ColorEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		ColorEditorViewWidget(PullPlan* plan, QWidget* parent=0);
		void setPullPlan(PullPlan* plan);
	
	signals:
		void someDataChanged();

	protected:
		void mouseReleaseEvent(QMouseEvent* event);

	private slots:
		void alphaSliderPositionChanged(int);

	private:
		PullPlan* plan;
		QSlider* alphaSlider;
};


#endif

