
#ifndef PICKUPPLANEDITORVIEWWIDGET_H
#define PICKUPPLANEDITORVIEWWIDGET_H

#include <QtGui>
#include "pickupplan.h"
#include "constants.h"
#include "niceviewwidget.h"
#include "mesh.h"
#include "geometry.h"
#include "pickupplan.h"
#include "asyncpullplanlibrarywidget.h"
#include "glassmime.h"

class PickupPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PickupPlanEditorViewWidget(PickupPlan* pickup, QWidget* parent=0);
		void setPickup(PickupPlan* p);
		QPixmap getPixmap();
	
	signals:
		void someDataChanged();

	protected:
                void mousePressEvent(QMouseEvent* event);
		void dragEnterEvent(QDragEnterEvent* dee);
		void dropEvent(QDropEvent* de);
                void resizeEvent(QResizeEvent* event);

	private:
		Mesher mesher;
		Geometry geometry;
		NiceViewWidget* niceViewWidget;
		PickupPlan* pickup;
                float ulX, ulY, squareSize;

                float adjustedX(float rawX);
                float adjustedY(float rawX);
                float rawX(float adjustedX);
                float rawY(float adjustedY);
		void getSubplanAt(float x, float y, PullPlan** plan, int* subplanIndex);
};


#endif

