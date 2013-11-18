
#ifndef CANEEDITORWIDGET_H
#define CANEEDITORWIDGET_H

#include <QWidget>
#include <QMutex>
#include <QWaitCondition>
#include <string>

#include "geometry.h"
#include "shape.h"
#include "mesh.h"
#include "constants.h"

class Cane;
class CaneEditorViewWidget;
class CaneCustomizeViewWidget;
class NiceViewWidget;
class CaneTemplateLibraryWidget;
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

#define MIN_CANE_COUNT_PARAMETER_VALUE 0
#define MAX_CANE_COUNT_PARAMETER_VALUE 30

class CaneEditorWidget : public QWidget
{
	Q_OBJECT

	friend class CaneGeometryThread;

	public:
		CaneEditorWidget(QWidget* parent=0);
		void resetCane();
		Cane* cane();
		void setCane(Cane* c);
		void seedTemplates();
		void writeCaneToPLYFile(QString& filename);
		void writeCaneToOBJFile(QString& filename);
		void updateEverything();
		QImage caneImage();
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
		QMutex tempCaneMutex;
		Cane* tempCane;
		bool tempCaneDirty;

		QWaitCondition wakeWait;
		QMutex wakeMutex;

		CaneGeometryThread* geometryThread;

		QMutex geometryDirtyMutex;
		bool geometryDirty;
		QMutex tempGeometryMutex;
		Geometry tempGeometry;

		Geometry geometry;
		Cane* _cane;
		CaneEditorViewWidget* viewWidget;	
		CaneCustomizeViewWidget* customizeViewWidget;	
		NiceViewWidget* niceViewWidget;
		TwistWidget* twistWidget;

		QLabel* countLabel;
		QSpinBox* countSpin;

		QHBoxLayout* templateLibraryLayout;
		QPushButton* addCasingButton;
		QPushButton* removeCasingButton;
		QPushButton* customizeCaneButton;
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

