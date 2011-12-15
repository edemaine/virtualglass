
#include "pullplaneditorviewwidget.h"

PullPlanEditorViewWidget :: PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setFixedSize(500, 500);
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
	if (!(type == COLOR_BAR_MIME || type == PULL_PLAN_MIME))
		return;

	if (droppedPlan->hasDependencyOn(plan)) // don't allow circular DAGs
		return;

	int drawSize = width() - 20;
	// check to see if the drop was in a subpull
	for (unsigned int i = 0; i < plan->getTemplate()->subtemps.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->getTemplate()->subtemps[i]);

		// Determine if drop hit the subplan
		bool hit = false;
		float dx = fabs(event->pos().x() - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
		float dy = fabs(event->pos().y() - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
		switch (subpull->shape)
		{
			case CIRCLE_SHAPE:
				if (pow(dx*dx + dy*dy, 0.5) < (subpull->diameter/2.0)*drawSize/2)
					hit = true;
				break;
			case SQUARE_SHAPE:
				if (MAX(dx, dy) < (subpull->diameter/2.0)*drawSize/2)
					hit = true;
				break;	
		}
		
		if (!hit)
			continue;

		// If the dropped plan is a complex plan and its casing shape doesn't match the shape of the
		// subplan, reject
		if (type == PULL_PLAN_MIME)
		{
			if (subpull->shape != droppedPlan->getTemplate()->getShape())
			{
				continue;
			}
		}

		event->accept();

		// If the shift button is down, fill in the entire group
		if (event->keyboardModifiers() & 0x02000000)
		{
			int group = plan->getTemplate()->subtemps[i].group;
			for (unsigned int j = 0; j < plan->getTemplate()->subtemps.size(); ++j)
			{
				if (plan->getTemplate()->subtemps[j].group == group)
					plan->subplans[j] = droppedPlan;
			}
		}
		// If the alt button is down, fill alternating elements in the group
		else if (event->keyboardModifiers() & 0x08000000)
		{
			int group = plan->getTemplate()->subtemps[i].group;
			bool parity = true;
			unsigned int subtempCount = plan->getTemplate()->subtemps.size();
			unsigned int j = i;
			do
			{
				if (plan->getTemplate()->subtemps[j].group == group)
				{
					if (parity)
						plan->subplans[j] = droppedPlan;
					parity = !parity;
				}
				++j;
				j = j % subtempCount;
			}
			while ((j != ((i-1 + subtempCount) % subtempCount)) && (j != i));
		}
		else // Otherwise just fill in this one
			plan->subplans[i] = droppedPlan;

		emit someDataChanged();
		return;
	}


	// don't allow complex pulls to be casing
	if (type == PULL_PLAN_MIME)
		return;

	// Deal w/casing
	float distanceFromCenter;
	switch (plan->getTemplate()->getShape())
	{
		case CIRCLE_SHAPE:
			distanceFromCenter = sqrt(pow(event->pos().x() - drawSize/2 + 10, 2) 
				+ pow(event->pos().y() - drawSize/2 + 10, 2));
			if (distanceFromCenter <= drawSize/2)
			{
				event->accept();
				plan->color = droppedPlan->color;
				emit someDataChanged();
				return;
			}
			break;
		case SQUARE_SHAPE:
			if (10 <= event->pos().x() && event->pos().x() <= drawSize 
				&& 10 <= event->pos().y() && event->pos().y() <= drawSize)
			{
				event->accept();
				plan->color = droppedPlan->color;
				emit someDataChanged();
				return;
			}
			break;
	}
}

void PullPlanEditorViewWidget :: setPullPlan(PullPlan* plan)
{
	this->plan = plan;
}


void PullPlanEditorViewWidget :: drawSubplan(float x, float y, float drawWidth, float drawHeight, 
	PullPlan* plan, int mandatedShape, int borderLevels, QPainter* painter)
{
	// Fill the subplan area with some `cleared out' color
	painter->setBrush(QColor(200, 200, 200));
	painter->setPen(Qt::NoPen);
	switch (mandatedShape)
	{
		case CIRCLE_SHAPE:
			painter->drawEllipse(x, y, drawWidth, drawHeight);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(x, y, drawWidth, drawHeight);
			break;
	}


	// If it's a base color, fill region with color
	if (plan->getTemplate()->isBase())
	{
		Color* c = plan->color;
		painter->setBrush(QColor(255*c->r, 255*c->g, 255*c->b, 255*c->a));
		painter->setPen(Qt::NoPen);

		switch (mandatedShape)
		{
			case CIRCLE_SHAPE:
				painter->drawEllipse(x, y, drawWidth, drawHeight);
				break;
			case SQUARE_SHAPE:
				painter->drawRect(x, y, drawWidth, drawHeight);
				break;
		}
	}

	// Draw casing shape
	painter->setBrush(Qt::NoBrush);
	if (MIN(drawWidth, drawHeight) < 10)
	{
                painter->setPen(Qt::NoPen);
	}
	else if (borderLevels > 0)
	{
		QPen pen;
		pen.setWidth(borderLevels*2+1);
		pen.setColor(Qt::black);
		painter->setPen(pen);
	}
	else
	{
		QPen pen;
		pen.setWidth(2);
		pen.setColor(Qt::black);
		pen.setStyle(Qt::DotLine);
		painter->setPen(pen);
	}
	painter->setBrush(QColor(255*plan->color->r, 255*plan->color->g, 255*plan->color->b, 255*plan->color->a));
	switch (mandatedShape)
	{
		case CIRCLE_SHAPE:
			painter->drawEllipse(x, y, drawWidth, drawHeight);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(x, y, drawWidth, drawHeight);
			break;
	}

	if (plan->getTemplate()->isBase())
		return;

	// Recurse
	for (unsigned int i = plan->getTemplate()->subtemps.size()-1; i < plan->getTemplate()->subtemps.size(); --i)
	{
		SubpullTemplate* subpull = &(plan->getTemplate()->subtemps[i]);

		float rX = x + (subpull->location.x - subpull->diameter/2.0) * drawWidth/2 + drawWidth/2;
		float rY = y + (subpull->location.y - subpull->diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = subpull->diameter * drawWidth/2;
		float rHeight = subpull->diameter * drawHeight/2;

		drawSubplan(rX, rY, rWidth, rHeight, plan->subplans[i], plan->getTemplate()->subtemps[i].shape, 
			borderLevels-1, painter);
	}
}

void PullPlanEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(event->rect(), QColor(200, 200, 200));
	drawSubplan(10, 10, width() - 20, height() - 20, plan, plan->getTemplate()->getShape(), 2, &painter);
	painter.end();
}



