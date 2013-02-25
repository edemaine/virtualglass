
#ifndef PICKUPPLANEDITORVIEWWIDGET_H
#define PICKUPPLANEDITORVIEWWIDGET_H

#include "pickupplan.h"
#include "mesh.h"
#include "geometry.h"
#include "pickupplan.h"
#include "asyncpullplanlibrarywidget.h"

class NiceViewWidget;

class PickupPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	friend class PieceEditorWidget;

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

	private:
		Geometry geometry;
		NiceViewWidget* niceViewWidget;
		PickupPlan* pickup;
		float ulX, ulY, squareSize;

		void setupConnections();
		float adjustedX(float rawX);
		float adjustedY(float rawX);
		float rawX(float adjustedX);
		float rawY(float adjustedY);
		void getSubplanAt(float x, float y, PullPlan** plan, int* subplanIndex);
};


#endif

