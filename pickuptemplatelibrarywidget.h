#ifndef PICKUPTEMPLATELIBRARYWIDGET_H
#define PICKUPTEMPLATELIBRARYWIDGET_H

#include <QtGui>
#include <QObject>

class PickupTemplateLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		PickupTemplateLibraryWidget(QPixmap view, int pickupTemplateType, QWidget* parent=0);
		int getPickupTemplateType();

	private:
		int pickupTemplateType;
};

#endif
