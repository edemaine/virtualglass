#ifndef PIECETEMPLATELIBRARYWIDGET_H
#define PIECETEMPLATELIBRARYWIDGET_H

#include <QtGui>
#include "qgraphicshighlighteffect.h"

class PieceTemplateLibraryWidget : public QLabel
{
	public:
		PieceTemplateLibraryWidget(QPixmap view, int pieceTemplateType, QWidget* parent=0);
		int getPieceTemplateType();

	private:
		int pieceTemplateType;
};

#endif
