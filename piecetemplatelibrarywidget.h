#ifndef PIECETEMPLATELIBRARYWIDGET_H
#define PIECETEMPLATELIBRARYWIDGET_H

#include <QtGui>
#include "piecetemplate.h"
#include "librarywidget.h"

class PieceTemplateLibraryWidget : public LibraryWidget
{
	public:
		PieceTemplateLibraryWidget(QPixmap view, enum PieceTemplate::Type t, QWidget* parent=0);
		enum PieceTemplate::Type type;
};

#endif
