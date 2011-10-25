
#include "pickuptemplatelibrarywidget.h"

PickupTemplateLibraryWidget :: PickupTemplateLibraryWidget(QPixmap view, int type, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(view);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->pickupTemplateType = type;
}

int PickupTemplateLibraryWidget :: getPickupTemplateType()
{
	return pickupTemplateType;
}


