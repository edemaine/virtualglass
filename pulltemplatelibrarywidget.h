#ifndef PULLTEMPLATELIBRARYWIDGET_H
#define PULLTEMPLATELIBRARYWIDGET_H

#include <QtGui>
#include "pulltemplate.h"

class PullTemplateLibraryWidget : public QLabel
{
	public:
		PullTemplateLibraryWidget(QPixmap view, enum PullTemplate::Type t, QWidget* parent=0);
		enum PullTemplate::Type type;
};

#endif
