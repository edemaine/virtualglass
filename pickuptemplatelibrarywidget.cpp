
#include "pickuptemplatelibrarywidget.h"

PickupTemplateLibraryWidget :: PickupTemplateLibraryWidget(QPixmap _view, enum PickupTemplate::Type _type, QWidget* _parent): LibraryWidget(_parent)
{
	setFixedSize(100, 100);
	setScaledContents(true);
	setPixmap(_view);
	this->type = _type;
}

