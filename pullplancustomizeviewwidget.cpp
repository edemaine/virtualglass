#include "pullplancustomizeviewwidget.h"
#include <QtDebug>

PullPlanCustomizeViewWidget::PullPlanCustomizeViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setMinimumSize(200, 200);
	setPullPlan(plan);
	Color* color = new Color;
	color->r = color->g = color->b = 1.0;
	color->a = 1.0;
	tempCirclePlan = new PullPlan(CUSTOM_CIRCLE_PULL_TEMPLATE);
	tempCirclePlan->setCasingColor(color, 0);
	tempSquarePlan = new PullPlan(CUSTOM_SQUARE_PULL_TEMPLATE);
	tempSquarePlan->setCasingColor(color, 0);
    mouseStartingLoc = new QPoint(INT_MAX,INT_MAX);
    subplansSelected.clear();
    boundActiveBox();
    this->setMouseTracking(true);
}

void PullPlanCustomizeViewWidget :: resizeEvent(QResizeEvent* event)
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

float PullPlanCustomizeViewWidget :: adjustedX(float rawX)
{
        return rawX - ulX;
}

float PullPlanCustomizeViewWidget :: adjustedY(float rawY)
{
        return rawY - ulY;
}

float PullPlanCustomizeViewWidget :: rawX(float adjustedX)
{
        return adjustedX + ulX;
}

float PullPlanCustomizeViewWidget :: rawY(float adjustedY)
{
        return adjustedY + ulY;
}

void PullPlanCustomizeViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PullPlanCustomizeViewWidget :: dropEvent(QDropEvent*)
{
	mouseStartingLoc->setX(INT_MAX);
    mouseStartingLoc->setY(INT_MAX);
//	event->setDropAction(Qt::CopyAction);
}

void PullPlanCustomizeViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
    int drawSize = squareSize - 20;
    if (adjustedX(event->pos().x()) < activeBox_xmin - 2*BOUNDING_BOX_SPACE ||
            adjustedY(event->pos().y()) < activeBox_ymin - 2*BOUNDING_BOX_SPACE ||
            adjustedX(event->pos().x()) > activeBox_xmax + 2*BOUNDING_BOX_SPACE ||
            adjustedY(event->pos().y()) > activeBox_ymax + 2*BOUNDING_BOX_SPACE )
    {
        activeBoxIndex = -1;
    }
    else if (activeBoxIndex == -1)
    {
        activeBoxIndex = INT_MAX;
    }
    if (activeBoxIndex != -1 && activeBoxIndex != INT_MAX)
    {
        SubpullTemplate* subpull = &(plan->subs[activeBoxIndex]);
        float dx = fabs(adjustedX(event->pos().x()) - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
        float dy = fabs(adjustedY(event->pos().y()) - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
        if (MAX(dx, dy) >= (subpull->diameter/2.0)*drawSize/2 + BOUNDING_BOX_SPACE)
        {
            activeBoxIndex = INT_MAX;
        }
    }
    if (activeBoxIndex == INT_MAX)
    {
        for (unsigned int i = 0; i < subplansSelected.size(); i++)
        {
            SubpullTemplate* subpull = &(plan->subs[subplansSelected[i]]);
            float dx = fabs(adjustedX(event->pos().x()) - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
            float dy = fabs(adjustedY(event->pos().y()) - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
            if (MAX(dx, dy) >= (subpull->diameter/2.0)*drawSize/2 + BOUNDING_BOX_SPACE)
            {
                activeBoxIndex = subplansSelected[i];
                break;
            }
        }
    }
    for (unsigned int i = 0; i < plan->subs.size(); ++i)
    {
        SubpullTemplate* subpull = &(plan->subs[i]);

        // Determine if drop hit the subplan
        //bool hit = false;
        float dx = fabs(adjustedX(event->pos().x()) - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
        float dy = fabs(adjustedY(event->pos().y()) - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
        switch (subpull->shape)
        {
            case CIRCLE_SHAPE:
                if (pow(double(dx*dx + dy*dy), 0.5) < (subpull->diameter/2.0)*drawSize/2)
                {
                    //hit = true;
                    //anyHit = true;
                    //if (hoveringIndex != (int)i)
                    //{
                    //    if (hoveringIndex != -1)
                    //    {
                    //        plan->subs[hoveringIndex].plan = hoveringPlan;
                    //    }
                        hoveringIndex = i;
                    //    hoveringPlan = subpull->plan;
                    //    plan->subs[i].plan = tempCirclePlan;
                    //}
                }
                break;
            case SQUARE_SHAPE:
                if (MAX(dx, dy) < (subpull->diameter/2.0)*drawSize/2)
                {
                    //hit = true;
                    //anyHit = true;
                    //if (hoveringIndex != (int)i)
                    //{
                    //    if (hoveringIndex != -1)
                    //    {
                    //        plan->subs[hoveringIndex].plan = hoveringPlan;
                    //    }
                        hoveringIndex = i;
                    //    hoveringPlan = subpull->plan;
                    //    plan->subs[i].plan = tempSquarePlan;
                    //}
                }
                break;
        }

        //if (!hit)
        //{
        //    continue;
        //}

        //event->accept();
        //this->update();
        //return;
    }

    //std::cout << hoveringIndex << std::endl;
    if (event->buttons() != Qt::NoButton)
	{
        // TODO: add in bounding box functionality here
		if (hoveringIndex == -1)
        {
            this->update();
            return;
        }
        /*if (mouseStartingLoc->x() == INT_MAX && mouseStartingLoc->y() == INT_MAX)
		{
            if (event->modifiers() != Qt::ControlModifier)
            {
                subplansSelected.clear();
            }
            bool isIn = false;
            for (unsigned int i = 0; i < subplansSelected.size(); i++)
            {
                if (hoveringIndex == int(subplansSelected.at(i)))
                {
                    isIn = true;
                    subplansSelected.erase(subplansSelected.begin()+i);
                    break;
                }
            }
            if (!isIn)
            {
                subplansSelected.push_back((unsigned int)hoveringIndex);
            }
			mouseStartingLoc->setX(event->pos().x());
			mouseStartingLoc->setY(event->pos().y());
        }*/
		if (isValidMovePosition(event))
		{
            for (unsigned int i = 0; i < subplansSelected.size(); i++)
            {
                plan->subs[subplansSelected[i]].location.x += (event->pos().x() - mouseStartingLoc->x())/(squareSize/2.0 - 10);
                plan->subs[subplansSelected[i]].location.y += (event->pos().y() - mouseStartingLoc->y())/(squareSize/2.0 - 10);
            }
//            plan->subs[hoveringIndex].location.x += (event->pos().x() - mouseStartingLoc->x())/(width()/2.0 - 10);
//            plan->subs[hoveringIndex].location.y += (event->pos().y() - mouseStartingLoc->y())/(width()/2.0 - 10);
            mouseStartingLoc->setX(event->pos().x());
            mouseStartingLoc->setY(event->pos().y());
        }
//		qDebug() << "location" << plan->subs[hoveringIndex].location.x << plan->subs[hoveringIndex].location.y;
//		qDebug() << "event" << event->pos().x() << event->pos().y();
//		qDebug() << "displacement" << event->pos().x()/(width()/2.0 - 10) - mouseStartingLoc->x()/(width()/2.0 - 10)
//				 << event->pos().y()/(width()/2.0 - 10) - mouseStartingLoc->y()/(width()/2.0 - 10);
		this->update();
		return;
	}
    /*if (event->modifiers() != Qt::ControlModifier)
    {
        subplansSelected.clear();
    }
	mouseStartingLoc->setX(INT_MAX);
	mouseStartingLoc->setY(INT_MAX);
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
                            std::cout << 'b' << std::endl;
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
                            std::cout << 'c' << std::endl;
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
    std::cout << 'd' << std::endl;
	plan->subs[hoveringIndex].plan = hoveringPlan;
	hoveringIndex = -1;
	this->update();
    return;*/
}

void PullPlanCustomizeViewWidget :: mousePressEvent(QMouseEvent* event)
{
    // TODO: add control point functionality here
    if (event->modifiers() != Qt::ControlModifier)
    {
        subplansSelected.clear();
    }
    bool isIn = false;
    for (unsigned int i = 0; i < subplansSelected.size(); i++)
    {
        if (hoveringIndex == int(subplansSelected.at(i)))
        {
            isIn = true;
            subplansSelected.erase(subplansSelected.begin()+i);
            break;
        }
    }
    if (!isIn)
    {
        subplansSelected.push_back((unsigned int)hoveringIndex);
    }
    mouseStartingLoc->setX(event->pos().x());
    mouseStartingLoc->setY(event->pos().y());
}

void PullPlanCustomizeViewWidget :: mouseReleaseEvent(QMouseEvent* event)
{
    mouseStartingLoc->setX(event->pos().x());
    mouseStartingLoc->setY(event->pos().y());
    emit pullPlanChanged(plan);
}

void PullPlanCustomizeViewWidget :: dragMoveEvent(QDragMoveEvent* event)
{
	if (hoveringIndex == -1)
		return;
	if (mouseStartingLoc->isNull())
	{
		mouseStartingLoc->setX(event->pos().x());
		mouseStartingLoc->setY(event->pos().y());
	}
    plan->subs[hoveringIndex].location.x += (event->pos().x() - mouseStartingLoc->x())/(squareSize/2.0 - 10);
    plan->subs[hoveringIndex].location.y += (event->pos().y() - mouseStartingLoc->y())/(squareSize/2.0 - 10);
    mouseStartingLoc->setX(event->pos().x());
    mouseStartingLoc->setY(event->pos().y());
    this->update();
}

void PullPlanCustomizeViewWidget :: setPullPlan(PullPlan* plan)
{
    this->plan = plan->copy();
    if (plan->getCasingShape(0) == SQUARE_SHAPE)
    {
        this->plan->setTemplateType(CUSTOM_SQUARE_PULL_TEMPLATE);
    }
    else
    {
        this->plan->setTemplateType(CUSTOM_CIRCLE_PULL_TEMPLATE);
    }
	this->plan->subs = plan->subs;
	hoveringIndex = -1;
    activeBoxIndex = -1;
    mouseStartingLoc = new QPoint(-1,-1);
}

bool PullPlanCustomizeViewWidget :: isValidMovePosition(QMouseEvent*)
{
	/* This should check if the subcane can be moved to the given position
	  (i.e. not intersecting with another subcane or leaving the casing).
	  For now it doesn't prevent anything. */
	return true;
}

void PullPlanCustomizeViewWidget :: boundActiveBox()
{
    activeBox_xmin = INT_MAX;
    activeBox_ymin = INT_MAX;
    activeBox_xmax = INT_MIN;
    activeBox_ymax = INT_MIN;
    int drawSize = squareSize - 20;
    for (unsigned int i = 0; i < subplansSelected.size(); i++)
    {
        SubpullTemplate* subpull = &(plan->subs[subplansSelected[i]]);
        float dx = drawSize/2 * subpull->location.x + drawSize/2 + 10;
        float dy = drawSize/2 * subpull->location.y + drawSize/2 + 10;
        if (dx - (subpull->diameter/2.0)*drawSize/2 < activeBox_xmin)
            activeBox_xmin = dx - (subpull->diameter/2.0)*drawSize/2;
        if (dy - (subpull->diameter/2.0)*drawSize/2 < activeBox_ymin)
            activeBox_ymin = dy - (subpull->diameter/2.0)*drawSize/2;
        if (dx + (subpull->diameter/2.0)*drawSize/2 > activeBox_xmax)
            activeBox_xmax = dx + (subpull->diameter/2.0)*drawSize/2;
        if (dy + (subpull->diameter/2.0)*drawSize/2 > activeBox_ymax)
            activeBox_ymax = dy + (subpull->diameter/2.0)*drawSize/2;
    }
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
			painter->drawEllipse(rawX(x), rawY(y), drawWidth, drawHeight);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(rawX(x), rawY(y), drawWidth, drawHeight);
			break;
	}


	// If it's a base color, fill region with color
	if (plan->isBase())
	{
		Color* c = plan->getCasingColor(0);
		painter->setBrush(QColor(255*c->r, 255*c->g, 255*c->b, 255*c->a));
		painter->setPen(Qt::NoPen);

		switch (mandatedShape)
		{
			case CIRCLE_SHAPE:
				painter->drawEllipse(rawX(x), rawY(y), drawWidth, drawHeight);
				break;
			case SQUARE_SHAPE:
				painter->drawRect(rawX(x), rawY(y), drawWidth, drawHeight);
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
	painter->setBrush(QColor(255*plan->getCasingColor(0)->r, 255*plan->getCasingColor(0)->g, 
		255*plan->getCasingColor(0)->b, 255*plan->getCasingColor(0)->a));
	switch (mandatedShape)
	{
		case CIRCLE_SHAPE:
			painter->drawEllipse(rawX(x), rawY(y), drawWidth, drawHeight);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(rawX(x), rawY(y), drawWidth, drawHeight);
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
	drawSubplan(10, 10, squareSize - 20, squareSize - 20, plan, plan->getCasingShape(0), 2, &painter);
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
