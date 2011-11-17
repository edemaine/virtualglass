
#include "colorbarlibrarywidget.h"

ColorBarLibraryWidget :: ColorBarLibraryWidget(PullPlan* plan, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	QImage image("./checkerboard.png");

	QPainter painter(&image);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.fillRect(image.rect(), QBrush(QColor(255*plan->color.r, 255*plan->color.g, 255*plan->color.b, 255*plan->color.a)));
	painter.end();

	setPixmap(QPixmap::fromImage(image.scaled(100, 100)));
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	this->pullPlan = plan;
	setGraphicsEffect(new QGraphicsColorizeEffect());
	graphicsEffect()->setEnabled(false);
}

PullPlan* ColorBarLibraryWidget :: getPullPlan()
{
	return pullPlan;
}


