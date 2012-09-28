#ifndef PULLTEMPLATELIBRARYWIDGET_H
#define PULLTEMPLATELIBRARYWIDGET_H

#include <QtGui>
#include "pulltemplate.h"
#include "librarywidget.h"

class PullTemplateLibraryWidget : public LibraryWidget
{
	public:
		PullTemplateLibraryWidget(QPixmap view, enum PullTemplate::Type t, QWidget* parent=0);
		enum PullTemplate::Type type;
};

#endif
