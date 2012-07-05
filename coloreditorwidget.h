
#ifndef COLOREDITORWIDGET_H
#define COLOREDITORWIDGET_H

#include <QtGui>
#include "pullplan.h"
#include "qgraphicshighlighteffect.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"
#include "purecolorlibrarywidget.h"

class ColorEditorWidget : public QWidget
{
	Q_OBJECT

	public:
		ColorEditorWidget(PullPlan* bar, QWidget* parent=0);
		void setColorBar(PullPlan* bar);
		PullPlan* getColorBar();
		void seedColors();

	signals:
		void someDataChanged();

	protected:
		void mousePressEvent(QMouseEvent* event);

	public slots:
		void updateEverything();
                void alphaSliderPositionChanged(int);
		void sourceComboBoxChanged(int);		

	private:
		Geometry geometry;
		Mesher mesher;
		PullPlan* colorBar;
		QComboBox* sourceComboBox;
		NiceViewWidget* niceViewWidget;
                QSlider* alphaSlider;
		vector<QScrollArea*> colorLibraryScrollAreas;
		vector<QVBoxLayout*> colorLibraryLayouts;

		void setupLayout();
		void setupConnections();
};


#endif

