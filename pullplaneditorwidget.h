
#ifndef PULLPLANEDITORWIDGET_H
#define PULLPLANEDITORWIDGET_H

#include <QtGui>
#include "pullplan.h"
#include "pullplaneditorviewwidget.h"
#include "pulltemplatelibrarywidget.h"
#include "qgraphicshighlighteffect.h"
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
		void updateLibraryWidgetPixmaps(PullPlanLibraryWidget* w);
		void seedTemplates();
		void setPlanSubplans(PullPlan* plan);
		void setPlanColor(Color* c);
		void setPlanTwist(int t);
		void setPlanTemplate(int templateType);
		void setPlanTemplateCasingThickness(float t);
		void mousePressEvent(QMouseEvent*);

	signals:
		void someDataChanged();
		void newPullPlan(PullPlan* p);

	public slots:
		void updateEverything();

	private slots:
		void fillRuleButtonGroupChanged(int);
		void addCasingButtonPressed();
		void viewWidgetDataChanged();
		void shapeButtonGroupChanged(int);
		void twistSpinChanged(int);
		void twistSliderChanged(int);
		void casingThicknessSpinChanged(int);
		void casingThicknessSliderChanged(int);
		void paramSpinChanged(int);

	private:
		Geometry geometry;
		Mesher mesher;
		PullPlan* plan;
		PullPlanEditorViewWidget* viewWidget;	
		NiceViewWidget* niceViewWidget;
		QSpinBox* casingThicknessSpin;
		QSlider* casingThicknessSlider;
		QSpinBox* twistSpin;
		QSlider* twistSlider;
		vector<QLabel*> paramLabels;
		vector<QSpinBox*> paramSpins;
                QButtonGroup* fillRuleButtonGroup;
                QButtonGroup* shapeButtonGroup;
        	QHBoxLayout* templateLibraryLayout;
		QPushButton* addCasingButton;

		void setupLayout();
		void setupConnections();
		void highlightLibraryWidget(PullTemplateLibraryWidget* w);
		void unhighlightLibraryWidget(PullTemplateLibraryWidget* w);
};


#endif

