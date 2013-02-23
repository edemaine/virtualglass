
#ifndef PICKUPPLANEDITORVIEWWIDGET_H
#define PICKUPPLANEDITORVIEWWIDGET_H

#include <QMutex>
#include <QWaitCondition>
#include "pickupplan.h"
#include "niceviewwidget.h"
#include "mesh.h"
#include "geometry.h"
#include "pickupplan.h"
#include "asyncpullplanlibrarywidget.h"
#include "glassmime.h"
#include "glasscolor.h"

class PickupGeometryThread;

class PickupPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	friend class PickupGeometryThread;

	public:
		PickupPlanEditorViewWidget(PickupPlan* pickup, QWidget* parent=0);
		void setPickup(PickupPlan* p);
		QPixmap getPixmap();
	
	signals:
		void someDataChanged();

	public slots:
		void updateEverything();

	protected:
		void mousePressEvent(QMouseEvent* event);
		void dragEnterEvent(QDragEnterEvent* dee);
		void dropEvent(QDropEvent* de);
		void resizeEvent(QResizeEvent* event);

	private slots:
		void geometryThreadFinishedMesh();

	private:
		QMutex tempPickupMutex;
		PickupPlan* tempPickup;
		bool tempPickupDirty;

		QWaitCondition wakeWait;
		QMutex wakeMutex;

		PickupGeometryThread* geometryThread;

		QMutex geometryDirtyMutex;
		bool geometryDirty;
		QMutex tempGeometry1Mutex;
		QMutex tempGeometry2Mutex;
		Geometry tempGeometry1;
		Geometry tempGeometry2;

		Geometry geometry;
		NiceViewWidget* niceViewWidget;
		PickupPlan* pickup;
		float ulX, ulY, squareSize;

		void setupThreading();
		void setupConnections();
		float adjustedX(float rawX);
		float adjustedY(float rawX);
		float rawX(float adjustedX);
		float rawY(float adjustedY);
		void getSubplanAt(float x, float y, PullPlan** plan, int* subplanIndex);
};


#endif

