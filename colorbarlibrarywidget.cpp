
#include "colorbarlibrarywidget.h"

ColorBarLibraryWidget :: ColorBarLibraryWidget(QPixmap niceViewPixmap, PullPlan* plan, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	QImage image("./checkerboard.png");

	QPainter painter(&image);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawPixmap(image.rect(), niceViewPixmap, niceViewPixmap.rect());
	painter.end();

	setPixmap(QPixmap::fromImage(image.scaled(100, 100)));
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	this->pullPlan = plan;
	setGraphicsEffect(new QGraphicsColorizeEffect());
	graphicsEffect()->setEnabled(false);
}

void ColorBarLibraryWidget :: updatePixmap(QPixmap niceViewPixmap)
{
	setPixmap(niceViewPixmap);
}

PullPlan* ColorBarLibraryWidget :: getPullPlan()
{
	return pullPlan;
}


