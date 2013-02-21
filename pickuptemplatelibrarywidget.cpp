
#include "pickuptemplatelibrarywidget.h"

PickupTemplateLibraryWidget :: PickupTemplateLibraryWidget(QPixmap _view, 
	enum PickupTemplate::Type _type, QWidget* _parent): TemplateLibraryWidget(_view, _parent)
{
	this->type = _type;
}

