
#ifndef PICKUPPLANEDITORVIEWWIDGET_H
#define PICKUPPLANEDITORVIEWWIDGET_H

#include <QtGui>
#include "pickupplan.h"
#include "constants.h"
#include "niceviewwidget.h"
#include "mesh.h"
#include "geometry.h"
#include "pickupplan.h"

class PickupPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PickupPlanEditorViewWidget(PickupPlan* pickup, QWidget* parent=0);
		void setPickup(PickupPlan* p);
		QPixmap getPixmap();
		void setFillRule(int r);
		int getFillRule();
                void resizeEvent(QResizeEvent* event);
	
	signals:
		void someDataChanged();

	protected:
		void dragEnterEvent(QDragEnterEvent* dee);
		void dropEvent(QDropEvent* de);

	private:
		Mesher mesher;
		Geometry geometry;
		NiceViewWidget* niceViewWidget;
		PickupPlan* pickup;
		int fillRule;

                float adjustedX(float rawX);
                float adjustedY(float rawX);
                float rawX(float adjustedX);
                float rawY(float adjustedY);
                float ulX, ulY, squareSize;

};


#endif

