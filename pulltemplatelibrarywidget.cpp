
#include "pulltemplatelibrarywidget.h"

PullTemplateLibraryWidget :: PullTemplateLibraryWidget(QPixmap _view, enum PullTemplate::Type _type, QWidget* _parent): LibraryItem(_parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);

	setPixmap(_view);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->type = _type;
}


