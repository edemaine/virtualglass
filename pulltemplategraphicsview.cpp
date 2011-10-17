

#include "pulltemplategraphicsview.h"

PullTemplateGraphicsView :: PullTemplateGraphicsView(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	setAcceptDrops(true);
	setFixedWidth(410);
	setFixedHeight(410);
	this->plan = plan;
	
}

void PullTemplateGraphicsView :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PullTemplateGraphicsView :: dropEvent(QDropEvent* event)
{
	event->setDropAction(Qt::CopyAction);

	for (unsigned int i = 0; i < plan->getTemplate()->subpulls.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->getTemplate()->subpulls[i]);
		if (fabs(event->pos().x() - (200.0 * subpull->location.x + 205.0)) 
			+ fabs(event->pos().y() - (200.0 * subpull->location.y + 205.0)) < (subpull->diameter/2.0)*200.0)
		{
			event->accept();
			PullPlan* ptr;
			sscanf(event->mimeData()->text().toAscii().constData(), "%p", &ptr);
			plan->setSubplan(i, ptr);
			emit someDataChanged();
			return;	
		}
	} 
}

void PullTemplateGraphicsView :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.setRenderHint(QPainter::HighQualityAntialiasing);
	painter.begin(this);
	painter.fillRect(event->rect(), Qt::black);
	QPen pen;
	pen.setColor(Qt::white);
	painter.setPen(pen);

	painter.drawEllipse(5, 5, 400, 400);

	pen.setWidth(5);
	painter.setPen(pen);
	
	for (unsigned int i = 0; i < plan->getTemplate()->subpulls.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->getTemplate()->subpulls[i]);
		if (plan->getSubplans()[i]->isBase)
		{
			Color c = plan->getSubplans()[i]->getColor();
			painter.setBrush(QColor(c.r, c.g, c.b, c.a));
			pen.setStyle(Qt::NoPen);
		}
		else
		{
			painter.setBrush(Qt::NoBrush);
			pen.setColor(Qt::white);
			pen.setStyle(Qt::DotLine);
		}
		painter.setPen(pen);

		int x = (subpull->location.x - subpull->diameter/2.0) * 200 + 205;
 		int y = (subpull->location.y - subpull->diameter/2.0) * 200 + 205;
 		int width = subpull->diameter * 200;
		int height = subpull->diameter * 200;

		switch (subpull->shape)
		{
			case CIRCLE_SHAPE:
				painter.drawEllipse(x, y, width, height);
				break;	
			case SQUARE_SHAPE:
				painter.drawRect(x, y, width, height);
				break;	
		}
	}

	painter.end();
}



