#ifndef PULLTEMPLATELIBRARYWIDGET_H
#define PULLTEMPLATELIBRARYWIDGET_H

#include "canetemplate.h"
#include "templatelibrarywidget.h"

class PullTemplateLibraryWidget : public TemplateLibraryWidget
{
	public:
		PullTemplateLibraryWidget(QPixmap view, enum CaneTemplate::Type type, QWidget* parent=0);
		enum CaneTemplate::Type type;
};

#endif
