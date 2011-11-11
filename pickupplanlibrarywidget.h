#ifndef PICKUPPLANLIBRARYWIDGET_H
#define PICKUPPLANLIBRARYWIDGET_H

class PickupPlan;

#include <QtGui>
#include <QObject>
#include "pickupplan.h"

class PickupPlanLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		PickupPlanLibraryWidget(QPixmap niceViewPixmap, QPixmap editorPixmap, PickupPlan* plan, QWidget* parent=0);
		PickupPlan* getPickupPlan();
		const QPixmap* getEditorPixmap();
		void updatePixmaps(QPixmap niceViewPixmap, QPixmap editorPixmap);

	private:
		PickupPlan* pickupPlan;
		QPixmap editorPixmap;
};

#endif
