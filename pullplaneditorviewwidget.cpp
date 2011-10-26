
#include "pullplaneditorviewwidget.h"

PullPlanEditorViewWidget :: PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	width = 500;
	height = 500;	

	setAcceptDrops(true);
	setFixedWidth(width + 20);
	setFixedHeight(height + 20);
	this->plan = plan;
}

void PullPlanEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PullPlanEditorViewWidget :: dropEvent(QDropEvent* event)
{
	event->setDropAction(Qt::CopyAction);

	for (unsigned int i = 0; i < plan->getTemplate()->subpulls.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->getTemplate()->subpulls[i]);
		if (fabs(event->pos().x() - (width/2 * subpull->location.x + width/2 + 10)) 
			+ fabs(event->pos().y() - (width/2 * subpull->location.y + width/2 + 10)) < (subpull->diameter/2.0)*width/2)
		{
			PullPlan* ptr;
			sscanf(event->mimeData()->text().toAscii().constData(), "%p", &ptr);

			// Check that there is no mismatch between plan and template
			if (ptr->getTemplate()->shape == AMORPHOUS_SHAPE 
				|| ptr->getTemplate()->shape == plan->getTemplate()->subpulls[i].shape)
				event->accept();
			else
				continue;

			if (ptr->getTemplate()->shape == AMORPHOUS_SHAPE) // if it's a color bar
			{
				switch (plan->getTemplate()->subpulls[i].shape)
				{
					// This is a memory leak, as every drag of a color bar makes a new pull plan
					case CIRCLE_SHAPE:
						ptr = new PullPlan(CIRCLE_BASE_TEMPLATE, true, ptr->color); 
						break;
					case SQUARE_SHAPE:
						ptr = new PullPlan(SQUARE_BASE_TEMPLATE, true, ptr->color); 
						break;
				}
			}

			// Fill in the entire group			
			int group = plan->getTemplate()->subpulls[i].group;
			for (unsigned int j = 0; j < plan->getTemplate()->subpulls.size(); ++j)
			{
				if (plan->getTemplate()->subpulls[j].group == group)
					plan->subplans[j] = ptr;
			}

			emit someDataChanged();
			return;	
		}
	} 
}

void PullPlanEditorViewWidget :: setPullPlan(PullPlan* plan)
{
	this->plan = plan;
}

void PullPlanEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(event->rect(), QColor(200, 200, 200));
	QPen pen;
	pen.setColor(Qt::white);
	pen.setWidth(3);
	painter.setPen(pen);

	switch (plan->getTemplate()->shape)
	{
		case CIRCLE_SHAPE:
			painter.drawEllipse(10, 10, width, height);
			break;
		case SQUARE_SHAPE:
			painter.drawRect(10, 10, width, height);
			break;
	}
	
	for (unsigned int i = 0; i < plan->getTemplate()->subpulls.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->getTemplate()->subpulls[i]);
		if (plan->subplans[i]->isBase)
		{
			Color c = plan->subplans[i]->color;
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

		int rX = (subpull->location.x - subpull->diameter/2.0) * width/2 + width/2 + 10;
 		int rY = (subpull->location.y - subpull->diameter/2.0) * width/2 + height/2 + 10;
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



