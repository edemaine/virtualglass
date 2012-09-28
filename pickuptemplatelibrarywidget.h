#ifndef PICKUPTEMPLATELIBRARYWIDGET_H
#define PICKUPTEMPLATELIBRARYWIDGET_H

#include <QtGui>
#include "pickuptemplate.h"
#include "libraryitemeffect.h"

class PickupTemplateLibraryWidget : public QLabel
{
	public:
		PickupTemplateLibraryWidget(QPixmap view, enum PickupTemplate::Type t, QWidget* parent=0);
		enum PickupTemplate::Type type;
};

#endif
