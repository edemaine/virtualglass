
#ifndef PULLPLANEDITORWIDGET_H
#define PULLPLANEDITORWIDGET_H

#include <QtGui>
#include <string>
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
class PullPlanGeometryThread;

class PullPlanEditorWidget : public QWidget
{
	Q_OBJECT

	friend class PullPlanGeometryThread;

	public:
		PullPlanEditorWidget(QWidget* parent=0);
		void resetPlan();
		PullPlan* getPlan();
		void setPlan(PullPlan* p);
		void seedTemplates();
		void mousePressEvent(QMouseEvent*);
		void writePlanToPLYFile(QString& filename);
		void writePlanToOBJFile(QString& filename);

	signals:
		void someDataChanged();
		void newPullPlan(PullPlan* p);

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
		void paramSliderChanged(int);
		void tabChanged(int);
		void geometryThreadFinishedMesh();

	private:
                QMutex tempPullPlanMutex;
                PullPlan* tempPullPlan;
                bool tempPullPlanDirty;

                QWaitCondition wakeWait;
                QMutex wakeMutex;

                PullPlanGeometryThread* geometryThread;
	
		QMutex geometryDirtyMutex;
		bool geometryDirty;
                QMutex tempGeometry1Mutex;
                QMutex tempGeometry2Mutex;
                Geometry tempGeometry1;
                Geometry tempGeometry2;

		Geometry geometry;
		PullPlan* plan;
		PullPlanEditorViewWidget* viewWidget;	
		PullPlanCustomizeViewWidget* customizeViewWidget;	
		QSpinBox* twistSpin;
		QSlider* twistSlider;
		vector<QLabel*> paramLabels;
		vector<QSpinBox*> paramSpins;
		vector<QSlider*> paramSliders;
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
		void setupThreading();
		void setupConnections();
};


#endif

