
#ifndef PICKUPTEMPLATELIBRARYWIDGET_H
#define PICKUPTEMPLATELIBRARYWIDGET_H

#include "pickuptemplate.h"
#include "templatelibrarywidget.h"

class PickupTemplateLibraryWidget : public TemplateLibraryWidget
{
	public:
		PickupTemplateLibraryWidget(QPixmap view, enum PickupTemplate::Type t, QWidget* parent=0);
		enum PickupTemplate::Type type;
};

#endif
