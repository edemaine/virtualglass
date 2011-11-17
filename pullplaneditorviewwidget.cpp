
#include "pullplaneditorviewwidget.h"

PullPlanEditorViewWidget :: PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	setAcceptDrops(true);
	setFixedWidth(500);
	setFixedHeight(500);
	this->plan = plan;
}

int PullPlanEditorViewWidget :: heightForWidth(int w)
{
	return w;
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

	int drawWidth = width() - 20;
	//int drawHeight = height() - 20;
	for (unsigned int i = 0; i < plan->getTemplate()->subpulls.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->getTemplate()->subpulls[i]);
		if (fabs(event->pos().x() - (drawWidth/2 * subpull->location.x + drawWidth/2 + 10))
			+ fabs(event->pos().y() - (drawWidth/2 * subpull->location.y + drawWidth/2 + 10)) < (subpull->diameter/2.0)*drawWidth/2)
		{
			event->accept();

			// If the shift button is down, fill in the entire group
			if (event->keyboardModifiers() & 0x02000000)
			{	
				int group = plan->getTemplate()->subpulls[i].group;
				for (unsigned int j = 0; j < plan->getTemplate()->subpulls.size(); ++j)
				{
					if (plan->getTemplate()->subpulls[j].group == group)
						plan->subplans[j] = droppedPlan;
				}
			}
			else // Otherwise just fill in this one
				plan->subplans[i] = droppedPlan;

			emit someDataChanged();
			return;
		}
	}
}

void PullPlanEditorViewWidget :: setPullPlan(PullPlan* plan)
{
	this->plan = plan;
}


void PullPlanEditorViewWidget :: drawSubplan(int x, int y, int drawWidth, int drawHeight, PullPlan* plan, QPainter* painter)
{
	// Draw casing shape
	painter->setBrush(Qt::NoBrush);
	QPen pen;
	pen.setWidth(5);
	pen.setColor(Qt::black);
	painter->setPen(pen);
	switch (plan->getTemplate()->shape)
	{
		case CIRCLE_SHAPE:
			painter->drawEllipse(x, y, drawWidth, drawHeight);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(x, y, drawWidth, drawHeight);
			break;
	}
	
	// If it's a base color, fill region with color
	if (plan->isBase)
	{
		Color c = plan->color;
		painter->setBrush(QColor(255*c.r, 255*c.g, 255*c.b, 255*c.a));
		painter->setPen(Qt::NoPen);
		switch (plan->getTemplate()->shape)
		{
			case CIRCLE_SHAPE:
				painter->drawEllipse(x, y, drawWidth, drawHeight);
				break;
			case SQUARE_SHAPE:
				painter->drawRect(x, y, drawWidth, drawHeight);
				break;
		}
		return;
	}

	// Recurse 
	for (unsigned int i = plan->getTemplate()->subpulls.size()-1; i < plan->getTemplate()->subpulls.size(); --i)
	{
		SubpullTemplate* subpull = &(plan->getTemplate()->subpulls[i]);
		
		int rX = x + (subpull->location.x - subpull->diameter/2.0) * drawWidth/2 + drawWidth/2;
		int rY = y + (subpull->location.y - subpull->diameter/2.0) * drawWidth/2 + drawHeight/2;
		int rWidth = subpull->diameter * drawWidth/2;
		int rHeight = subpull->diameter * drawHeight/2;
		
		drawSubplan(rX, rY, rWidth, rHeight, plan->subplans[i], painter);
	}	
}

void PullPlanEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(event->rect(), QColor(200, 200, 200));
	drawSubplan(10, 10, width() - 20, height() - 20, plan, &painter);
	painter.end();
}



