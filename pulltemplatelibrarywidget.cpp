
#include "pulltemplatelibrarywidget.h"

PullTemplateLibraryWidget :: PullTemplateLibraryWidget(QPixmap _view, 
	enum PullTemplate::Type _type, QWidget* _parent): TemplateLibraryWidget(_view, _parent)
{
	this->type = _type;
}


