
#include <QPainter>

#include "globalglass.h"
#include "constants.h"
#include "glasscolor.h"
#include "cane.h"
#include "canetemplate.h"
#include "subcanetemplate.h"
#include "canecustomizeviewwidget.h"
#include "globalbackgroundcolor.h"
#include "globalundoredo.h"

CaneCustomizeViewWidget::CaneCustomizeViewWidget(Cane* cane, QWidget* parent) : QWidget(parent)
{
	// Drawing 
	setMinimumSize(200, 200);
	this->cane = cane;
	
	// Mouse tracking 
	mouseStartingLoc.x = FLT_MAX;
	mouseStartingLoc.y = FLT_MAX;
	clickedLoc.x = FLT_MAX;
	clickedLoc.y = FLT_MAX;
	clickMoved = false;
	hoveringIndex = -1;
	activeBoxIndex = -1;
	activeControlPoint = -1;
	mode = MOVE_MODE;
	boundingBoxSpace = 1; //squareSize isn't set yet, so can't do MAX(squareSize / 100, 1);
	boundActiveBox();
	this->setMouseTracking(true);

	// Keyboard tracking
	this->setFocusPolicy(Qt::StrongFocus);

	// Listen to your cane
	connect(this->cane, SIGNAL(modified()), this, SLOT(updateEverything()));
}

void CaneCustomizeViewWidget :: resizeEvent(QResizeEvent* event)
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

	boundingBoxSpace = MAX(squareSize / 100, 1);
}

float CaneCustomizeViewWidget :: adjustedX(float rawX)
{
	return rawX - drawUpperLeft.x;
}

float CaneCustomizeViewWidget :: adjustedY(float rawY)
{
	return rawY - drawUpperLeft.y;
}

float CaneCustomizeViewWidget :: rawX(float adjustedX)
{
	return adjustedX + drawUpperLeft.x;
}

float CaneCustomizeViewWidget :: rawY(float adjustedY)
{
	return adjustedY + drawUpperLeft.y;
}

void CaneCustomizeViewWidget :: updateIndexes(QPoint pos)
{
	int drawSize = squareSize - 20;
	float delta_x = (adjustedX(pos.x()) - clickedLoc.x);
	float delta_y = (adjustedY(pos.y()) - clickedLoc.y);
	float dist = sqrt(delta_x * delta_x + delta_y * delta_y); 
	if (dist > boundingBoxSpace)
	{
		clickMoved = true;
	}

	if (adjustedX(pos.x()) < activeBoxLL.x - 6*boundingBoxSpace ||
		adjustedY(pos.y()) < activeBoxLL.y - 6*boundingBoxSpace ||
		adjustedX(pos.x()) > activeBoxUR.x + 6*boundingBoxSpace ||
		adjustedY(pos.y()) > activeBoxUR.y + 6*boundingBoxSpace )
	{
		activeBoxIndex = -1;
	}
	else if (activeBoxIndex == -1)
	{
		activeBoxIndex = INT_MAX;
	}

	if (activeBoxIndex != -1 && activeBoxIndex != INT_MAX)
	{
		SubcaneTemplate subcane = cane->subcaneTemplate(activeBoxIndex);
		float dx = fabs(adjustedX(pos.x()) - (drawSize/2 * subcane.location.x + drawSize/2 + 10));
		float dy = fabs(adjustedY(pos.y()) - (drawSize/2 * subcane.location.y + drawSize/2 + 10));
		if (MAX(dx, dy) >= (subcane.diameter/2.0) * drawSize/2 + 3*boundingBoxSpace)
		{
			activeBoxIndex = INT_MAX;
		}
	}

	if (activeBoxIndex == INT_MAX)
	{
		for (unsigned int i = 0; i < subcanesSelected.size(); i++)
		{
			SubcaneTemplate subcane = cane->subcaneTemplate(subcanesSelected[i]);
			float dx = fabs(adjustedX(pos.x()) - (drawSize/2 * subcane.location.x + drawSize/2 + 10));
			float dy = fabs(adjustedY(pos.y()) - (drawSize/2 * subcane.location.y + drawSize/2 + 10));

			switch (subcane.shape)
			{
				case CIRCLE_SHAPE:
					if (pow(double(dx*dx + dy*dy), 0.5) < (subcane.diameter/2.0)*drawSize/2)
					{
						activeBoxIndex = subcanesSelected[i];
					}
					break;
				case SQUARE_SHAPE:
					if (MAX(dx, dy) < (subcane.diameter/2.0)*drawSize/2)
					{
						activeBoxIndex = subcanesSelected[i];
					}
					break;
			}
		}
	}

	if (activeBoxIndex == INT_MAX)
	{
		for (unsigned int i = 0; i < subcanesSelected.size(); i++)
		{
			SubcaneTemplate subcane = cane->subcaneTemplate(subcanesSelected[i]);
			float dx = fabs(adjustedX(pos.x()) - (drawSize/2 * subcane.location.x + drawSize/2 + 10));
			float dy = fabs(adjustedY(pos.y()) - (drawSize/2 * subcane.location.y + drawSize/2 + 10));
			if (MAX(dx, dy) < (subcane.diameter/2.0) * drawSize/2 + boundingBoxSpace)
			{
				activeBoxIndex = subcanesSelected[i];
				break;
			}
		}
	}

	hoveringIndex = -1;
	for (unsigned int i = 0; i < cane->subcaneCount(); ++i)
	{
		SubcaneTemplate subcane = cane->subcaneTemplate(i);
		float dx = fabs(adjustedX(pos.x()) - (drawSize/2 * subcane.location.x + drawSize/2 + 10));
		float dy = fabs(adjustedY(pos.y()) - (drawSize/2 * subcane.location.y + drawSize/2 + 10));
		switch (subcane.shape)
		{
			case CIRCLE_SHAPE:
				if (pow(double(dx*dx + dy*dy), 0.5) < (subcane.diameter/2.0)*drawSize/2)
				{
					hoveringIndex = i;
				}
				break;
			case SQUARE_SHAPE:
				if (MAX(dx, dy) < (subcane.diameter/2.0) * drawSize/2)
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

void CaneCustomizeViewWidget :: mouseMoveEvent(QMouseEvent* event)
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
			float delta_x = adjustedX(event->pos().x()) - clickedLoc.x;
			float delta_y = adjustedY(event->pos().y()) - clickedLoc.y;
			float dist = sqrt(delta_x * delta_x + delta_y * delta_y); 
			if (dist > boundingBoxSpace)
			{
				clickMoved = true;
			}

			if (isValidMovePosition(event))
			{
				for (unsigned int i = 0; i < subcanesSelected.size(); i++)
				{
					SubcaneTemplate sub = cane->subcaneTemplate(subcanesSelected[i]);
					sub.location.x += (adjustedX(event->pos().x()) - mouseStartingLoc.x)/(squareSize/2.0 - 10);
					sub.location.y += (adjustedY(event->pos().y()) - mouseStartingLoc.y)/(squareSize/2.0 - 10);
					cane->setSubcaneTemplate(sub, subcanesSelected[i]);
				}
				if (subcanesSelected.size() > 0)
					GlobalUndoRedo::modifiedCane(cane);
				mouseStartingLoc.x = adjustedX(event->pos().x());
				mouseStartingLoc.y = adjustedY(event->pos().y());
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
				SubcaneTemplate subcane = cane->subcaneTemplate(activeBoxIndex);
				float dx = fabs(adjustedX(event->pos().x()) 
					- (drawSize/2 * subcane.location.x + drawSize/2 + 10));
				float dy = fabs(adjustedY(event->pos().y()) 
					- (drawSize/2 * subcane.location.y + drawSize/2 + 10));
				float new_diameter = MAX(dx,dy);
				new_diameter -= boundingBoxSpace;
				new_diameter /= float(drawSize/2);
				new_diameter *= 2.0;
				float proportion = new_diameter / subcane.diameter;
				for (unsigned int i = 0; i < subcanesSelected.size(); i++)
				{
					SubcaneTemplate sub = cane->subcaneTemplate(subcanesSelected[i]);
					sub.diameter *= proportion;
					cane->setSubcaneTemplate(sub, subcanesSelected[i]);
				}
				if (subcanesSelected.size() > 0)
					GlobalUndoRedo::modifiedCane(cane);
			}
			else
			{
				float center_x = activeBoxLL.x/2.0 + activeBoxUR.x/2.0;
				float center_y = activeBoxLL.y/2.0 + activeBoxUR.y/2.0;
				float dx = fabs(adjustedX(event->pos().x()) - center_x)-3*boundingBoxSpace;
				float dy = fabs(adjustedY(event->pos().y()) - center_y)-3*boundingBoxSpace;
				float yx_proportion = (activeBoxUR.y-activeBoxLL.y)/(activeBoxUR.x-activeBoxLL.x);
				// no = new/old
				float no_proportion = 1.0;

				if (dy > dx*yx_proportion)
				{
					no_proportion = dy/(activeBoxUR.y/2.0 - activeBoxLL.y/2.0);
				}
				else
				{
					no_proportion = dx/(activeBoxUR.x/2.0 - activeBoxLL.x/2.0);
				}

				for (unsigned int i = 0; i < subcanesSelected.size(); i++)
				{
					SubcaneTemplate sub = cane->subcaneTemplate(subcanesSelected[i]);
					sub.diameter *= no_proportion;
					sub.location.x = (center_x - 10 - drawSize/2) / double(drawSize/2.0) 
						+ ((sub.location.x - (center_x - 10 - drawSize/2) / double(drawSize/2.0)) * no_proportion);
					sub.location.y = (center_y - 10 - drawSize/2) / double(drawSize/2.0) 
						+ ((sub.location.y - (center_y - 10 - drawSize/2)/double(drawSize/2.0))*no_proportion);
					cane->setSubcaneTemplate(sub, subcanesSelected[i]);
				}
				if (subcanesSelected.size() > 0)
					GlobalUndoRedo::modifiedCane(cane);
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
		SubcaneTemplate subcane = cane->subcaneTemplate(activeBoxIndex);
		float dx = fabs(adjustedX(event->pos().x()) - (drawSize/2 * subcane.location.x + drawSize/2 + 10));
		float dy = fabs(adjustedY(event->pos().y()) - (drawSize/2 * subcane.location.y + drawSize/2 + 10));
		if (fabs((subcane.diameter/2.0)*drawSize/2 - dx) < boundingBoxSpace &&
		    fabs((subcane.diameter/2.0)*drawSize/2 - dy) < boundingBoxSpace)
		{
			// not yet implemented
		}
	}
	else if (activeBoxIndex == INT_MAX)
	{
		if((fabs(float(adjustedX(event->pos().x()) - activeBoxLL.x - 3*boundingBoxSpace)) < boundingBoxSpace ||
			fabs(float(adjustedX(event->pos().x()) - activeBoxUR.x - 3*boundingBoxSpace)) < boundingBoxSpace) &&
			(fabs(float(adjustedY(event->pos().y()) - activeBoxLL.y + 3*boundingBoxSpace)) < boundingBoxSpace ||
			fabs(float(adjustedY(event->pos().y()) - activeBoxUR.y + 3*boundingBoxSpace)) < boundingBoxSpace))
		{
			// not yet implemented
		}
	}
}

void CaneCustomizeViewWidget :: mousePressEvent(QMouseEvent* event)
{
	int drawSize = squareSize - 20;
	mode = MOVE_MODE;
	clickedLoc.x = adjustedX(event->pos().x());
	clickedLoc.y = adjustedY(event->pos().y());
	clickMoved = false;
	if (activeBoxIndex != -1 && activeBoxIndex != INT_MAX)
	{
		SubcaneTemplate subcane = cane->subcaneTemplate(activeBoxIndex);
		float dx = fabs(adjustedX(event->pos().x()) - (drawSize/2 * subcane.location.x + drawSize/2 + 10));
		float dy = fabs(adjustedY(event->pos().y()) - (drawSize/2 * subcane.location.y + drawSize/2 + 10));
		if (fabs((subcane.diameter/2.0)*drawSize/2+boundingBoxSpace - dx) < 2*boundingBoxSpace &&
			fabs((subcane.diameter/2.0)*drawSize/2+boundingBoxSpace - dy) < 2*boundingBoxSpace)
		{
			mode = SCALE_MODE;
		}
	}
	else if (activeBoxIndex == INT_MAX)
	{
		if((fabs(float(adjustedX(event->pos().x()) - activeBoxLL.x + 3*boundingBoxSpace)) < 2*boundingBoxSpace ||
			fabs(float(adjustedX(event->pos().x()) - activeBoxUR.x - 3*boundingBoxSpace)) < 2*boundingBoxSpace) &&
			(fabs(float(adjustedY(event->pos().y()) - activeBoxLL.y + 3*boundingBoxSpace)) < 2*boundingBoxSpace ||
			fabs(float(adjustedY(event->pos().y()) - activeBoxUR.y - 3*boundingBoxSpace)) < 2*boundingBoxSpace))
		{
			mode = SCALE_MODE;
		}
	}

	if (mode == MOVE_MODE)
	{
		bool isIn = false;
		for (unsigned int i = 0; i < subcanesSelected.size(); i++)
		{
			if (hoveringIndex == int(subcanesSelected.at(i)))
			{
				isIn = true;
				break;
			}
		}
		if (!isIn && hoveringIndex != -1)
		{
			if (event->modifiers() != Qt::ControlModifier && event->modifiers() != Qt::ShiftModifier)
			{
				subcanesSelected.clear();
			}
			else
			{
				clickMoved = true;
			}
			subcanesSelected.push_back((unsigned int)hoveringIndex);
			activeBoxIndex = hoveringIndex;
		}
		if (activeBoxIndex == -1)
		{
			subcanesSelected.clear();
		}
	}

	mouseStartingLoc.x = adjustedX(event->pos().x());
	mouseStartingLoc.y = adjustedY(event->pos().y());
	boundActiveBox();
	updateIndexes(event->pos());
	update();
}

void CaneCustomizeViewWidget :: mouseReleaseEvent(QMouseEvent* event)
{
	if (!clickMoved)
	{
		if (event->modifiers() != Qt::ControlModifier && event->modifiers() != Qt::ShiftModifier)
		{
			subcanesSelected.clear();
			if (hoveringIndex != -1)
			{
				subcanesSelected.push_back((unsigned int)hoveringIndex);
				activeBoxIndex = hoveringIndex;
			}
		}
		else
		{
			for (unsigned int i = 0; i < subcanesSelected.size(); i++)
			{
				if (hoveringIndex == int(subcanesSelected.at(i)))
				{
					subcanesSelected.erase(subcanesSelected.begin()+i);
					break;
				}
			}
		}
	}
	
	mouseStartingLoc.x = adjustedX(event->pos().x());
	mouseStartingLoc.y = adjustedY(event->pos().y());
	boundActiveBox();
	updateIndexes(event->pos());
	update();
}

void CaneCustomizeViewWidget :: keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
	{
		deleteSelection();
		return;
	}

	float moveDist = 0.01f;
	if (event->key() == Qt::Key_Left)
		moveSelection(-moveDist, 0);	
	else if (event->key() == Qt::Key_Right)
		moveSelection(moveDist, 0);	
	else if (event->key() == Qt::Key_Down)
		moveSelection(0, moveDist);
	else if (event->key() == Qt::Key_Up)
		moveSelection(0, -moveDist);
}

void CaneCustomizeViewWidget :: setCane(Cane* _cane)
{
	cane = _cane;

	// reset gui stuffs
	subcanesSelected.clear();
	activeControlPoint = -1;
	hoveringIndex = -1;
	activeBoxIndex = -1;
	updateEverything();

	mouseStartingLoc.x = FLT_MAX;
	mouseStartingLoc.y = FLT_MAX;
	boundActiveBox();
}

void CaneCustomizeViewWidget :: updateEverything()
{
	this->repaint();
}

bool CaneCustomizeViewWidget :: isValidMovePosition(QMouseEvent*)
{
	/* This should check if the subcane can be moved to the given position
	  (i.e. not intersecting with another subcane or leaving the casing).
	  For now it doesn't prevent anything. */
	return true;
}

void CaneCustomizeViewWidget :: boundActiveBox()
{
	activeBoxLL.x = activeBoxLL.y = INT_MAX;
	activeBoxUR.x = activeBoxUR.y = INT_MIN;
	int drawSize = squareSize - 20;
	for (unsigned int i = 0; i < subcanesSelected.size(); i++)
	{
		SubcaneTemplate subcane = cane->subcaneTemplate(subcanesSelected[i]);
		float dx = drawSize/2 * subcane.location.x + drawSize/2 + 10;
		float dy = drawSize/2 * subcane.location.y + drawSize/2 + 10;
		if (dx - (subcane.diameter/2.0)*drawSize/2 < activeBoxLL.x)
		    activeBoxLL.x = dx - (subcane.diameter/2.0)*drawSize/2;
		if (dy - (subcane.diameter/2.0)*drawSize/2 < activeBoxLL.y)
		    activeBoxLL.y = dy - (subcane.diameter/2.0)*drawSize/2;
		if (dx + (subcane.diameter/2.0)*drawSize/2 > activeBoxUR.x)
		    activeBoxUR.x = dx + (subcane.diameter/2.0)*drawSize/2;
		if (dy + (subcane.diameter/2.0)*drawSize/2 > activeBoxUR.y)
		    activeBoxUR.y = dy + (subcane.diameter/2.0)*drawSize/2;
	}
}

void CaneCustomizeViewWidget :: setBoundaryPainter(QPainter* painter, bool outermostLevel, bool greyedOut = false) 
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

void CaneCustomizeViewWidget :: paintShape(Point2D upperLeft, float size, enum GeometricShape shape, QPainter* painter)
{
	int roundedX = rawX(upperLeft.x) + 0.5;
	int roundedY = rawY(upperLeft.y) + 0.5;
	//int roundedSize = size;
	if (size < 1) // If size is 0, drawEllipse/drawRect will cause a failed assertion it seems
		return;

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
void CaneCustomizeViewWidget :: drawSubcane(Point2D upperLeft, float drawWidth, float drawHeight,
	Cane* cane, bool outermostLevel, QPainter* painter)
{
	// Fill the subcane area with some `cleared out' color
	painter->setBrush(GlobalBackgroundColor::qcolor);
	painter->setPen(Qt::NoPen);
	paintShape(upperLeft, drawWidth, cane->outermostCasingShape(), painter);

	// Do casing colors outermost to innermost to get concentric rings of each casing's color
	// Skip outermost casing (that is done by your parent) and innermost (that is the `invisible'
	// casing for you to resize your subcanes)
	for (unsigned int i = cane->casingCount() - 1; i < cane->casingCount(); --i)
	{
		float casingWidth = drawWidth * cane->casingThickness(i);
		float casingHeight = drawHeight * cane->casingThickness(i);
		Point2D casingUpperLeft;
		casingUpperLeft.x = upperLeft.x + drawWidth / 2 - casingWidth / 2;
		casingUpperLeft.y = upperLeft.y + drawHeight / 2 - casingHeight / 2;

		// Fill with solid neutral grey (in case fill is transparent)
		painter->setBrush(GlobalBackgroundColor::qcolor);
		painter->setPen(Qt::NoPen); // Will draw boundary after all filling is done
		paintShape(casingUpperLeft, casingWidth, cane->casingShape(i), painter);

		Color casingColor = cane->casingColor(i)->color();
		QColor qc;
		qc.setRgbF(casingColor.r, casingColor.g, casingColor.b, casingColor.a); 
		painter->setBrush(qc);
		setBoundaryPainter(painter, outermostLevel, outermostLevel);
		paintShape(casingUpperLeft, casingWidth, cane->casingShape(i), painter);
	}

	// Recursively call drawing on subcanes
	for (unsigned int i = cane->subcaneCount()-1; i < cane->subcaneCount(); --i)
	{
		SubcaneTemplate subcane = cane->subcaneTemplate(i);

		Point2D subUpperLeft;
		subUpperLeft.x = upperLeft.x + (subcane.location.x - subcane.diameter/2.0) * drawWidth/2 + drawWidth/2;
		subUpperLeft.y = upperLeft.y + (subcane.location.y - subcane.diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = subcane.diameter * drawWidth/2;
		float rHeight = subcane.diameter * drawHeight/2;

		drawSubcane(subUpperLeft, rWidth, rHeight, subcane.cane, false, painter);

		setBoundaryPainter(painter, outermostLevel);
		painter->setBrush(Qt::NoBrush);
		paintShape(subUpperLeft, rWidth, subcane.shape, painter);
	}
}

void CaneCustomizeViewWidget :: drawActionControls(QPainter* painter)
{
	int drawSize = squareSize - 20;

	// Outline for individual selected canes
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::black);
	pen.setStyle(Qt::DotLine);
	painter->setBrush(Qt::NoBrush); 
	painter->setPen(pen);
	for (unsigned int i = 0; i < subcanesSelected.size(); i++)
	{
		SubcaneTemplate subcane = cane->subcaneTemplate(subcanesSelected[i]);
		painter->drawRect(rawX((subcane.location.x - subcane.diameter/2.0)*drawSize/2 + drawSize/2 + 10) - boundingBoxSpace, 
			rawY((subcane.location.y - subcane.diameter/2.0)*drawSize/2+drawSize/2+10) - boundingBoxSpace,
			subcane.diameter*drawSize/2.0 + 2*boundingBoxSpace,
			subcane.diameter*drawSize/2.0 + 2*boundingBoxSpace);
	}

	// Outline for a group selection
	if (subcanesSelected.size() > 1)
	{
		painter->drawRect(rawX(activeBoxLL.x)-3*boundingBoxSpace,
			rawY(activeBoxLL.y)-3*boundingBoxSpace,
			(activeBoxUR.x-activeBoxLL.x)+6*boundingBoxSpace,
			(activeBoxUR.y-activeBoxLL.y)+6*boundingBoxSpace);
	}

	// White control points for resizing
	painter->setBrush(QColor(0, 0, 0, 255));
	pen.setWidth(2);
	pen.setColor(Qt::white);
	pen.setStyle(Qt::SolidLine);
	painter->setPen(pen);
	if (activeBoxIndex != -1 && activeBoxIndex != INT_MAX)
	{
		SubcaneTemplate subcane = cane->subcaneTemplate(activeBoxIndex);
		painter->drawEllipse(rawX((subcane.location.x - subcane.diameter/2.0)*drawSize/2+drawSize/2+10)-boundingBoxSpace*2,
			rawY((subcane.location.y - subcane.diameter/2.0)*drawSize/2+drawSize/2+10)-boundingBoxSpace*2,
			boundingBoxSpace*2,
			boundingBoxSpace*2);
		painter->drawEllipse(rawX((subcane.location.x - subcane.diameter/2.0)*drawSize/2+drawSize/2+10)-boundingBoxSpace*2,
			rawY((subcane.location.y + subcane.diameter/2.0)*drawSize/2+drawSize/2+10)+boundingBoxSpace*0,
			boundingBoxSpace*2,
			boundingBoxSpace*2);
		painter->drawEllipse(rawX((subcane.location.x + subcane.diameter/2.0)*drawSize/2+drawSize/2+10)+boundingBoxSpace*0,
			rawY((subcane.location.y - subcane.diameter/2.0)*drawSize/2+drawSize/2+10)-boundingBoxSpace*2,
			boundingBoxSpace*2,
			boundingBoxSpace*2);
		painter->drawEllipse(rawX((subcane.location.x + subcane.diameter/2.0)*drawSize/2+drawSize/2+10)+boundingBoxSpace*0,
			rawY((subcane.location.y + subcane.diameter/2.0)*drawSize/2+drawSize/2+10)+boundingBoxSpace*0,
			boundingBoxSpace*2,
			boundingBoxSpace*2);
	}
	if (subcanesSelected.size() > 1 && activeBoxIndex == INT_MAX)
	{
		painter->drawEllipse(rawX(activeBoxLL.x)-4*boundingBoxSpace,
			rawY(activeBoxLL.y)-4*boundingBoxSpace,
			boundingBoxSpace*2,
			boundingBoxSpace*2);
		painter->drawEllipse(rawX(activeBoxLL.x)-4*boundingBoxSpace,
			rawY(activeBoxUR.y)+2*boundingBoxSpace,
			boundingBoxSpace*2,
			boundingBoxSpace*2);
		painter->drawEllipse(rawX(activeBoxUR.x)+2*boundingBoxSpace,
			rawY(activeBoxLL.y)-4*boundingBoxSpace,
			boundingBoxSpace*2,
			boundingBoxSpace*2);
		painter->drawEllipse(rawX(activeBoxUR.x)+2*boundingBoxSpace,
			rawY(activeBoxUR.y)+2*boundingBoxSpace,
			boundingBoxSpace*2,
			boundingBoxSpace*2);
	}
}

void CaneCustomizeViewWidget :: paintEvent(QPaintEvent *event)
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
	paintShape(drawUpperLeft, squareSize - 20, cane->outermostCasingShape(), &painter);

	drawSubcane(drawUpperLeft, squareSize - 20, squareSize - 20, cane, true, &painter);

	drawActionControls(&painter);

	painter.end();
}

void CaneCustomizeViewWidget :: copySelectionClicked()
{
	copySelection();
}

void CaneCustomizeViewWidget :: copySelection()
{
	if (subcanesSelected.size() == 0)
		return;
	vector<SubcaneTemplate> newSubcanes;
	newSubcanes.clear();
	for (unsigned int i = 0; i < subcanesSelected.size(); i++)
	{
		SubcaneTemplate sub = cane->subcaneTemplate(subcanesSelected[i]);
		sub.location.x += 3*boundingBoxSpace/squareSize;
		sub.location.y += 3*boundingBoxSpace/squareSize;
		newSubcanes.push_back(sub);
	}
	int oldCount = cane->subcaneCount();
	for (unsigned int i = 0; i < subcanesSelected.size(); i++)
	{
		cane->addSubcaneTemplate(newSubcanes[i]);	
	}
	GlobalUndoRedo::modifiedCane(cane);
	subcanesSelected.clear();
	for (unsigned int i = 0; i < newSubcanes.size(); i++)
	{
		subcanesSelected.push_back(i+oldCount);
	}
	boundActiveBox();
	update();
}

void CaneCustomizeViewWidget :: deleteSelectionClicked()
{
	deleteSelection();
}

void CaneCustomizeViewWidget :: deleteSelection()
{
	for (unsigned int i = 0; i < subcanesSelected.size(); i++)
	{
		unsigned int n = subcanesSelected[i];
		cane->removeSubcaneTemplate(n);
		for (unsigned int j = i+1; j < subcanesSelected.size(); j++)
		{
			if (subcanesSelected[j] > n)
				subcanesSelected[j] -= 1;
		}
	}
	GlobalUndoRedo::modifiedCane(cane);
	subcanesSelected.clear();
	hoveringIndex = -1;
	activeBoxIndex = -1;
	activeControlPoint = -1;
	boundActiveBox();
	update();
}

void CaneCustomizeViewWidget :: moveSelection(float xRaw, float yRaw)
{
	for (unsigned int i = 0; i < subcanesSelected.size(); i++)
	{
		SubcaneTemplate temp = cane->subcaneTemplate(subcanesSelected[i]);
		temp.location.x = adjustedX(rawX(temp.location.x) + xRaw);  
		temp.location.y = adjustedY(rawY(temp.location.y) + yRaw); 
		cane->setSubcaneTemplate(temp, subcanesSelected[i]);
	}
	GlobalUndoRedo::modifiedCane(cane);
}

void CaneCustomizeViewWidget :: addCircleClicked()
{
	Point2D p = make_vector(0.0f, 0.0f);
	float diameter = 0;
	for (unsigned int i = 0; i < cane->subcaneCount(); i++)
	{
		SubcaneTemplate subcane = cane->subcaneTemplate(i);
		diameter += subcane.diameter / cane->subcaneCount();
	}
	if (diameter == 0)
	{
		diameter = cane->casingThickness(0);
	}
	
	cane->addSubcaneTemplate(SubcaneTemplate(GlobalGlass::circleCane(), CIRCLE_SHAPE, p, diameter));
	GlobalUndoRedo::modifiedCane(cane);
	subcanesSelected.clear();
	subcanesSelected.push_back(cane->subcaneCount()-1);
	boundActiveBox();
	update();
}

void CaneCustomizeViewWidget :: addSquareClicked()
{
	Point2D p = make_vector(0.0f, 0.0f);
	float diameter = 0;
	for (unsigned int i = 0; i < cane->subcaneCount(); i++)
	{
		SubcaneTemplate subcane = cane->subcaneTemplate(i);
		diameter += subcane.diameter / cane->subcaneCount();
	}
	if (diameter == 0)
	{
		diameter = cane->casingThickness(0);
	}
	cane->addSubcaneTemplate(SubcaneTemplate(GlobalGlass::squareCane(), SQUARE_SHAPE, p, diameter));
	GlobalUndoRedo::modifiedCane(cane);
	subcanesSelected.clear();
	subcanesSelected.push_back(cane->subcaneCount()-1);
	boundActiveBox();
	update();
}




