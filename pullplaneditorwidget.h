
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
#include "pullplancustomizewidget.h"

class PullPlanEditorWidget : public QWidget
{
	Q_OBJECT

	public:
		PullPlanEditorWidget(QWidget* parent=0);
		PullPlan* getPlan();
		void setPlan(PullPlan* p);
		void updateLibraryWidgetPixmaps(PullPlanLibraryWidget* w);
		void seedTemplates();
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
		void paramSpinChanged(int);
		void openCustomizeWidget();

	private:
		Geometry geometry;
		Mesher mesher;
		PullPlan* plan;
		PullPlanEditorViewWidget* viewWidget;	
		PullPlanCustomizeWidget* pullPlanCustomizeWidget;
		NiceViewWidget* niceViewWidget;
		QSpinBox* twistSpin;
		QSlider* twistSlider;
		vector<QLabel*> paramLabels;
		vector<QSpinBox*> paramSpins;
                QButtonGroup* fillRuleButtonGroup;
                QButtonGroup* shapeButtonGroup;
        	QHBoxLayout* templateLibraryLayout;
		QPushButton* addCasingButton;
		QPushButton* customizePlanButton;

		void setupLayout();
		void setupConnections();
		void highlightLibraryWidget(PullTemplateLibraryWidget* w);
		void unhighlightLibraryWidget(PullTemplateLibraryWidget* w);
};


#endif

