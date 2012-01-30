#include "pullplancustomizeviewwidget.h"

PullPlanCustomizeViewWidget::PullPlanCustomizeViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setFixedSize(500, 500);
	setPullPlan(plan);
	Color* color = new Color;
	color->r = color->g = color->b = 1.0;
	color->a = 1.0;
	tempCirclePlan = new PullPlan(CUSTOM_CIRCLE_PULL_TEMPLATE,color);
	tempSquarePlan = new PullPlan(CUSTOM_SQUARE_PULL_TEMPLATE,color);
}

void PullPlanCustomizeViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PullPlanCustomizeViewWidget :: dropEvent(QDropEvent* event)
{
	event->setDropAction(Qt::CopyAction);
}

void PullPlanCustomizeViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
	int drawSize = width() - 20;
	// check to see if the drop was in a subpull
	bool anyHit = false;
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->subs[i]);

		// Determine if drop hit the subplan
		bool hit = false;
		float dx = fabs(event->pos().x() - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
		float dy = fabs(event->pos().y() - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
		switch (subpull->shape)
		{
			case CIRCLE_SHAPE:
				if (pow(double(dx*dx + dy*dy), 0.5) < (subpull->diameter/2.0)*drawSize/2)
				{
					hit = true;
					anyHit = true;
					if (hoveringIndex != (int)i)
					{
						if (hoveringIndex != -1)
						{
							plan->subs[hoveringIndex].plan = hoveringPlan;
						}
						hoveringIndex = i;
						hoveringPlan = subpull->plan;
						plan->subs[i].plan = tempCirclePlan;
					}
				}
				break;
			case SQUARE_SHAPE:
				if (MAX(dx, dy) < (subpull->diameter/2.0)*drawSize/2)
				{
					hit = true;
					anyHit = true;
					if (hoveringIndex != (int)i)
					{
						if (hoveringIndex != -1)
						{
							plan->subs[hoveringIndex].plan = hoveringPlan;
						}
						hoveringIndex = i;
						hoveringPlan = subpull->plan;
						plan->subs[i].plan = tempSquarePlan;
					}
				}
				break;
		}

		if (!hit)
		{
			continue;
		}

		event->accept();
		this->update();
		return;
	}
	plan->subs[hoveringIndex].plan = hoveringPlan;
	hoveringIndex = -1;
	this->update();
	return;
}

void PullPlanCustomizeViewWidget :: setPullPlan(PullPlan* plan)
{
	this->plan = plan->copy();
	if (plan->getCasingShape() == SQUARE_SHAPE)
	{
		this->plan->setTemplateType(CUSTOM_SQUARE_PULL_TEMPLATE);
	}
	else
	{
		this->plan->setTemplateType(CUSTOM_CIRCLE_PULL_TEMPLATE);
	}
	this->plan->subs = plan->subs;
	hoveringIndex = -1;
}


void PullPlanCustomizeViewWidget :: drawSubplan(float x, float y, float drawWidth, float drawHeight,
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
	if (plan->isBase())
	{
		Color* c = plan->getColor();
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
	painter->setBrush(QColor(255*plan->getColor()->r, 255*plan->getColor()->g, 255*plan->getColor()->b, 255*plan->getColor()->a));
	switch (mandatedShape)
	{
		case CIRCLE_SHAPE:
			painter->drawEllipse(x, y, drawWidth, drawHeight);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(x, y, drawWidth, drawHeight);
			break;
	}

	if (plan->isBase())
		return;

	// Recurse
	for (unsigned int i = plan->subs.size()-1; i < plan->subs.size(); --i)
	{
		SubpullTemplate* sub = &(plan->subs[i]);

		float rX = x + (sub->location.x - sub->diameter/2.0) * drawWidth/2 + drawWidth/2;
		float rY = y + (sub->location.y - sub->diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = sub->diameter * drawWidth/2;
		float rHeight = sub->diameter * drawHeight/2;

		drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, plan->subs[i].shape,
			borderLevels-1, painter);
	}
}

void PullPlanCustomizeViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(event->rect(), QColor(200, 200, 200));
	drawSubplan(10, 10, width() - 20, height() - 20, plan, plan->getCasingShape(), 2, &painter);
	painter.end();
}

void PullPlanCustomizeViewWidget :: revertAndClose()
{
	if (hoveringIndex != -1)
	{
		plan->subs[hoveringIndex].plan = hoveringPlan;
		hoveringIndex = -1;
	}
	//revertAllChanges();
	this->setMouseTracking(false);
}
