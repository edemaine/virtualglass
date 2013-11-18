
#include "canetemplatelibrarywidget.h"

CaneTemplateLibraryWidget :: CaneTemplateLibraryWidget(QPixmap _view, 
	enum CaneTemplate::Type _type, QWidget* _parent): TemplateLibraryWidget(_view, _parent)
{
	this->type = _type;
}


