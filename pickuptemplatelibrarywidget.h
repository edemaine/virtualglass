#ifndef PICKUPTEMPLATELIBRARYWIDGET_H
#define PICKUPTEMPLATELIBRARYWIDGET_H

#include <QtGui>
#include "pickuptemplate.h"
#include "libraryitem.h"

class PickupTemplateLibraryWidget : public LibraryItem
{
	public:
		PickupTemplateLibraryWidget(QPixmap view, enum PickupTemplate::Type t, QWidget* parent=0);
		enum PickupTemplate::Type type;
};

#endif
