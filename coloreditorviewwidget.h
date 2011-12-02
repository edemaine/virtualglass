
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
		void seedMartyColors();
		void seedBrandColors();
	
	signals:
		void someDataChanged();

	protected:
		void mouseReleaseEvent(QMouseEvent* event);

	private slots:
		void alphaSliderPositionChanged(int);

	private:
		PullPlan* plan;
		QSlider* alphaSlider;
		QVBoxLayout* colorLibrary1Layout;
		QVBoxLayout* colorLibrary2Layout;

};


#endif

