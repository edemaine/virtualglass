
#include "pullplaneditorviewwidget.h"

PullPlanEditorViewWidget :: PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setFixedSize(500, 500);
	this->plan = plan;
	fill_rule = SINGLE_FILL_RULE;
	isDraggingCasing = false;
	casingHighlighted = false;
}

int PullPlanEditorViewWidget :: getFillRule()
{
	return fill_rule;
}

void PullPlanEditorViewWidget :: setFillRule(int r)
{
	fill_rule = r;
}

void PullPlanEditorViewWidget :: mousePressEvent(QMouseEvent* event)
{
	float x = (event->pos().x() - width()/2) / float(width()/2-10);
	float y = (event->pos().y() - height()/2) / float(width()/2-10);
	float radius = sqrt(x * x + y * y); 

	for (unsigned int i = 0; i < plan->getCasingCount() - 1; ++i) {
		switch (plan->getCasingShape(i)) {
			case CIRCLE_SHAPE:
				if (fabs(radius - plan->getCasingThickness(i)) < 0.05) {
					isDraggingCasing = true; 
					draggedCasingIndex = i;
					return;
				}
				break;
			case SQUARE_SHAPE:
				if (fabs(radius - plan->getCasingThickness(i)) < 0.05) {
					isDraggingCasing = true; 
					draggedCasingIndex = i;
					return;
				}
				break;
		}
	}
}

void PullPlanEditorViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
	if (isDraggingCasing)
	{
		float x = (event->pos().x() - width()/2);
		float y = (event->pos().y() - height()/2);
		float radius = sqrt(x * x + y * y) / (width()/2 - 10); 

		float min;
		float max;

		if (draggedCasingIndex == 0) {
			min = 0.01;
			if (plan->getCasingShape(0) == SQUARE_SHAPE 
				&& plan->getCasingShape(1) == CIRCLE_SHAPE) 
				max = plan->getCasingThickness(1) / sqrt(2.0) - 0.05;
			else 
				max = plan->getCasingThickness(1) - 0.05;
		}
		else {
			if (plan->getCasingShape(draggedCasingIndex) == CIRCLE_SHAPE
				&& plan->getCasingShape(draggedCasingIndex-1) == SQUARE_SHAPE) 
				min = plan->getCasingThickness(draggedCasingIndex-1) * sqrt(2.0) + 0.05;
			else
				min = plan->getCasingThickness(draggedCasingIndex-1) + 0.05;

			if (plan->getCasingShape(draggedCasingIndex) == SQUARE_SHAPE
				&& plan->getCasingShape(draggedCasingIndex+1) == CIRCLE_SHAPE) 
				max = plan->getCasingThickness(draggedCasingIndex+1) / sqrt(2.0) - 0.05;
			else
				max = plan->getCasingThickness(draggedCasingIndex+1) - 0.05;
		}

		plan->setCasingThickness(MIN(MAX(radius, min), max), draggedCasingIndex);
		emit someDataChanged();
	}
}

void PullPlanEditorViewWidget :: mouseReleaseEvent(QMouseEvent* /*event*/)
{
	isDraggingCasing = false;
	emit someDataChanged();
}

void PullPlanEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PullPlanEditorViewWidget :: dragLeaveEvent(QDragLeaveEvent* /*event*/)
{
	subplansHighlighted.clear();
	casingHighlighted = false;
	emit someDataChanged();
}

void PullPlanEditorViewWidget :: dragMoveEvent(QDragMoveEvent* event)
{
	PullPlan* draggedPlan;
	int type;
	sscanf(event->mimeData()->text().toAscii().constData(), "%p %d", &draggedPlan, &type);

	subplansHighlighted.clear();
	casingHighlighted = false;

	populateHighlightedSubplans(event->pos().x(), event->pos().y(), draggedPlan, type);
	if (subplansHighlighted.size() == 0) // anything highlighted must be casing from a color bar
	{
		populateHighlightedCasings(event->pos().x(), event->pos().y(), type);
		draggingColor = *(draggedPlan->getOutermostCasingColor());
	}
	else
	{
		switch (type) {
			case COLOR_BAR_MIME:
				draggingColor = *(draggedPlan->getOutermostCasingColor());
				break;
			default:	
				draggingColor.r = draggingColor.g = draggingColor.b = draggingColor.a = 1.0;
				break;
		}
	}
	emit someDataChanged();
}

void PullPlanEditorViewWidget :: dropEvent(QDropEvent* event)
{
	PullPlan* droppedPlan;
	int type;
	sscanf(event->mimeData()->text().toAscii().constData(), "%p %d", &droppedPlan, &type);

	populateHighlightedSubplans(event->pos().x(), event->pos().y(), droppedPlan, type);
	if (subplansHighlighted.size() > 0)
	{
		event->accept();
		for (unsigned int i = 0; i < subplansHighlighted.size(); ++i)
		{
			plan->subs[subplansHighlighted[i]].plan = droppedPlan;
		}
	}
	else
	{
		populateHighlightedCasings(event->pos().x(), event->pos().y(), type);
		if (casingHighlighted)
		{
			event->accept();
			// at this point we have already checked that the dropped plan is a color bar
			// (that was done in populateHighlightedCasings()). So now we can just
			// take the color w/o thinking.
			plan->setCasingColor(droppedPlan->getOutermostCasingColor(), casingHighlightIndex);
		}
	}

	subplansHighlighted.clear();
	casingHighlighted = false;
	emit someDataChanged();

}


void PullPlanEditorViewWidget :: populateHighlightedSubplans(int x, int y, PullPlan* droppedPlan, int type)
{
	subplansHighlighted.clear();

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
		float dx = fabs(x - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
		float dy = fabs(y - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
		switch (subpull->shape)
		{
			case CIRCLE_SHAPE:
				if (pow(double(dx*dx + dy*dy), 0.5) < (subpull->diameter/2.0) * drawSize/2)
					hit = true;
				break;
			case SQUARE_SHAPE:
				if (MAX(dx, dy) < (subpull->diameter/2.0) * drawSize/2)
					hit = true;
				break;
		}

		if (!hit)
			continue;

		// If the dropped plan is a complex plan and its casing shape doesn't match the shape of the
		// subplan, reject
		if (type == PULL_PLAN_MIME)
		{
			if (subpull->shape != droppedPlan->getOutermostCasingShape())
			{
				continue;
			}
		}

		// If the shift button is down, fill in the entire group
		switch (fill_rule)
		{
			case SINGLE_FILL_RULE:
			{
				subplansHighlighted.push_back(i);	
				break;
			}
			case ALL_FILL_RULE:
			{
				for (unsigned int j = 0; j < plan->subs.size(); ++j)
					subplansHighlighted.push_back(j);
				break;
			}
			case GROUP_FILL_RULE:
			{
				int group = plan->subs[i].group;
				for (unsigned int j = i; j < plan->subs.size(); ++j)
				{
					if (plan->subs[j].group == group)
						subplansHighlighted.push_back(j);
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
							subplansHighlighted.push_back(j);
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
							subplansHighlighted.push_back(j);
						triarity = (triarity + 1) % 3;
					}
				}
				break;
			}
		}
	}
}


void PullPlanEditorViewWidget :: populateHighlightedCasings(int x, int y, int type)
{
	casingHighlighted = false;

	if (type != COLOR_BAR_MIME)
		return; 

	// Deal w/casing
	float drawSize = (width() - 20);
	float distanceFromCenter;
	for (unsigned int i = 1; i < plan->getCasingCount(); ++i) {
		switch (plan->getCasingShape(i)) {
			case CIRCLE_SHAPE:
				distanceFromCenter = sqrt(pow(x - (drawSize/2.0 + 10.0), 2.0)
					+ pow(y - (drawSize/2.0 + 10.0), 2.0));
				if (distanceFromCenter <= drawSize/2 * plan->getCasingThickness(i))
				{
					casingHighlighted = true;
					casingHighlightIndex = i;
					return;
				}
				break;
			case SQUARE_SHAPE:
				if (MAX(fabs(x - width()/2.0), fabs(y - height()/2.0)) < drawSize/2 * plan->getCasingThickness(i))
				{
					casingHighlighted = true;
					casingHighlightIndex = i;
					return;
				}
				break;
		}
	}
}

void PullPlanEditorViewWidget :: setPullPlan(PullPlan* plan) {

	this->plan = plan;
}


void PullPlanEditorViewWidget :: setBoundaryPainter(QPainter* painter, int drawWidth, 
	int drawHeight, int borderLevels) {

	painter->setBrush(Qt::NoBrush);
	if (MIN(drawWidth, drawHeight) < 10)
	{
                painter->setPen(Qt::NoPen);
	}
	else if (borderLevels > 0)
	{
		QPen pen;
		pen.setWidth(3);
		pen.setColor(Qt::black);
		painter->setPen(pen);
	}
	else
	{
		QPen pen;
		pen.setWidth(1);
		pen.setColor(Qt::black);
		painter->setPen(pen);
	}

}


void PullPlanEditorViewWidget :: drawSubplan(float x, float y, float drawWidth, float drawHeight, 
	PullPlan* plan, bool highlightThis, int mandatedShape, int borderLevels, QPainter* painter) {

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
		Color* c = plan->getOutermostCasingColor();
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

	// Draw casings
	setBoundaryPainter(painter, drawWidth, drawHeight, borderLevels);
	if (borderLevels == 1 && highlightThis)
		painter->setBrush(QColor(255*draggingColor.r, 255*draggingColor.g, 255*draggingColor.b, 
			255*draggingColor.a));
	else
		painter->setBrush(QColor(255*plan->getOutermostCasingColor()->r, 255*plan->getOutermostCasingColor()->g, 
			255*plan->getOutermostCasingColor()->b, 255*plan->getOutermostCasingColor()->a));
	switch (mandatedShape) {
		case CIRCLE_SHAPE:
			painter->drawEllipse(x, y, drawWidth, drawHeight);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(x, y, drawWidth, drawHeight);
			break;
	}

	if (borderLevels == 1 && highlightThis)
		return;

	for (unsigned int i = plan->getCasingCount() - 1; i > 0; --i) {

		int casingWidth = drawWidth * plan->getCasingThickness(i);
		int casingHeight = drawHeight * plan->getCasingThickness(i);
		int casingX = x + drawWidth / 2 - casingWidth / 2;
		int casingY = y + drawHeight / 2 - casingHeight / 2;
	
		setBoundaryPainter(painter, casingHeight, casingWidth, borderLevels);

		// set fill to be white if highlighted or background grey if not
		bool highlighted = false;
		if ((borderLevels == 2 && casingHighlighted && i == casingHighlightIndex) 
			|| (borderLevels == 1 && highlightThis))  
		{
			highlighted = true;
			painter->setBrush(QColor(255*draggingColor.r, 255*draggingColor.g, 255*draggingColor.b, 
				255*draggingColor.a));
		}
		else
			painter->setBrush(QColor(200, 200, 200));
		switch (plan->getCasingShape(i))
		{
			case CIRCLE_SHAPE:
				painter->drawEllipse(casingX, casingY, casingWidth, casingHeight);
				if (highlighted)
					break;
				painter->setBrush(QColor(255*plan->getCasingColor(i)->r, 255*plan->getCasingColor(i)->g, 
					255*plan->getCasingColor(i)->b, 255*plan->getCasingColor(i)->a));
				painter->drawEllipse(casingX, casingY, casingWidth, casingHeight);
				break;
			case SQUARE_SHAPE:
				painter->drawRect(casingX, casingY, casingWidth, casingHeight);
				if (highlighted)
					break;
				painter->setBrush(QColor(255*plan->getCasingColor(i)->r, 255*plan->getCasingColor(i)->g, 
					255*plan->getCasingColor(i)->b, 255*plan->getCasingColor(i)->a));
				painter->drawRect(casingX, casingY, casingWidth, casingHeight);
				break;
		}
	}

	for (unsigned int i = plan->subs.size()-1; i < plan->subs.size(); --i)
	{
		SubpullTemplate* sub = &(plan->subs[i]);

		float rX = x + (sub->location.x - sub->diameter/2.0) * drawWidth/2 + drawWidth/2;
		float rY = y + (sub->location.y - sub->diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = sub->diameter * drawWidth/2;
		float rHeight = sub->diameter * drawHeight/2;

		if (borderLevels == 2) {
			bool highlighted = false;
			for (unsigned int j = 0; j < subplansHighlighted.size(); ++j) {
				if (subplansHighlighted[j] == i)
					highlighted = true;
			}
			if (highlighted) {
				drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
					true, plan->subs[i].shape, 1, painter);
			}
			else {
				drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
					false, plan->subs[i].shape, 1, painter);

			}
		}
		else {
			drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
				false, plan->subs[i].shape, 
				borderLevels-1, painter);
		}
	}
}

void PullPlanEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(event->rect(), QColor(200, 200, 200));
	drawSubplan(10, 10, width() - 20, height() - 20, plan, casingHighlighted, 
		plan->getOutermostCasingShape(), 2, &painter);
	painter.end();
}



