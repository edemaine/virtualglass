
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

	PullPlan* droppedPlan;
	int type;
	sscanf(event->mimeData()->text().toAscii().constData(), "%p %d", &droppedPlan, &type);
	if (type != PULL_PLAN_MIME) // if the thing passed isn't a pull plan (no, you can't put a piece in your pull plan)
		return;  

	if (droppedPlan == plan) // don't allow circular DAGs
		return;

	for (unsigned int i = 0; i < plan->getTemplate()->subpulls.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->getTemplate()->subpulls[i]);
		if (fabs(event->pos().x() - (width/2 * subpull->location.x + width/2 + 10)) 
			+ fabs(event->pos().y() - (width/2 * subpull->location.y + width/2 + 10)) < (subpull->diameter/2.0)*width/2)
		{
			if (droppedPlan->getTemplate()->shape == AMORPHOUS_SHAPE 
				|| droppedPlan->getTemplate()->shape == plan->getTemplate()->subpulls[i].shape)
				event->accept();
			else
				continue;

			if (droppedPlan->getTemplate()->shape == AMORPHOUS_SHAPE) // if it's a color bar
			{
				switch (plan->getTemplate()->subpulls[i].shape)
				{
					// This is a memory leak, as every drag of a color bar makes a new pull plan
					case CIRCLE_SHAPE:
						droppedPlan = new PullPlan(CIRCLE_BASE_TEMPLATE, true, droppedPlan->color); 
						break;
					case SQUARE_SHAPE:
						droppedPlan = new PullPlan(SQUARE_BASE_TEMPLATE, true, droppedPlan->color); 
						break;
				}
			}

			// Fill in the entire group			
			int group = plan->getTemplate()->subpulls[i].group;
			for (unsigned int j = 0; j < plan->getTemplate()->subpulls.size(); ++j)
			{
				if (plan->getTemplate()->subpulls[j].group == group)
					plan->subplans[j] = droppedPlan;
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

	// Draw back to front for subplans contained in other subplans, 
	// which are checked front to back.	
	for (unsigned int i = plan->getTemplate()->subpulls.size()-1; i < plan->getTemplate()->subpulls.size(); --i)
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



