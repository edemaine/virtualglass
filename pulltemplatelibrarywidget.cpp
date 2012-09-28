
#include "pulltemplatelibrarywidget.h"

PullTemplateLibraryWidget :: PullTemplateLibraryWidget(QPixmap _view, enum PullTemplate::Type _type, QWidget* _parent): LibraryItem(_parent)
{
	setFixedSize(100, 100);
	setScaledContents(true);

	setPixmap(_view);
	this->type = _type;
}


