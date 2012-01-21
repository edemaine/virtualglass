
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
	
	signals:
		void someDataChanged();

	protected:
		void dragEnterEvent(QDragEnterEvent* dee);
		void dropEvent(QDropEvent* de);
		void paintEvent(QPaintEvent *event);

	private:
		Mesher mesher;
		Geometry geometry;
		NiceViewWidget* niceViewWidget;
		PickupPlan* pickup;
		int fillRule;
};


#endif

