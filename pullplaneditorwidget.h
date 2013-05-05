
#ifndef PULLPLANEDITORWIDGET_H
#define PULLPLANEDITORWIDGET_H

#include <QWidget>
#include <QMutex>
#include <QWaitCondition>
#include <string>

#include "geometry.h"
#include "shape.h"
#include "mesh.h"
#include "constants.h"

class PullPlan;
class PullPlanEditorViewWidget;
class PullPlanCustomizeViewWidget;
class NiceViewWidget;
class AsyncPullPlanLibraryWidget;
class PullTemplateLibraryWidget;
class PullPlanGeometryThread;
class TwistWidget;

class QPushButton;
class QStackedWidget;
class QLabel;
class QTabWidget;
class QSpinBox;
class QHBoxLayout;
class QScrollArea;

class PullPlanEditorWidget : public QWidget
{
	Q_OBJECT

	friend class PullPlanGeometryThread;

	public:
		PullPlanEditorWidget(QWidget* parent=0);
		void resetPullPlan();
		PullPlan* getPullPlan();
		void setPullPlan(PullPlan* p);
		void seedTemplates();
		void writePlanToPLYFile(QString& filename);
		void writePlanToOBJFile(QString& filename);
		void updateEverything();

	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);

	signals:
		void someDataChanged();
		void showMessage(const QString& message, unsigned int timeout);

	private slots:
		void circleCasingButtonPressed();
		void squareCasingButtonPressed();
		void addCasingButtonPressed();
		void removeCasingButtonPressed();
		void addCircleButtonPressed();
		void addSquareButtonPressed();
		void copySelectedButtonPressed();
		void deleteSelectedButtonPressed();
		void childWidgetDataChanged();
		void countSpinChanged(int);
		void controlsTabChanged(int);
		void geometryThreadFinishedMesh(bool completed, unsigned int quality);

	private:
		QMutex tempPullPlanMutex;
		PullPlan* tempPullPlan;
		bool tempPullPlanDirty;

		QWaitCondition wakeWait;
		QMutex wakeMutex;

		PullPlanGeometryThread* geometryThread;

		QMutex geometryDirtyMutex;
		bool geometryDirty;
		QMutex tempGeometryMutex;
		Geometry tempGeometry;

		Geometry geometry;
		PullPlan* plan;
		PullPlanEditorViewWidget* viewWidget;	
		PullPlanCustomizeViewWidget* customizeViewWidget;	
		TwistWidget* twistWidget;
		QLabel* countLabel;
		QSpinBox* countSpin;
		QHBoxLayout* templateLibraryLayout;
		QPushButton* addCasingButton;
		QPushButton* removeCasingButton;
		QPushButton* customizePlanButton;
		QPushButton* circleCasingPushButton;
		QPushButton* squareCasingPushButton;
		QPushButton* addCircleButton;
		QPushButton* addSquareButton;
		QPushButton* copySelectedButton;
		QPushButton* deleteSelectedButton;
		QStackedWidget* viewEditorStack;
		QLabel* descriptionLabel;
		NiceViewWidget* niceViewWidget;
		QTabWidget* controlsTab;
		QScrollArea* pullTemplateLibraryScrollArea;

		bool isDragging;
		QPoint dragStartPosition;
		QPoint lastDragPosition;
		int maxDragDistance;

		void setupLayout();
		void setupThreading();
		void setupConnections();
};


#endif

