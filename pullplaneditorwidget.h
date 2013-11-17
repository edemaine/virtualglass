
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
class CaneCustomizeViewWidget;
class NiceViewWidget;
class AsyncPullPlanLibraryWidget;
class PullTemplateLibraryWidget;
class CaneGeometryThread;
class TwistWidget;

class QImage;
class QPushButton;
class QStackedWidget;
class QLabel;
class QTabWidget;
class QSpinBox;
class QHBoxLayout;
class QScrollArea;

#define MIN_PULLPLAN_COUNT_PARAMETER_VALUE 0
#define MAX_PULLPLAN_COUNT_PARAMETER_VALUE 30

class PullPlanEditorWidget : public QWidget
{
	Q_OBJECT

	friend class CaneGeometryThread;

	public:
		PullPlanEditorWidget(QWidget* parent=0);
		void resetPullPlan();
		PullPlan* pullPlan();
		void setPullPlan(PullPlan* p);
		void seedTemplates();
		void writePlanToPLYFile(QString& filename);
		void writePlanToOBJFile(QString& filename);
		void updateEverything();
		QImage pullPlanImage();
		void reset3DCamera();

		void undo();
		void redo();
		bool canUndo();
		bool canRedo();

	protected:
		bool eventFilter(QObject* obj, QEvent* event);
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);

	signals:
		void someDataChanged();
		void showMessage(const QString& message, unsigned int timeout);

	private slots:
		void circleCasingButtonClicked();
		void squareCasingButtonClicked();
		void addCasingButtonClicked();
		void removeCasingButtonClicked();
		void addCircleButtonClicked();
		void addSquareButtonClicked();
		void copySelectedButtonClicked();
		void deleteSelectedButtonClicked();
		void childWidgetDataChanged();
		void twistEnded();
		void countSpinChanged(int);
		void controlsTabChanged(int);
		void geometryThreadFinishedMesh(bool completed, unsigned int quality);

	private:
		QMutex tempPullPlanMutex;
		PullPlan* tempPullPlan;
		bool tempPullPlanDirty;

		QWaitCondition wakeWait;
		QMutex wakeMutex;

		CaneGeometryThread* geometryThread;

		QMutex geometryDirtyMutex;
		bool geometryDirty;
		QMutex tempGeometryMutex;
		Geometry tempGeometry;

		Geometry geometry;
		PullPlan* plan;
		PullPlanEditorViewWidget* viewWidget;	
		CaneCustomizeViewWidget* customizeViewWidget;	
		NiceViewWidget* niceViewWidget;
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

