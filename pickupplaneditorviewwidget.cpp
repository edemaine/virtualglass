
#include "pickupplaneditorviewwidget.h"

PickupPlanEditorViewWidget :: PickupPlanEditorViewWidget(PickupPlan* plan, QWidget* parent) : QWidget(parent)
{
	width = 500;
	height = 500;	

	setAcceptDrops(true);
	setFixedWidth(width + 20);
	setFixedHeight(height + 20);
	this->plan = plan;
}

void PickupPlanEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PickupPlanEditorViewWidget :: dropEvent(QDropEvent* event)
{
	event->setDropAction(Qt::CopyAction);

	for (unsigned int i = 0; i < plan->getTemplate()->subpulls.size(); ++i)
	{
		SubpickupTemplate* sp = &(plan->getTemplate()->subpulls[i]);
		Point ul, br;

		switch (sp->orientation)
		{
			case HORIZONTAL_ORIENTATION:
				ul.x = sp->location.x - sp->length/2;	
				ul.y = sp->location.y - sp->width/2;	
				br.x = sp->location.x + sp->length/2;	
				br.y = sp->location.y + sp->width/2;	
				break;		
			case VERTICAL_ORIENTATION:
				ul.x = sp->location.x - sp->width/2;	
				ul.y = sp->location.y - sp->length/2;	
				br.x = sp->location.x + sp->width/2;	
				br.y = sp->location.y + sp->length/2;	
				break;		
		}	

		ul.x = ul.x * width/2 + width/2 + 10;  
		ul.y = ul.y * height/2 + height/2 + 10;  
		br.x = br.x * width/2 + width/2 + 10;  
		br.y = br.y * height/2 + height/2 + 10;  
	
		if (ul.x < event->pos().x() && event->pos().x() < br.x 
			&& ul.y < event->pos().y() && event->pos().y() < br.y)
		{
			event->accept();
		}
		else
			continue;

		PullPlan* ptr;
		sscanf(event->mimeData()->text().toAscii().constData(), "%p", &ptr);
		if (ptr->getTemplate()->shape == AMORPHOUS_SHAPE) // if it's a color bar
		{
			// This is a memory leak, as every drag of a color bar makes a new pull plan
			ptr = new PullPlan(CIRCLE_BASE_TEMPLATE, true, ptr->color);
		}

		int group = plan->getTemplate()->subpulls[i].group;
		for (unsigned int j = 0; j < plan->getTemplate()->subpulls.size(); ++j)
		{
			if (plan->getTemplate()->subpulls[j].group == group)
				plan->setSubplan(j, ptr);
		}
		
		emit someDataChanged();
		return;	
	} 
}

void PickupPlanEditorViewWidget :: setPickupPlan(PickupPlan* plan)
{
	this->plan = plan;
}

void PickupPlanEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.setRenderHint(QPainter::HighQualityAntialiasing);
	painter.begin(this);
	painter.fillRect(event->rect(), Qt::black);
	QPen pen;
	pen.setColor(Qt::white);
	pen.setWidth(3);
	painter.setPen(pen);

	painter.drawRect(10, 10, width, height);
	
	for (unsigned int i = 0; i < plan->getTemplate()->subpulls.size(); ++i)
	{
		if (plan->getSubplans()[i]->isBase)
		{
			Color c = plan->getSubplans()[i]->color;
			painter.setBrush(QColor(255*c.r, 255*c.g, 255*c.b, 255*c.a));
			pen.setStyle(Qt::NoPen);
		}
		else
		{
			painter.setBrush(Qt::NoBrush);
			pen.setColor(Qt::white);
			pen.setStyle(Qt::DotLine);
		}
		painter.setPen(pen);

		int rX, rY, rWidth, rHeight;
		SubpickupTemplate* sp = &(plan->getTemplate()->subpulls[i]);
		switch (sp->orientation)
		{
			case HORIZONTAL_ORIENTATION:
				rX = (sp->location.x - sp->length/2.0) * width/2 + width/2 + 10;
 				rY = (sp->location.y - sp->width/2.0) * width/2 + height/2 + 10;
 				rWidth = sp->length * width/2;
				rHeight = sp->width * height/2;
				break;
			case VERTICAL_ORIENTATION:
				rX = (sp->location.x - sp->width/2.0) * width/2 + width/2 + 10;
 				rY = (sp->location.y - sp->length/2.0) * width/2 + height/2 + 10;
 				rWidth = sp->width * width/2;
				rHeight = sp->length * height/2;
				break;
			default:
				exit(1);
		}

		painter.drawRect(rX, rY, rWidth, rHeight);
	}

	painter.end();
}



