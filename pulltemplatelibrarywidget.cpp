
#include "pulltemplatelibrarywidget.h"
#include "qgraphicshighlighteffect.h"

PullTemplateLibraryWidget :: PullTemplateLibraryWidget(QPixmap view, int type, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(view);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->pullTemplateType = type;

        setGraphicsEffect(new QGraphicsHighlightEffect());
        connect(graphicsEffect(), SIGNAL(enabledChanged(bool)), graphicsEffect(), SLOT(setStyleSheet(bool)));
        connect(graphicsEffect(), SIGNAL(styleSheetString(QString)), this, SLOT(setStyleSheet(QString)));
}

int PullTemplateLibraryWidget :: getPullTemplateType()
{
	return pullTemplateType;
}


