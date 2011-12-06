
#include "colorbarlibrarywidget.h"
#include "qgraphicshighlighteffect.h"

ColorBarLibraryWidget :: ColorBarLibraryWidget(PullPlan* plan, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

        QPixmap pixmap(100, 100);
        pixmap.fill(Qt::white);
	setPixmap(pixmap);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
        this->editorPixmap = pixmap;

	this->pullPlan = plan;
	this->pullPlan->setColorLibraryWidget(this);
	setGraphicsEffect(new QGraphicsHighlightEffect());
	connect(graphicsEffect(), SIGNAL(enabledChanged(bool)), graphicsEffect(), SLOT(setStyleSheet(bool)));
	connect(graphicsEffect(), SIGNAL(styleSheetString(QString)), this, SLOT(setStyleSheet(QString)));
}

const QPixmap* ColorBarLibraryWidget :: getEditorPixmap()
{
        return &(this->editorPixmap);
}

void ColorBarLibraryWidget :: updatePixmaps(QPixmap niceViewPixmap, QPixmap editorPixmap)
{
	setPixmap(niceViewPixmap);
	this->editorPixmap = editorPixmap;
}

PullPlan* ColorBarLibraryWidget :: getPullPlan()
{
	return pullPlan;
}




