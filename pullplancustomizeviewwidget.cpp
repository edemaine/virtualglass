
#include <QPainter>
#include "constants.h"
#include "glasscolor.h"
#include "pullplan.h"
#include "pulltemplate.h"
#include "subpulltemplate.h"
#include "pullplancustomizeviewwidget.h"
#include "globalbackgroundcolor.h"

PullPlanCustomizeViewWidget::PullPlanCustomizeViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setMinimumSize(200, 200);
	setPullPlan(plan);
	mouseStartingLoc.x = FLT_MAX;
	mouseStartingLoc.y = FLT_MAX;
	clickedLoc = new QPoint(INT_MAX, INT_MAX);
	clickMoved = false;
	subplansSelected.clear();
	hoveringIndex = -1;
	activeBoxIndex = -1;
	activeControlPoint = -1;
	mode = MOVE_MODE;
	BOUNDING_BOX_SPACE = 1; //squareSize isn't set yet, so can't do MAX(squareSize / 100, 1);
	boundActiveBox();
	this->setMouseTracking(true);
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(this, SIGNAL(someDataChanged()), this, SLOT(setCustomTemplate()));
}

void PullPlanCustomizeViewWidget :: setCustomTemplate()
{
	plan->setTemplateType(PullTemplate::CUSTOM);	
}

void PullPlanCustomizeViewWidget :: resizeEvent(QResizeEvent* event)
{
	int width, height;

	width = event->size().width();
	height = event->size().height();

	if (width > height) // wider than tall 
	{
		drawUpperLeft.x = (width - height)/2.0;
		drawUpperLeft.y = 0;
		squareSize = height;
	}
	else
	{
		drawUpperLeft.x = 0;
		drawUpperLeft.y = (height - width)/2.0;
		squareSize = width;
	}

	BOUNDING_BOX_SPACE = MAX(squareSize / 100, 1);
}

float PullPlanCustomizeViewWidget :: adjustedX(float rawX)
{
	return rawX - drawUpperLeft.x;
}

float PullPlanCustomizeViewWidget :: adjustedY(float rawY)
{
	return rawY - drawUpperLeft.y;
}

float PullPlanCustomizeViewWidget :: rawX(float adjustedX)
{
	return adjustedX + drawUpperLeft.x;
}

float PullPlanCustomizeViewWidget :: rawY(float adjustedY)
{
	return adjustedY + drawUpperLeft.y;
}

void PullPlanCustomizeViewWidget :: dropEvent(QDropEvent* event)
{
	mouseStartingLoc.x= FLT_MAX;
	mouseStartingLoc.y= FLT_MAX;
	event->setDropAction(Qt::CopyAction);
}

void PullPlanCustomizeViewWidget :: updateIndexes(QPoint pos)
{
	int drawSize = squareSize - 20;
	if (pow(double((adjustedX(pos.x()) - clickedLoc->x())*(adjustedX(pos.x()) - clickedLoc->x()) 
		+ (adjustedY(pos.y()) - clickedLoc->y())*(adjustedY(pos.y()) - clickedLoc->y())),0.5) > BOUNDING_BOX_SPACE)
	{
		clickMoved = true;
	}

	if (adjustedX(pos.x()) < activeBox_xmin - 6*BOUNDING_BOX_SPACE ||
		adjustedY(pos.y()) < activeBox_ymin - 6*BOUNDING_BOX_SPACE ||
		adjustedX(pos.x()) > activeBox_xmax + 6*BOUNDING_BOX_SPACE ||
		adjustedY(pos.y()) > activeBox_ymax + 6*BOUNDING_BOX_SPACE )
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
		float dx = fabs(adjustedX(pos.x()) - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
		float dy = fabs(adjustedY(pos.y()) - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
		if (MAX(dx, dy) >= (subpull->diameter/2.0)*drawSize/2 + 3*BOUNDING_BOX_SPACE)
		{
			activeBoxIndex = INT_MAX;
		}
	}

	if (activeBoxIndex == INT_MAX)
	{
		for (unsigned int i = 0; i < subplansSelected.size(); i++)
		{
			SubpullTemplate* subpull = &(plan->subs[subplansSelected[i]]);
			float dx = fabs(adjustedX(pos.x()) - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
			float dy = fabs(adjustedY(pos.y()) - (drawSize/2 * subpull->location.y + drawSize/2 + 10));

			switch (subpull->shape)
			{
				case CIRCLE_SHAPE:
					if (pow(double(dx*dx + dy*dy), 0.5) < (subpull->diameter/2.0)*drawSize/2)
					{
						activeBoxIndex = subplansSelected[i];
					}
					break;
				case SQUARE_SHAPE:
					if (MAX(dx, dy) < (subpull->diameter/2.0)*drawSize/2)
					{
						activeBoxIndex = subplansSelected[i];
					}
					break;
			}
		}
	}

	if (activeBoxIndex == INT_MAX)
	{
		for (unsigned int i = 0; i < subplansSelected.size(); i++)
		{
			SubpullTemplate* subpull = &(plan->subs[subplansSelected[i]]);
			float dx = fabs(adjustedX(pos.x()) - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
			float dy = fabs(adjustedY(pos.y()) - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
			if (MAX(dx, dy) < (subpull->diameter/2.0)*drawSize/2 + BOUNDING_BOX_SPACE)
			{
				activeBoxIndex = subplansSelected[i];
				break;
			}
		}
	}

	hoveringIndex = -1;
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->subs[i]);
		float dx = fabs(adjustedX(pos.x()) - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
		float dy = fabs(adjustedY(pos.y()) - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
		switch (subpull->shape)
		{
			case CIRCLE_SHAPE:
				if (pow(double(dx*dx + dy*dy), 0.5) < (subpull->diameter/2.0)*drawSize/2)
				{
					hoveringIndex = i;
				}
				break;
			case SQUARE_SHAPE:
				if (MAX(dx, dy) < (subpull->diameter/2.0)*drawSize/2)
				{
					hoveringIndex = i;
				}
				break;
		}
		if (hoveringIndex != -1)
		{
			break;
		}
	}

	update();
}

void PullPlanCustomizeViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
	int drawSize = squareSize - 20;
	if (event->buttons() == Qt::NoButton)
	{
		updateIndexes(event->pos());
		return;
	}
	
	switch (mode)
	{
		case MOVE_MODE:
		{
			if (pow(double((adjustedX(event->pos().x()) - clickedLoc->x()) 
				* (adjustedX(event->pos().x()) - clickedLoc->x()) 
				+ (adjustedY(event->pos().y()) - clickedLoc->y())
				* (adjustedY(event->pos().y()) - clickedLoc->y())),0.5) > BOUNDING_BOX_SPACE)
			{
				clickMoved = true;
			}

			if (isValidMovePosition(event))
			{
				for (unsigned int i = 0; i < subplansSelected.size(); i++)
				{
					plan->subs[subplansSelected[i]].location.x += 
						(adjustedX(event->pos().x()) - mouseStartingLoc.x)/(squareSize/2.0 - 10);
					plan->subs[subplansSelected[i]].location.y += 
						(adjustedY(event->pos().y()) - mouseStartingLoc.y)/(squareSize/2.0 - 10);
					emit someDataChanged();
				}
				mouseStartingLoc.x=(adjustedX(event->pos().x()));
				mouseStartingLoc.y=(adjustedY(event->pos().y()));
			}
			break;
		}
		case SCALE_MODE:
		{
			clickMoved = true;
			if (activeBoxIndex == -1)
			{
				update();
				return;
			}

			if (activeBoxIndex != INT_MAX)
			{
				SubpullTemplate* subpull = &(plan->subs[activeBoxIndex]);
				float dx = fabs(adjustedX(event->pos().x()) 
					- (drawSize/2 * subpull->location.x + drawSize/2 + 10));
				float dy = fabs(adjustedY(event->pos().y()) 
					- (drawSize/2 * subpull->location.y + drawSize/2 + 10));
				float new_diameter = MAX(dx,dy);
				new_diameter -= BOUNDING_BOX_SPACE;
				new_diameter /= float(drawSize/2);
				new_diameter *= 2.0;
				float proportion = new_diameter / plan->subs[activeBoxIndex].diameter;
				for (unsigned int i = 0; i < subplansSelected.size(); i++)
				{
					plan->subs[subplansSelected[i]].diameter *= proportion;
				}
				emit someDataChanged();
			}
			else
			{
				float center_x = activeBox_xmin/2.0 + activeBox_xmax/2.0;
				float center_y = activeBox_ymin/2.0 + activeBox_ymax/2.0;
				float dx = fabs(adjustedX(event->pos().x()) - center_x)-3*BOUNDING_BOX_SPACE;
				float dy = fabs(adjustedY(event->pos().y()) - center_y)-3*BOUNDING_BOX_SPACE;
				float yx_proportion = (activeBox_ymax-activeBox_ymin)/(activeBox_xmax-activeBox_xmin);
				// no = new/old
				float no_proportion = 1.0;

				if (dy > dx*yx_proportion)
				{
					no_proportion = dy/(activeBox_ymax/2.0 - activeBox_ymin/2.0);
				}
				else
				{
					no_proportion = dx/(activeBox_xmax/2.0 - activeBox_xmin/2.0);
				}

				for (unsigned int i = 0; i < subplansSelected.size(); i++)
				{
					plan->subs[subplansSelected[i]].diameter *= no_proportion;
					plan->subs[subplansSelected[i]].location.x = (center_x - 10 - drawSize/2) / double(drawSize/2.0) 
						+ ((plan->subs[subplansSelected[i]].location.x 
							- (center_x - 10 - drawSize/2) / double(drawSize/2.0)) * no_proportion);
					plan->subs[subplansSelected[i]].location.y = (center_y - 10 - drawSize/2) / double(drawSize/2.0) 
						+ ((plan->subs[subplansSelected[i]].location.y 
							- (center_y - 10 - drawSize/2)/double(drawSize/2.0))*no_proportion);
				}
				emit someDataChanged();
			}
			break;
		}
	}
	boundActiveBox();
	update();

	// This is to turn a control point a different color when it's being hovered over. For now it does nothing.
	activeControlPoint = -1;
	if (activeBoxIndex != -1 && activeBoxIndex != INT_MAX)
	{
		SubpullTemplate* subpull = &(plan->subs[activeBoxIndex]);
		float dx = fabs(adjustedX(event->pos().x()) - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
		float dy = fabs(adjustedY(event->pos().y()) - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
		if (fabs((subpull->diameter/2.0)*drawSize/2 - dx) < BOUNDING_BOX_SPACE &&
		    fabs((subpull->diameter/2.0)*drawSize/2 - dy) < BOUNDING_BOX_SPACE)
		{
			// not yet implemented
		}
	}
	else if (activeBoxIndex == INT_MAX)
	{
		if((fabs(float(adjustedX(event->pos().x()) - activeBox_xmin - 3*BOUNDING_BOX_SPACE)) < BOUNDING_BOX_SPACE ||
			fabs(float(adjustedX(event->pos().x()) - activeBox_xmax - 3*BOUNDING_BOX_SPACE)) < BOUNDING_BOX_SPACE) &&
			(fabs(float(adjustedY(event->pos().y()) - activeBox_ymin + 3*BOUNDING_BOX_SPACE)) < BOUNDING_BOX_SPACE ||
			fabs(float(adjustedY(event->pos().y()) - activeBox_ymax + 3*BOUNDING_BOX_SPACE)) < BOUNDING_BOX_SPACE))
		{
			// not yet implemented
		}
	}
}

void PullPlanCustomizeViewWidget :: mousePressEvent(QMouseEvent* event)
{
	int drawSize = squareSize - 20;
	mode = MOVE_MODE;
	clickedLoc->setX(adjustedX(event->pos().x()));
	clickedLoc->setY(adjustedY(event->pos().y()));
	clickMoved = false;
	if (activeBoxIndex != -1 && activeBoxIndex != INT_MAX)
	{
		SubpullTemplate* subpull = &(plan->subs[activeBoxIndex]);
		float dx = fabs(adjustedX(event->pos().x()) - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
		float dy = fabs(adjustedY(event->pos().y()) - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
		if (fabs((subpull->diameter/2.0)*drawSize/2+BOUNDING_BOX_SPACE - dx) < 2*BOUNDING_BOX_SPACE &&
			fabs((subpull->diameter/2.0)*drawSize/2+BOUNDING_BOX_SPACE - dy) < 2*BOUNDING_BOX_SPACE)
		{
			mode = SCALE_MODE;
		}
	}
	else if (activeBoxIndex == INT_MAX)
	{
		if((fabs(float(adjustedX(event->pos().x()) - activeBox_xmin + 3*BOUNDING_BOX_SPACE)) < 2*BOUNDING_BOX_SPACE ||
			fabs(float(adjustedX(event->pos().x()) - activeBox_xmax - 3*BOUNDING_BOX_SPACE)) < 2*BOUNDING_BOX_SPACE) &&
			(fabs(float(adjustedY(event->pos().y()) - activeBox_ymin + 3*BOUNDING_BOX_SPACE)) < 2*BOUNDING_BOX_SPACE ||
			fabs(float(adjustedY(event->pos().y()) - activeBox_ymax - 3*BOUNDING_BOX_SPACE)) < 2*BOUNDING_BOX_SPACE))
		{
			mode = SCALE_MODE;
		}
	}

	if (mode == MOVE_MODE)
	{
		bool isIn = false;
		for (unsigned int i = 0; i < subplansSelected.size(); i++)
		{
			if (hoveringIndex == int(subplansSelected.at(i)))
			{
				isIn = true;
				break;
			}
		}
		if (!isIn && hoveringIndex != -1)
		{
			if (event->modifiers() != Qt::ControlModifier && event->modifiers() != Qt::ShiftModifier)
			{
				subplansSelected.clear();
			}
			else
			{
				clickMoved = true;
			}
			subplansSelected.push_back((unsigned int)hoveringIndex);
			activeBoxIndex = hoveringIndex;
		}
		if (activeBoxIndex == -1)
		{
			subplansSelected.clear();
		}
	}

	mouseStartingLoc.x=(adjustedX(event->pos().x()));
	mouseStartingLoc.y=(adjustedY(event->pos().y()));
	boundActiveBox();
	updateIndexes(event->pos());
	update();
}

void PullPlanCustomizeViewWidget :: mouseReleaseEvent(QMouseEvent* event)
{
	if (!clickMoved)
	{
		if (event->modifiers() != Qt::ControlModifier && event->modifiers() != Qt::ShiftModifier)
		{
			subplansSelected.clear();
			if (hoveringIndex != -1)
			{
				subplansSelected.push_back((unsigned int)hoveringIndex);
				activeBoxIndex = hoveringIndex;
			}
		}
		else
		{
			for (unsigned int i = 0; i < subplansSelected.size(); i++)
			{
				if (hoveringIndex == int(subplansSelected.at(i)))
				{
					subplansSelected.erase(subplansSelected.begin()+i);
					break;
				}
			}
		}
	}
	
	mouseStartingLoc.x = (adjustedX(event->pos().x()));
	mouseStartingLoc.y = (adjustedY(event->pos().y()));
	emit someDataChanged();
	boundActiveBox();
	updateIndexes(event->pos());
	update();
}

void PullPlanCustomizeViewWidget :: dragMoveEvent(QDragMoveEvent* event)
{
	if (hoveringIndex == -1)
		return;
	if (mouseStartingLoc.x == FLT_MAX && mouseStartingLoc.y == FLT_MAX)
	{
		mouseStartingLoc.x=(adjustedX(event->pos().x()));
		mouseStartingLoc.y=(adjustedY(event->pos().y()));
	}
	plan->subs[hoveringIndex].location.x += (adjustedX(event->pos().x()) - mouseStartingLoc.x)/(squareSize/2.0 - 10);
	plan->subs[hoveringIndex].location.y += (adjustedY(event->pos().y()) - mouseStartingLoc.y)/(squareSize/2.0 - 10);
	mouseStartingLoc.x=(adjustedX(event->pos().x()));
	mouseStartingLoc.y=(adjustedY(event->pos().y()));
	boundActiveBox();
	emit someDataChanged();
	update();
}

void PullPlanCustomizeViewWidget :: keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
	{
		deleteSelectionPressed();
	}
}

void PullPlanCustomizeViewWidget :: setPullPlan(PullPlan* _plan)
{
	plan = _plan;

	// reset gui stuffs
	subplansSelected.clear();
	activeControlPoint = -1;
	hoveringIndex = -1;
	activeBoxIndex = -1;
	updateEverything();

	for (unsigned int i = 0; i < subplansSelected.size(); i++)
	{
		if (subplansSelected[i] >= plan->subs.size())
		{
			subplansSelected.clear();
			activeControlPoint = -1;
			hoveringIndex = -1;
			activeBoxIndex = -1;
			break;
		}
	}
	mouseStartingLoc.x = FLT_MAX;
	mouseStartingLoc.y = FLT_MAX;
	boundActiveBox();
}

void PullPlanCustomizeViewWidget :: updateEverything()
{
	this->repaint();
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
	activeBox_xmin = activeBox_ymin = INT_MAX;
	activeBox_xmax = activeBox_ymax = INT_MIN;
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

void PullPlanCustomizeViewWidget :: setBoundaryPainter(QPainter* painter, bool outermostLevel, bool greyedOut = false) 
{

	if (outermostLevel)
	{
		QPen pen;
		pen.setWidth(3);
		if (greyedOut)
		    pen.setColor(Qt::gray);
		else
		    pen.setColor(Qt::black);
		painter->setPen(pen);
	}
	else
		painter->setPen(Qt::NoPen);

}

void PullPlanCustomizeViewWidget :: paintShape(Point2D upperLeft, float size, enum GeometricShape shape, QPainter* painter)
{
	int roundedX, roundedY;
	
	roundedX = rawX(upperLeft.x) + 0.5;
	roundedY = rawY(upperLeft.y) + 0.5;

	switch (shape)
	{
		case CIRCLE_SHAPE:
		    painter->drawEllipse(roundedX, roundedY, size, size);
		    break;
		case SQUARE_SHAPE:
		    painter->drawRect(roundedX, roundedY, size, size);
		    break;
	}

}
void PullPlanCustomizeViewWidget :: drawSubplan(Point2D upperLeft, float drawWidth, float drawHeight,
	PullPlan* plan, bool outermostLevel, QPainter* painter)
{
	// Fill the subplan area with some `cleared out' color
	painter->setBrush(GlobalBackgroundColor::qcolor);
	painter->setPen(Qt::NoPen);
	paintShape(upperLeft, drawWidth, plan->getOutermostCasingShape(), painter);

	// Do casing colors outermost to innermost to get concentric rings of each casing's color
	// Skip outermost casing (that is done by your parent) and innermost (that is the `invisible'
	// casing for you to resize your subcanes)
	for (unsigned int i = plan->getCasingCount() - 1; i < plan->getCasingCount(); --i)
	{
		float casingWidth = drawWidth * plan->getCasingThickness(i);
		float casingHeight = drawHeight * plan->getCasingThickness(i);
		Point2D casingUpperLeft;
		casingUpperLeft.x = upperLeft.x + drawWidth / 2 - casingWidth / 2;
		casingUpperLeft.y = upperLeft.y + drawHeight / 2 - casingHeight / 2;

		// Fill with solid neutral grey (in case fill is transparent)
		painter->setBrush(GlobalBackgroundColor::qcolor);
		painter->setPen(Qt::NoPen); // Will draw boundary after all filling is done
		paintShape(casingUpperLeft, casingWidth, plan->getCasingShape(i), painter);

		Color casingColor = plan->getCasingColor(i)->getColor();
		QColor qc(255*casingColor.r, 255*casingColor.g, 255*casingColor.b, 255*casingColor.a);
		painter->setBrush(qc);
		setBoundaryPainter(painter, outermostLevel, outermostLevel);
		paintShape(casingUpperLeft, casingWidth, plan->getCasingShape(i), painter);
	}

	// Recursively call drawing on subplans
	for (unsigned int i = plan->subs.size()-1; i < plan->subs.size(); --i)
	{
		SubpullTemplate* sub = &(plan->subs[i]);

		Point2D subUpperLeft;
		subUpperLeft.x = upperLeft.x + (sub->location.x - sub->diameter/2.0) * drawWidth/2 + drawWidth/2;
		subUpperLeft.y = upperLeft.y + (sub->location.y - sub->diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = sub->diameter * drawWidth/2;
		float rHeight = sub->diameter * drawHeight/2;

		drawSubplan(subUpperLeft, rWidth, rHeight, plan->subs[i].plan, false, painter);

		setBoundaryPainter(painter, outermostLevel);
		painter->setBrush(Qt::NoBrush);
		paintShape(subUpperLeft, rWidth, plan->subs[i].shape, painter);
	}
}

void PullPlanCustomizeViewWidget :: drawActionControls(QPainter* painter)
{
	int drawSize = squareSize - 20;

	// Outline for individual selected plans
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::black);
	pen.setStyle(Qt::DotLine);
	painter->setBrush(Qt::NoBrush); 
	painter->setPen(pen);
	for (unsigned int i = 0; i < subplansSelected.size(); i++)
	{
		SubpullTemplate* subpull = &(plan->subs[subplansSelected[i]]);
		painter->drawRect(rawX((subpull->location.x - subpull->diameter/2.0)*drawSize/2 + drawSize/2 + 10) - BOUNDING_BOX_SPACE, 
			rawY((subpull->location.y - subpull->diameter/2.0)*drawSize/2+drawSize/2+10)-BOUNDING_BOX_SPACE,
			subpull->diameter*drawSize/2.0 + 2*BOUNDING_BOX_SPACE,
			subpull->diameter*drawSize/2.0 + 2*BOUNDING_BOX_SPACE);
	}

	// Outline for a group selection
	if (subplansSelected.size() > 1)
	{
		painter->drawRect(rawX(activeBox_xmin)-3*BOUNDING_BOX_SPACE,
			rawY(activeBox_ymin)-3*BOUNDING_BOX_SPACE,
			(activeBox_xmax-activeBox_xmin)+6*BOUNDING_BOX_SPACE,
			(activeBox_ymax-activeBox_ymin)+6*BOUNDING_BOX_SPACE);
	}

	// White control points for resizing
	painter->setBrush(QColor(0, 0, 0, 255));
	pen.setWidth(2);
	pen.setColor(Qt::white);
	pen.setStyle(Qt::SolidLine);
	painter->setPen(pen);
	if (activeBoxIndex != -1 && activeBoxIndex != INT_MAX)
	{
		SubpullTemplate* subpull = &(plan->subs[activeBoxIndex]);
		painter->drawEllipse(rawX((subpull->location.x - subpull->diameter/2.0)*drawSize/2+drawSize/2+10)-BOUNDING_BOX_SPACE*2,
			rawY((subpull->location.y - subpull->diameter/2.0)*drawSize/2+drawSize/2+10)-BOUNDING_BOX_SPACE*2,
			BOUNDING_BOX_SPACE*2,
			BOUNDING_BOX_SPACE*2);
		painter->drawEllipse(rawX((subpull->location.x - subpull->diameter/2.0)*drawSize/2+drawSize/2+10)-BOUNDING_BOX_SPACE*2,
			rawY((subpull->location.y + subpull->diameter/2.0)*drawSize/2+drawSize/2+10)+BOUNDING_BOX_SPACE*0,
			BOUNDING_BOX_SPACE*2,
			BOUNDING_BOX_SPACE*2);
		painter->drawEllipse(rawX((subpull->location.x + subpull->diameter/2.0)*drawSize/2+drawSize/2+10)+BOUNDING_BOX_SPACE*0,
			rawY((subpull->location.y - subpull->diameter/2.0)*drawSize/2+drawSize/2+10)-BOUNDING_BOX_SPACE*2,
			BOUNDING_BOX_SPACE*2,
			BOUNDING_BOX_SPACE*2);
		painter->drawEllipse(rawX((subpull->location.x + subpull->diameter/2.0)*drawSize/2+drawSize/2+10)+BOUNDING_BOX_SPACE*0,
			rawY((subpull->location.y + subpull->diameter/2.0)*drawSize/2+drawSize/2+10)+BOUNDING_BOX_SPACE*0,
			BOUNDING_BOX_SPACE*2,
			BOUNDING_BOX_SPACE*2);
	}
	if (subplansSelected.size() > 1 && activeBoxIndex == INT_MAX)
	{
		painter->drawEllipse(rawX(activeBox_xmin)-4*BOUNDING_BOX_SPACE,
			rawY(activeBox_ymin)-4*BOUNDING_BOX_SPACE,
			BOUNDING_BOX_SPACE*2,
			BOUNDING_BOX_SPACE*2);
		painter->drawEllipse(rawX(activeBox_xmin)-4*BOUNDING_BOX_SPACE,
			rawY(activeBox_ymax)+2*BOUNDING_BOX_SPACE,
			BOUNDING_BOX_SPACE*2,
			BOUNDING_BOX_SPACE*2);
		painter->drawEllipse(rawX(activeBox_xmax)+2*BOUNDING_BOX_SPACE,
			rawY(activeBox_ymin)-4*BOUNDING_BOX_SPACE,
			BOUNDING_BOX_SPACE*2,
			BOUNDING_BOX_SPACE*2);
		painter->drawEllipse(rawX(activeBox_xmax)+2*BOUNDING_BOX_SPACE,
			rawY(activeBox_ymax)+2*BOUNDING_BOX_SPACE,
			BOUNDING_BOX_SPACE*2,
			BOUNDING_BOX_SPACE*2);
	}
}

void PullPlanCustomizeViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;

	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(event->rect(), GlobalBackgroundColor::qcolor);
	Point2D drawUpperLeft;
	drawUpperLeft.x = 10.0;
	drawUpperLeft.y = 10.0;

	painter.setBrush(Qt::NoBrush);
	setBoundaryPainter(&painter, true, true);
	paintShape(drawUpperLeft, squareSize - 20, plan->getOutermostCasingShape(), &painter);

	drawSubplan(drawUpperLeft, squareSize - 20, squareSize - 20, plan, true, &painter);

	drawActionControls(&painter);

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
}

void PullPlanCustomizeViewWidget :: copySelectionPressed()
{
	if (subplansSelected.size() == 0)
		return;
	vector<SubpullTemplate*> newSubplans;
	newSubplans.clear();
	for (unsigned int i = 0; i < subplansSelected.size(); i++)
	{
		newSubplans.push_back(new SubpullTemplate(
			plan->subs[subplansSelected[i]].plan,
			plan->subs[subplansSelected[i]].shape,
			plan->subs[subplansSelected[i]].location,
			plan->subs[subplansSelected[i]].diameter));
		newSubplans.at(i)->location.x += 3*BOUNDING_BOX_SPACE/squareSize;
		newSubplans.at(i)->location.y += 3*BOUNDING_BOX_SPACE/squareSize;
	}
	for (unsigned int i = 0; i < subplansSelected.size(); i++)
	{
		plan->subs.insert(plan->subs.begin()+i,*newSubplans.at(i));
	}
	subplansSelected.clear();
	for (unsigned int i = 0; i < newSubplans.size(); i++)
	{
		subplansSelected.push_back(i);
	}
	emit someDataChanged();
	boundActiveBox();
	update();
}

void PullPlanCustomizeViewWidget :: deleteSelectionPressed()
{
	for (unsigned int i = 0; i < subplansSelected.size(); i++)
	{
		unsigned int n = subplansSelected[i];
		plan->subs.erase(plan->subs.begin()+n);
		for (unsigned int j = i+1; j < subplansSelected.size(); j++)
		{
			if (subplansSelected[j] > n)
				subplansSelected[j] -= 1;
		}
	}
	subplansSelected.clear();
	hoveringIndex = -1;
	activeBoxIndex = -1;
	activeControlPoint = -1;
	emit someDataChanged();
	boundActiveBox();
	update();
}

void PullPlanCustomizeViewWidget :: addCirclePressed()
{
	Point2D p = make_vector(0.0f, 0.0f);
	float diameter = 0;
	for (unsigned int i = 0; i < plan->subs.size(); i++)
	{
		diameter += plan->subs[i].diameter/plan->subs.size();
	}
	if (diameter == 0)
	{
		diameter = plan->getCasingThickness(0);
	}
	plan->subs.insert(plan->subs.begin(), 
		SubpullTemplate(new PullPlan(PullTemplate::BASE_CIRCLE), CIRCLE_SHAPE, p, diameter));
	subplansSelected.clear();
	subplansSelected.push_back(0);
	emit someDataChanged();
	boundActiveBox();
	update();
}

void PullPlanCustomizeViewWidget :: addSquarePressed()
{
	Point2D p = make_vector(0.0f, 0.0f);
	float diameter = 0;
	for (unsigned int i = 0; i < plan->subs.size(); i++)
	{
		diameter += plan->subs[i].diameter/plan->subs.size();
	}
	if (diameter == 0)
	{
		diameter = plan->getCasingThickness(0);
	}
	plan->subs.insert(plan->subs.begin(),
		SubpullTemplate(new PullPlan(PullTemplate::BASE_SQUARE), SQUARE_SHAPE, p, diameter));
	subplansSelected.clear();
	subplansSelected.push_back(0);
	emit someDataChanged();
	boundActiveBox();
	update();
}




