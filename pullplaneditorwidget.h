
#ifndef PULLPLANEDITORWIDGET_H
#define PULLPLANEDITORWIDGET_H

#include <QtGui>
#include "pullplan.h"
#include "pullplaneditorviewwidget.h"
#include "pulltemplatelibrarywidget.h"
#include "pullplanlibrarywidget.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"

class PullPlanEditorWidget : public QWidget
{
	Q_OBJECT

	public:
		PullPlanEditorWidget(QWidget* parent=0);
		PullPlan* getPlan();
		void setPlan(PullPlan* p);
		PullTemplateLibraryWidget* getTemplateLibraryWidget();	
		void updateLibraryWidgetPixmaps(PullPlanLibraryWidget* w);
		void seedTemplates();
		void setPlanSubplans(Color* c);
		void setPlanColor(Color* c);
		void setPlanTwist(int t);
		void setPlanTemplate(PullTemplate* t);
		void setPlanTemplateCasingThickness(float t);
		void mousePressEvent(QMouseEvent*);

	signals:
		void someDataChanged();

	public slots:
		void updateEverything();

	private slots:
		void shapeButtonGroupChanged(int);
		void twistSpinChanged(int);
		void twistSliderChanged(int);
		void casingThicknessSliderChanged(int);
		void paramSpinChanged(int);

	private:
		Geometry geometry;
		Mesher mesher;
		PullPlan* plan;
		PullPlanEditorViewWidget* viewWidget;	
		NiceViewWidget* niceViewWidget;
		QSpinBox* twistSpin;
		QSlider* twistSlider;
		vector<QLabel*> paramLabels;
		vector<QSpinBox*> paramSpins;
                QButtonGroup* shapeButtonGroup;
		QSlider* casingThicknessSlider;
        	QHBoxLayout* templateLibraryLayout;

		void setupLayout();
		void setupConnections();
};


#endif

