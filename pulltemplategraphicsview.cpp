

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

	vector<Point> locs = plan->getTemplate()->locations;
	vector<float> dias = plan->getTemplate()->diameters;
	for (unsigned int i = 0; i < locs.size(); ++i)
	{
		if (fabs(event->pos().x() - (200.0 * locs[i].x + 200.0)) 
			+ fabs(event->pos().y() - (200.0 * locs[i].y + 200.0)) < (dias[i]/2.0)*200.0)
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
	
	vector<Point> locs = plan->getTemplate()->locations;
	vector<float> dias = plan->getTemplate()->diameters;
	for (unsigned int i = 0; i < locs.size(); ++i)
	{
		if (plan->getSubplans()[i]->isBase)
		{
			Color c = plan->getSubplans()[i]->getColor();
			pen.setColor(QColor(c.r*255, c.g*255, c.b*255));
		}
		else
			pen.setColor(Qt::white);
		painter.setPen(pen);	
		painter.drawEllipse((1.0 + locs[i].x - dias[i]/2.0) * 200.0, (1.0 + locs[i].y - dias[i]/2.0) * 200.0, 
			dias[i]*200.0, dias[i]*200.0);
	}

	painter.end();
}



