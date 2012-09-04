#ifndef PICKUPTEMPLATELIBRARYWIDGET_H
#define PICKUPTEMPLATELIBRARYWIDGET_H

#include <QtGui>
#include "qgraphicshighlighteffect.h"

class PickupTemplateLibraryWidget : public QLabel
{
	public:
		PickupTemplateLibraryWidget(QPixmap view, int pickupTemplateType, QWidget* parent=0);
		int getPickupTemplateType();

	private:
		int pickupTemplateType;
};

#endif
