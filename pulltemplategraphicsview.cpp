
#include "pulltemplategraphicsview.h"

PullTemplateGraphicsView :: PullTemplateGraphicsView(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	width = 500;
	height = 500;	

	setAcceptDrops(true);
	setFixedWidth(width + 10);
	setFixedHeight(height + 10);
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
		if (fabs(event->pos().x() - (width/2 * subpull->location.x + width/2 + 10)) 
			+ fabs(event->pos().y() - (width/2 * subpull->location.y + width/2 + 10)) < (subpull->diameter/2.0)*width/2)
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

void PullTemplateGraphicsView :: setPullPlan(PullPlan* plan)
{
	this->plan = plan;
}

void PullTemplateGraphicsView :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.setRenderHint(QPainter::HighQualityAntialiasing);
	painter.begin(this);
	painter.fillRect(event->rect(), Qt::black);
	QPen pen;
	pen.setColor(Qt::white);
	pen.setWidth(3);
	painter.setPen(pen);
	painter.drawEllipse(5, 5, width, height);

	
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

		int rX = (subpull->location.x - subpull->diameter/2.0) * width/2 + width/2 + 5;
 		int rY = (subpull->location.y - subpull->diameter/2.0) * width/2 + height/2 + 5;
 		int rWidth = subpull->diameter * width/2;
		int rHeight = subpull->diameter * height/2;

		switch (subpull->shape)
		{
			case CIRCLE_SHAPE:
				painter.drawEllipse(rX, rY, rWidth, rHeight);
				break;	
			case SQUARE_SHAPE:
				painter.drawRect(rX, rY, rWidth, rHeight);
				break;	
		}
	}

	painter.end();
}



