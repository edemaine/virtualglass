
#ifndef CANETEMPLATELIBRARYWIDGET_H
#define CANETEMPLATELIBRARYWIDGET_H

#include "canetemplate.h"
#include "templatelibrarywidget.h"

class CaneTemplateLibraryWidget : public TemplateLibraryWidget
{
	public:
		CaneTemplateLibraryWidget(QPixmap view, enum CaneTemplate::Type type, QWidget* parent=0);
		enum CaneTemplate::Type type;
};

#endif
