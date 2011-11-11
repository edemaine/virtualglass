
#include "colorbarlibrarywidget.h"

ColorBarLibraryWidget :: ColorBarLibraryWidget(Color color, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	QPixmap image(100, 100);
	image.fill(QColor(255*color.r, 255*color.g, 255*color.b, 255*color.a));
	setPixmap(image);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	this->pullPlan = new PullPlan(AMORPHOUS_BASE_TEMPLATE, true, color);
	setGraphicsEffect(new QGraphicsColorizeEffect());
	graphicsEffect()->setEnabled(false);
}

PullPlan* ColorBarLibraryWidget :: getPullPlan()
{
	return pullPlan;
}

