
#ifndef PULLPLANEDITORWIDGET_H
#define PULLPLANEDITORWIDGET_H

#include <QtGui>
#include "shape.h"
#include "mesh.h"

class PullPlan;
class Geometry;
class Mesher;
class PullPlanEditorViewWidget;
class PullPlanCustomizeViewWidget;
class NiceViewWidget;
class AsyncPullPlanLibraryWidget;
class PullTemplateLibraryWidget;

class PullPlanEditorWidget : public QWidget
{
	Q_OBJECT

	public:
		PullPlanEditorWidget(QWidget* parent=0);
		void resetPlan();
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
		QStackedWidget* paramStack;
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
};


#endif

