
#include "piecetemplatelibrarywidget.h"

PieceTemplateLibraryWidget :: PieceTemplateLibraryWidget(QPixmap _view, 
	enum PieceTemplate::Type _type, QWidget* _parent): TemplateLibraryWidget(_view, _parent)
{
	this->type = _type;
}


