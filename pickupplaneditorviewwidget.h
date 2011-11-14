
#ifndef PICKUPPLANEDITORVIEWWIDGET_H
#define PICKUPPLANEDITORVIEWWIDGET_H

#include <QtGui>
#include "pickupplan.h"
#include "constants.h"
#include "niceviewwidget.h"
#include "model.h"

class PickupPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PickupPlanEditorViewWidget(PickupPlan* plan, Model* model, QWidget* parent=0);
		void setPickupPlan(PickupPlan* plan);
		QPixmap getPixmap();
	
	signals:
		void someDataChanged();

	protected:
		void dragEnterEvent(QDragEnterEvent* dee);
		void dropEvent(QDropEvent* de);
		void paintEvent(QPaintEvent *event);

	private:
		Model* model;
		NiceViewWidget* niceViewWidget;
		PickupPlan* plan;
		int width;
		int height;	
};


#endif

