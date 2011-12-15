
#include "piecetemplatelibrarywidget.h"

PieceTemplateLibraryWidget :: PieceTemplateLibraryWidget(QPixmap view, int type, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(view);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->pieceTemplateType = type;

        setGraphicsEffect(new QGraphicsHighlightEffect());
        connect(graphicsEffect(), SIGNAL(enabledChanged(bool)), graphicsEffect(), SLOT(setStyleSheet(bool)));
        connect(graphicsEffect(), SIGNAL(styleSheetString(QString)), this, SLOT(setStyleSheet(QString)));
}



int PieceTemplateLibraryWidget :: getPieceTemplateType()
{
	return pieceTemplateType;
}


