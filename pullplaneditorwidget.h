
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
#include "shape.h"
#include "geometry.h"
#include "mesh.h"
#include "dependancy.h"

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

	public slots:
		void updateEverything();

	private slots:
		void circleCasingButtonPressed();
		void squareCasingButtonPressed();
		void addCasingButtonPressed();
		void removeCasingButtonPressed();
        void addCircleButtonPressed();
        void addSquareButtonPressed();
        void copySelectedButtonPressed();
        void deleteSelectedButtonPressed();
        void viewWidgetDataChanged();
        void customizeViewWidgetDataChanged();
		void twistSpinChanged(int);
		void twistSliderChanged(int);
		void paramSpinChanged(int);
        void tabChanged(int);

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
                QPushButton* confirmChangesButton;
                QPushButton* cancelChangesButton;
                QPushButton* addCircleButton;
                QPushButton* addSquareButton;
                QPushButton* copySelectedButton;
                QPushButton* deleteSelectedButton;
        QTabWidget* tabs;
        QLabel* descriptionLabel;
		NiceViewWidget* niceViewWidget;

		void setupLayout();
		void setupConnections();
		void highlightLibraryWidget(PullTemplateLibraryWidget* w);
		void unhighlightLibraryWidget(PullTemplateLibraryWidget* w);
};


#endif

