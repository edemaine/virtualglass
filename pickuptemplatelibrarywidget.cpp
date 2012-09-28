
#include "pickuptemplatelibrarywidget.h"

PickupTemplateLibraryWidget :: PickupTemplateLibraryWidget(QPixmap _view, enum PickupTemplate::Type _type, QWidget* _parent): LibraryItem(_parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(_view);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->type = _type;
}

