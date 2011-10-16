

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
			
			return;	
		}
	} 
}

void PullTemplateGraphicsView :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(event->rect(), Qt::black);
	painter.setPen(Qt::white);
	
	vector<Point> locs = plan->getTemplate()->locations;
	vector<float> dias = plan->getTemplate()->diameters;
	for (unsigned int i = 0; i < locs.size(); ++i)
	{
		QBrush brush;
		painter.drawEllipse((1.0 + locs[i].x - dias[i]/2.0) * 200.0, (1.0 + locs[i].y - dias[i]/2.0) * 200.0, 
			dias[i]*200.0, dias[i]*200.0);
	}

	painter.end();
}



