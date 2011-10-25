
#include "pulltemplatelibrarywidget.h"

PullTemplateLibraryWidget :: PullTemplateLibraryWidget(QPixmap view, int type, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(view);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->pullTemplateType = type;
}

int PullTemplateLibraryWidget :: getPullTemplateType()
{
	return pullTemplateType;
}


