
#include "pullplanlibrarywidget.h"
#include "qgraphicshighlighteffect.h"

PullPlanLibraryWidget :: PullPlanLibraryWidget(PullPlan* plan,
	QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

        QPixmap pixmap(100, 100);
        pixmap.fill(Qt::white);

	setPixmap(pixmap);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->pullPlan = plan;
	this->pullPlan->setLibraryWidget(this);
	this->editorPixmap = pixmap;

	setGraphicsEffect(new QGraphicsHighlightEffect());
	connect(graphicsEffect(), SIGNAL(enabledChanged(bool)), graphicsEffect(), SLOT(setStyleSheet(bool)));
	connect(graphicsEffect(), SIGNAL(styleSheetString(QString)), this, SLOT(setStyleSheet(QString)));
}

PullPlan* PullPlanLibraryWidget :: getPullPlan()
{
	return pullPlan;
}

void PullPlanLibraryWidget :: updatePixmaps(QPixmap niceViewPixmap, QPixmap editorPixmap)
{
	setPixmap(niceViewPixmap);
	this->editorPixmap = editorPixmap;
}

const QPixmap* PullPlanLibraryWidget :: getEditorPixmap()
{
	return &(this->editorPixmap);
}

