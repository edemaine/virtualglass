
#ifndef PICKUPGEOMETRYTHREAD_H
#define PICKUPGEOMETRYTHREAD_H

#include <QThread>

class PickupPlanEditorViewWidget;

class PickupGeometryThread : public QThread
{
	Q_OBJECT

	public:
		PickupGeometryThread(PickupPlanEditorViewWidget* ppew);
		void run();
		PickupPlanEditorViewWidget* ppew;

	signals:
		void finishedMesh();
};

#endif




