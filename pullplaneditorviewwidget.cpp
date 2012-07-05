
#include "pullplaneditorviewwidget.h"

PullPlanEditorViewWidget :: PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setMinimumSize(200, 200);
	this->plan = plan;
	fill_rule = SINGLE_FILL_RULE;
	isDraggingCasing = false;
	casingHighlighted = false;
}


QRect PullPlanEditorViewWidget :: usedRect()
{
	return QRect(ulX, ulY, squareSize, squareSize);
}

void PullPlanEditorViewWidget :: resizeEvent(QResizeEvent* event)
{
	int width, height;

	width = event->size().width();
	height = event->size().height();

	if (width > height) // wider than tall 
	{
		ulX = (width - height)/2.0;
		ulY = 0;
		squareSize = height; 
	}
	else  
	{
		ulX = 0;
		ulY = (height - width)/2.0;
		squareSize = width; 
	}
}

int PullPlanEditorViewWidget :: getFillRule()
{
	return fill_rule;
}

void PullPlanEditorViewWidget :: setFillRule(int r)
{
	fill_rule = r;
}

float PullPlanEditorViewWidget :: adjustedX(float rawX)
{
	return rawX - ulX;
}

float PullPlanEditorViewWidget :: adjustedY(float rawY)
{
	return rawY - ulY;
}

float PullPlanEditorViewWidget :: rawX(float adjustedX)
{
	return adjustedX + ulX;
}

float PullPlanEditorViewWidget :: rawY(float adjustedY)
{
	return adjustedY + ulY;
}

float PullPlanEditorViewWidget :: getShapeRadius(int shape, float x, float y)
{
	switch (shape)
	{
		case CIRCLE_SHAPE:
			return sqrt(x * x + y * y); 
		case SQUARE_SHAPE:
			return MAX(fabs(x), fabs(y));
	}

	return -1;
}

bool PullPlanEditorViewWidget :: isOnCasing(int casingIndex, float x, float y)
{
	return fabs(plan->getCasingThickness(casingIndex) - getShapeRadius(plan->getCasingShape(casingIndex), x, y)) < 0.05; 
}

void PullPlanEditorViewWidget :: mousePressEvent(QMouseEvent* event)
{
	float x = (adjustedX(event->pos().x()) - squareSize/2) / float(squareSize/2-10);
	float y = (adjustedY(event->pos().y()) - squareSize/2) / float(squareSize/2-10);

	for (unsigned int i = 0; i < plan->getCasingCount() - 1; ++i) 
	{
		if (isOnCasing(i, x, y))
		{
			isDraggingCasing = true; 
			draggedCasingIndex = i;
			return;
		}	
	}
}

void PullPlanEditorViewWidget :: setMinMaxCasingRadii(float* min, float* max)
{
	/*
	Goal here is to deal with casings of different shapes, and set upper and 
	lower bounds for the radius of a particular casing based upon how much it
	can change before bumping into the inscribed (next smallest) or 
	circumscribed (next largest) casing. 

	It's really just adjusting by sqrt(2) in case, say, the casing is square
	and is surrounded by two circle casings.
	*/
	if (draggedCasingIndex == 0) {
		int cs0 = plan->getCasingShape(draggedCasingIndex);
		int cs1 = plan->getCasingShape(draggedCasingIndex+1);
		int ct1 = plan->getCasingThickness(draggedCasingIndex+1);
		*min = 0.01;
		if (cs0 == SQUARE_SHAPE && cs1 == CIRCLE_SHAPE)
			*max = ct1 / sqrt(2.0) - 0.05;
		else
			*max = ct1 - 0.05;
        }
	else {
		int csi = plan->getCasingShape(draggedCasingIndex);
		int csi_minus_1 = plan->getCasingShape(draggedCasingIndex-1);
		int csi_plus_1 = plan->getCasingShape(draggedCasingIndex+1);
		int cti_minus_1 = plan->getCasingThickness(draggedCasingIndex-1);
		int cti_plus_1 = plan->getCasingThickness(draggedCasingIndex+1);
		
		if (csi == CIRCLE_SHAPE && csi_minus_1 == SQUARE_SHAPE)
			*min = cti_minus_1 * sqrt(2.0) + 0.05;
		else
			*min = cti_minus_1 + 0.05;

		if (csi == SQUARE_SHAPE && csi_plus_1 == CIRCLE_SHAPE)
			*max = cti_plus_1 / sqrt(2.0) - 0.05;
		else
			*max = cti_plus_1 - 0.05;
        }
} 

void PullPlanEditorViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
	if (!isDraggingCasing)
		return;

	float x = (adjustedX(event->pos().x()) - squareSize/2);
	float y = (adjustedY(event->pos().y()) - squareSize/2);
	float radius = getShapeRadius(plan->getCasingShape(draggedCasingIndex), x, y) / (squareSize/2 - 10);

	float min;
	float max;

	setMinMaxCasingRadii(&min, &max);	
	plan->setCasingThickness(MIN(MAX(radius, min), max), draggedCasingIndex);

	emit someDataChanged();
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

	populateHighlightedSubplans(adjustedX(event->pos().x()), adjustedY(event->pos().y()), draggedPlan, type);
	if (subplansHighlighted.size() == 0) // anything highlighted must be casing from a color bar
	{
		populateHighlightedCasings(adjustedX(event->pos().x()), adjustedY(event->pos().y()), type);
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

	populateHighlightedSubplans(adjustedX(event->pos().x()), adjustedY(event->pos().y()), droppedPlan, type);
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
		populateHighlightedCasings(adjustedX(event->pos().x()), adjustedY(event->pos().y()), type);
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

	int drawSize = squareSize - 20;
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
	float drawSize = (squareSize - 20);
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
				if (MAX(fabs(x - squareSize/2.0), fabs(y - squareSize/2.0)) < drawSize/2 * plan->getCasingThickness(i))
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


void PullPlanEditorViewWidget :: setBoundaryPainter(QPainter* painter, bool outermostLevel) {

	if (outermostLevel)
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

void PullPlanEditorViewWidget :: paintShape(float x, float y, float size, int shape, QPainter* painter)
{
	switch (shape)
	{
		case CIRCLE_SHAPE:
			painter->drawEllipse(rawX(x), rawY(y), size, size);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(rawX(x), rawY(y), size, size);
			break;
	}
	
}


void PullPlanEditorViewWidget :: drawSubplan(float x, float y, float drawWidth, float drawHeight, 
	PullPlan* plan, bool highlightThis, int mandatedShape, bool outermostLevel, QPainter* painter) {

	// Fill the subplan area with some `cleared out' color
	painter->setBrush(QColor(200, 200, 200));
	painter->setPen(Qt::NoPen);
	paintShape(x, y, drawWidth, mandatedShape, painter);

	// Do casing colors outermost to innermost to get concentric rings of each casing's color
	// Skip outermost casing (that is done by your parent) and innermost (that is the `invisible'
	// casing for you to resize your subcanes)
	for (unsigned int i = plan->getCasingCount() - 1; plan->getCasingCount() > i && i > 0; --i) 
	{
		int casingWidth = drawWidth * plan->getCasingThickness(i);
		int casingHeight = drawHeight * plan->getCasingThickness(i);
		int casingX = x + drawWidth / 2 - casingWidth / 2;
		int casingY = y + drawHeight / 2 - casingHeight / 2;

		// Fill with solid neutral grey (in case fill is transparent)
		painter->setBrush(QColor(200, 200, 200));
		painter->setPen(Qt::NoPen); // Will draw boundary after all filling is done
		paintShape(casingX, casingY, casingWidth, plan->getCasingShape(i), painter);
		
		// Fill with actual casing color (highlighting white or some other color)
		if (outermostLevel && casingHighlighted && i == casingHighlightIndex)	
		{
			painter->setBrush(QColor(255*draggingColor.r, 255*draggingColor.g, 255*draggingColor.b, 
				255*draggingColor.a));
		}
		else
		{
			painter->setBrush(QColor(255*plan->getCasingColor(i)->r, 255*plan->getCasingColor(i)->g, 
				255*plan->getCasingColor(i)->b, 255*plan->getCasingColor(i)->a));
		}
		setBoundaryPainter(painter, outermostLevel);
		paintShape(casingX, casingY, casingWidth, plan->getCasingShape(i), painter);
	}

	// If you're supposed to become highlighted, do it. Note: this is not a casing highlight, that
	// was already (just) done when processing casing...this is a subcane being highlighted.
	if (highlightThis)
	{
		painter->setBrush(QColor(255*draggingColor.r, 255*draggingColor.g, 255*draggingColor.b, 
			255*draggingColor.a));
		painter->setPen(Qt::NoPen);
		paintShape(x, y, drawWidth, mandatedShape, painter);
		return;
	}

	// If you're a color bar, just fill region with color.
	if (plan->isBase())
	{
		Color* c = plan->getOutermostCasingColor();
		painter->setBrush(QColor(255*c->r, 255*c->g, 255*c->b, 255*c->a));
		painter->setPen(Qt::NoPen);
		paintShape(x, y, drawWidth, mandatedShape, painter);
		return;
	}

	// Recursively call drawing on subplans
	for (unsigned int i = plan->subs.size()-1; i < plan->subs.size(); --i)
	{
		SubpullTemplate* sub = &(plan->subs[i]);

		float rX = x + (sub->location.x - sub->diameter/2.0) * drawWidth/2 + drawWidth/2;
		float rY = y + (sub->location.y - sub->diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = sub->diameter * drawWidth/2;
		float rHeight = sub->diameter * drawHeight/2;

		if (outermostLevel) {
			bool highlighted = false;
			for (unsigned int j = 0; j < subplansHighlighted.size(); ++j) {
				if (subplansHighlighted[j] == i)
					highlighted = true;
			}
			if (highlighted) {
				drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
					true, plan->subs[i].shape, false, painter);
			}
			else {
				drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
					false, plan->subs[i].shape, false, painter);

			}
		}
		else {
			drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
				false, plan->subs[i].shape, 
				false, painter);
		}
		
		setBoundaryPainter(painter, outermostLevel);
		painter->setBrush(Qt::NoBrush);
		paintShape(rX, rY, rWidth, plan->subs[i].shape, painter);
	}
}

void PullPlanEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;

	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(event->rect(), QColor(200, 200, 200));
	drawSubplan(10, 10, squareSize - 20, squareSize - 20, plan, false, 
		plan->getOutermostCasingShape(), true, &painter);

	setBoundaryPainter(&painter, true);
	paintShape(10, 10, squareSize - 20, plan->getOutermostCasingShape(), &painter);

	painter.end();
}



