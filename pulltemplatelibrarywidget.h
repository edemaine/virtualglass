#ifndef PULLTEMPLATELIBRARYWIDGET_H
#define PULLTEMPLATELIBRARYWIDGET_H

#include "pulltemplate.h"
#include "templatelibrarywidget.h"

class PullTemplateLibraryWidget : public TemplateLibraryWidget
{
	public:
		PullTemplateLibraryWidget(QPixmap view, enum PullTemplate::Type type, QWidget* parent=0);
		enum PullTemplate::Type type;
};

#endif
