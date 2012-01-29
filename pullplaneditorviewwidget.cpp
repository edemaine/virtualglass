
#include "pullplaneditorviewwidget.h"

PullPlanEditorViewWidget :: PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setFixedSize(500, 500);
	this->plan = plan;
	fill_rule = SINGLE_FILL_RULE;
	isDraggingCasing = false;
}

int PullPlanEditorViewWidget :: getFillRule()
{
	return fill_rule;
}

void PullPlanEditorViewWidget :: setFillRule(int r)
{
	fill_rule = r;
}

void PullPlanEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PullPlanEditorViewWidget :: mousePressEvent(QMouseEvent* event)
{
	float x = (event->pos().x() - width()/2);
	float y = (event->pos().y() - height()/2);
	float radius = sqrt(x * x + y * y) / (width()/2 - 10); 

	if (fabs(radius - (1.0 - plan->getCasingThickness())) < 20)
	{
		isDraggingCasing = true; 
	}
}

void PullPlanEditorViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
	float x = (event->pos().x() - width()/2);
	float y = (event->pos().y() - height()/2);
	if (isDraggingCasing)
	{
		float radius = sqrt(x * x + y * y) / (width()/2 - 10); 
		plan->setCasingThickness(MAX(1.0 - radius, 0));
		emit someDataChanged();
	}
}

void PullPlanEditorViewWidget :: mouseReleaseEvent(QMouseEvent* /*event*/)
{
	isDraggingCasing = false;
}

void PullPlanEditorViewWidget :: dropEvent(QDropEvent* event)
{
	// deactivate any highlighting
	for (unsigned int i = 0; i<plan->subs.size(); ++i)
	{
		plan->deactivate((int)i);
	}
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
			if (subpull->shape != droppedPlan->getShape())
			{
				continue;
			}
		}

		event->accept();

		// If the shift button is down, fill in the entire group
		switch (fill_rule)
		{
			case SINGLE_FILL_RULE:
			{
				plan->subs[i].plan = droppedPlan;
				break;
			}
			case ALL_FILL_RULE:
			{
				for (unsigned int j = 0; j < plan->subs.size(); ++j)
					plan->subs[j].plan = droppedPlan;
				break;
			}
			case GROUP_FILL_RULE:
			{
				int group = plan->subs[i].group;
				for (unsigned int j = i; j < plan->subs.size(); ++j)
				{
					if (plan->subs[j].group == group)
						plan->subs[j].plan = droppedPlan;
				}
				break;
			}
			case EVERY_OTHER_FILL_RULE:
			{
				int group = plan->subs[i].group;
				bool parity = true;
                                for (unsigned int j = i; j < plan->subs.size(); ++j)
                                {
                                        if (plan->subs[j].group == group)
					{
						if (parity)
							plan->subs[j].plan = droppedPlan;
						parity = !parity;
					}
                                }
				break;
			}
                        case EVERY_THIRD_FILL_RULE:
                        {
                                int group = plan->subs[i].group;
                                int triarity = 0;
                                for (unsigned int j = i; j < plan->subs.size(); ++j)
                                {
                                        if (plan->subs[j].group == group)
                                        {       
                                                if (triarity == 0)
                                                        plan->subs[j].plan = droppedPlan;
                                                triarity = (triarity + 1) % 3;
                                        }
                                }
				break;
                        }
		}

		emit someDataChanged();
		return;
	}

	// don't allow complex pulls to be casing
	if (type == PULL_PLAN_MIME)
		return;

	// Deal w/casing
	float distanceFromCenter;
	switch (plan->getShape())
	{
		case CIRCLE_SHAPE:
			distanceFromCenter = sqrt(pow(double(event->pos().x() - drawSize/2 + 10), 2.0) 
				+ pow(double(event->pos().y() - drawSize/2 + 10), 2.0));
			if (distanceFromCenter <= drawSize/2)
			{
				event->accept();
				plan->setColor(droppedPlan->getColor());
				emit someDataChanged();
				return;
			}
			break;
		case SQUARE_SHAPE:
			if (10 <= event->pos().x() && event->pos().x() <= drawSize 
				&& 10 <= event->pos().y() && event->pos().y() <= drawSize)
			{
				event->accept();
				plan->setColor(droppedPlan->getColor());
				emit someDataChanged();
				return;
			}
			break;
	}
}

void PullPlanEditorViewWidget :: dragMoveEvent(QDragMoveEvent* event)
{
	// deactivate any highlighting
	for (unsigned int i = 0; i<plan->subs.size(); ++i)
	{
		plan->deactivate((int)i);
	}
	PullPlan* droppedPlan;
	int type;
	sscanf(event->mimeData()->text().toAscii().constData(), "%p %d", &droppedPlan, &type);
	if (!(type == COLOR_BAR_MIME || type == PULL_PLAN_MIME))
		return;

	if (droppedPlan->hasDependencyOn(plan)) // don't allow circular DAGs
		return;

	int drawSize = width() - 20;
	// check to see if the drop was in a subpull
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
					hit = true;
				break;
			case SQUARE_SHAPE:
				if (MAX(dx, dy) < (subpull->diameter/2.0)*drawSize/2)
					hit = true;
				break;
		}

		if (!hit)
		{
			plan->deactivate((int)i);
			continue;
		}

		// If the dropped plan is a complex plan and its casing shape doesn't match the shape of the
		// subplan, reject
		if (type == PULL_PLAN_MIME)
		{
			if (subpull->shape != droppedPlan->getShape())
			{
				plan->deactivate((int)i);
				continue;
			}
		}

		event->accept();

		// If the shift button is down, fill in the entire group
		switch (fill_rule)
		{
			case SINGLE_FILL_RULE:
			{
				plan->activate((int)i);
				break;
			}
			case ALL_FILL_RULE:
			{
				for (unsigned int j = 0; j < plan->subs.size(); ++j)
					plan->activate((int)j);
				break;
			}
			case GROUP_FILL_RULE:
			{
				int group = plan->subs[i].group;
				for (unsigned int j = i; j < plan->subs.size(); ++j)
				{
					if (plan->subs[j].group == group)
						plan->activate((int)j);
				}
				break;
			}
			case EVERY_OTHER_FILL_RULE:
			{
				int group = plan->subs[i].group;
				bool parity = true;
				for (unsigned int j = i; j < plan->subs.size(); ++j)
				{
					if (plan->subs[j].group == group)
					{
						if (parity)
							plan->activate((int)j);
						parity = !parity;
					}
				}
				break;
			}
						case EVERY_THIRD_FILL_RULE:
						{
								int group = plan->subs[i].group;
								int triarity = 0;
								for (unsigned int j = i; j < plan->subs.size(); ++j)
								{
										if (plan->subs[j].group == group)
										{
												if (triarity == 0)
														plan->activate((int)j);
												triarity = (triarity + 1) % 3;
										}
								}
						break;
			}
		}

		this->update();
		return;
	}

	// don't allow complex pulls to be casing
	if (type == PULL_PLAN_MIME)
	{
//		plan->setActivated(false);
		this->update();
		return;
	}

	// Deal w/casing
	float distanceFromCenter;
	switch (plan->getShape())
	{
		case CIRCLE_SHAPE:
			distanceFromCenter = sqrt(pow(double(event->pos().x() - drawSize/2 + 10), 2.0)
				+ pow(double(event->pos().y() - drawSize/2 + 10), 2.0));
			if (distanceFromCenter <= drawSize/2)
			{
				event->accept();
//				plan->setActivated(true);
				this->update();
				return;
			}
			break;
		case SQUARE_SHAPE:
			if (10 <= event->pos().x() && event->pos().x() <= drawSize
				&& 10 <= event->pos().y() && event->pos().y() <= drawSize)
			{
				event->accept();
//				plan->setActivated(true);
				this->update();
				return;
			}
			break;
	}
	this->update();
}

void PullPlanEditorViewWidget :: setPullPlan(PullPlan* plan)
{
	this->plan = plan;
}


void PullPlanEditorViewWidget :: drawSubplan(float x, float y, float drawWidth, float drawHeight, 
	PullPlan* plan, int mandatedShape, int borderLevels, QPainter* painter, int index=-1)
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
		if (this->plan->isActivated(index))
			pen.setColor(Qt::white);
		else
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
			borderLevels-1, painter, i);
	}
}

void PullPlanEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(event->rect(), QColor(200, 200, 200));
	drawSubplan(10, 10, width() - 20, height() - 20, plan, plan->getShape(), 2, &painter);
	painter.end();
}



