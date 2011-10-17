

#include "pulltemplategraphicsview.h"

PullTemplateGraphicsView :: PullTemplateGraphicsView(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	setAcceptDrops(true);
	setFixedWidth(400);
	setFixedHeight(400);
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
		if (fabs(event->pos().x() - (200.0 * subpull->location.x + 200.0)) 
			+ fabs(event->pos().y() - (200.0 * subpull->location.y + 200.0)) < (subpull->diameter/2.0)*200.0)
		{
			event->accept();
			//QString* colors = event->mimeData()->text();
			float r = (rand() % 10) / 10.0;
			float g = (rand() % 10) / 10.0;
			float b = (rand() % 10) / 10.0;
			plan->getSubplans()[i] = new PullPlan();
			plan->getSubplans()[i]->isBase = true;
			plan->getSubplans()[i]->setColor(r, g, b, 0.7);
			repaint();
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

	painter.drawEllipse(0, 0, 400, 400);

	pen.setWidth(5);
	painter.setPen(pen);
	
	for (unsigned int i = 0; i < plan->getTemplate()->subpulls.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->getTemplate()->subpulls[i]);
		if (plan->getSubplans()[i]->isBase)
		{
			Color c = plan->getSubplans()[i]->getColor();
			pen.setColor(QColor(c.r*255, c.g*255, c.b*255));
		}
		else
			pen.setColor(Qt::white);
		painter.setPen(pen);
		int x = (1.0 + subpull->location.x - subpull->diameter/2.0) * 200;
 		int y = (1.0 + subpull->location.y - subpull->diameter/2.0) * 200;
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



