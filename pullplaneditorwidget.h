
#ifndef PULLPLANEDITORWIDGET_H
#define PULLPLANEDITORWIDGET_H

#include <QtGui>
#include "pullplan.h"
#include "pullplaneditorviewwidget.h"
#include "pullplancustomizeviewwidget.h"
#include "pulltemplatelibrarywidget.h"
#include "qgraphicshighlighteffect.h"
#include "asyncpullplanlibrarywidget.h"
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
		void seedTemplates();
		void mousePressEvent(QMouseEvent*);

	signals:
		void someDataChanged();
		void newPullPlan(PullPlan* p);
		void geometryChanged(Geometry g);
		void pullPlanChanged(PullPlan* p);

	public slots:
		void updateEverything();

	private slots:
		void fillRuleComboBoxChanged(int);
		void circleCasingButtonPressed();
		void squareCasingButtonPressed();
		void addCasingButtonPressed();
		void removeCasingButtonPressed();
		void viewWidgetDataChanged();
		void twistSpinChanged(int);
		void twistSliderChanged(int);
		void paramSpinChanged(int);

	private:
		Geometry geometry;
		Mesher mesher;
		PullPlan* plan;
		PullPlanEditorViewWidget* viewWidget;	
		PullPlanCustomizeViewWidget* customizeViewWidget;	
		QSpinBox* twistSpin;
		QSlider* twistSlider;
		vector<QLabel*> paramLabels;
		vector<QSpinBox*> paramSpins;
        	QHBoxLayout* templateLibraryLayout;
		QPushButton* addCasingButton;
		QPushButton* removeCasingButton;
		QPushButton* customizePlanButton;
		QPushButton* circleCasingPushButton;
		QPushButton* squareCasingPushButton;
		QComboBox* fillRuleComboBox;
                QPushButton* confirmChangesButton;
                QPushButton* cancelChangesButton;
		NiceViewWidget* niceViewWidget;

		void setupLayout();
		void setupConnections();
		void highlightLibraryWidget(PullTemplateLibraryWidget* w);
		void unhighlightLibraryWidget(PullTemplateLibraryWidget* w);
};


#endif

