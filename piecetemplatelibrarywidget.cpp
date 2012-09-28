
#include "piecetemplatelibrarywidget.h"

PieceTemplateLibraryWidget :: PieceTemplateLibraryWidget(QPixmap _view, enum PieceTemplate::Type _type, QWidget* _parent): LibraryItem(_parent)
{
	setFixedSize(100, 100);
	setScaledContents(true);
	setPixmap(_view);
	this->type = _type;

}


