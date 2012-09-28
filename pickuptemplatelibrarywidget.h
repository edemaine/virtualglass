#ifndef PICKUPTEMPLATELIBRARYWIDGET_H
#define PICKUPTEMPLATELIBRARYWIDGET_H

#include <QtGui>
#include "pickuptemplate.h"
#include "librarywidget.h"

class PickupTemplateLibraryWidget : public LibraryWidget
{
	public:
		PickupTemplateLibraryWidget(QPixmap view, enum PickupTemplate::Type t, QWidget* parent=0);
		enum PickupTemplate::Type type;
};

#endif
