
#include "constants.h"
#include "glasscolor.h"
#include "pullplan.h"
#include "glassmime.h"
#include "asyncpullplanlibrarywidget.h"
#include "asynccolorbarlibrarywidget.h"
#include "pullplaneditorviewwidget.h"

PullPlanEditorViewWidget :: PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setMinimumSize(200, 200);
	this->plan = plan;

	isDraggingColor = false;
	isDraggingPlan = false;
	isDraggingCasing = false;
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

float PullPlanEditorViewWidget :: getShapeRadius(enum GeometricShape shape, float x, float y)
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
	return fabs(plan->getCasingThickness(casingIndex) - getShapeRadius(plan->getCasingShape(casingIndex), x, y)) < 0.02; 
}

void PullPlanEditorViewWidget :: mousePressEvent(QMouseEvent* event)
{
	float x = (adjustedX(event->pos().x()) - squareSize/2) / float(squareSize/2-10);
	float y = (adjustedY(event->pos().y()) - squareSize/2) / float(squareSize/2-10);

	// Check for casing resize
	for (unsigned int i = 0; i < plan->getCasingCount() - 1; ++i) 
	{
		if (isOnCasing(i, x, y))
		{
			isDraggingCasing = true; 
			draggedCasingIndex = i;
			return;
		}	
	}

	// Check for convenience subplan-to-subplan drag
	PullPlan* selectedSubplan = getSubplanAt(x, y);
	if (selectedSubplan != NULL)
	{
		enum GlassMime::Type type = GlassMime::pullplan;
		AsyncPullPlanLibraryWidget plplw(selectedSubplan);		
		QPixmap pixmap = *(plplw.getDragPixmap());

	        char buf[500];
		GlassMime::encode(buf, selectedSubplan, type);
		QByteArray pointerData(buf);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText(pointerData);

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->setPixmap(pixmap);

		drag->exec(Qt::CopyAction);
	}	
}

PullPlan* PullPlanEditorViewWidget :: getSubplanAt(float x, float y)
{
	// Recursively call drawing on subplans
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		SubpullTemplate* sub = &(plan->subs[i]);
		if (getShapeRadius(sub->shape, x - sub->location.x, y - sub->location.y) < sub->diameter/2.0)
			return sub->plan;
	}			

	return NULL;
}

void PullPlanEditorViewWidget :: setMinMaxCasingRadii(float* min, float* max)
{
	/*
	Goal here is to deal with casings of different shapes, and set upper and 
	lower bounds for the radius of a particular casing based upon how much it
	can change before bumping into the inscribed (next smallest) or 
	circumscribed (next largest) casing. 

	The major situation/issue to deal with is adjusting by sqrt(2) in the case
	that the casing is square and is surrounded by two circle casings or vice versa.
	We also allow adjacent casings of different shapes to get really close (0.01)
	while casing of the same shape need to be spaced a little (0.05) for UX reasons:
	if they are too close, it becomes hard to tell there are two casings, or to 
	click on them reliably to resize them.
	*/
	
	int csi, csi_minus_1, csi_plus_1;
	float cti_minus_1, cti_plus_1;

	if (draggedCasingIndex == 0) 
	{
		csi = plan->getCasingShape(0);
		csi_minus_1 = csi;
		csi_plus_1 = plan->getCasingShape(1);
	
		cti_minus_1 = 0.0;
		cti_plus_1 = plan->getCasingThickness(1);	
	}
	else 
	{
		csi = plan->getCasingShape(draggedCasingIndex);
		csi_minus_1 = plan->getCasingShape(draggedCasingIndex-1);
		csi_plus_1 = plan->getCasingShape(draggedCasingIndex+1);
		cti_minus_1 = plan->getCasingThickness(draggedCasingIndex-1);
		cti_plus_1 = plan->getCasingThickness(draggedCasingIndex+1);
	}
		
	if (csi == CIRCLE_SHAPE && csi_minus_1 == SQUARE_SHAPE)
		*min = cti_minus_1 * sqrt(2.0) + 0.01;
	else if (csi == SQUARE_SHAPE && csi_minus_1 == CIRCLE_SHAPE)
		*min = cti_minus_1 + 0.01;
	else
		*min = cti_minus_1 + 0.02;

	if (csi == SQUARE_SHAPE && csi_plus_1 == CIRCLE_SHAPE)
		*max = cti_plus_1 / sqrt(2.0) - 0.01;
	else if (csi == CIRCLE_SHAPE && csi_plus_1 == SQUARE_SHAPE)
		*max = cti_plus_1 - 0.01;
	else
		*max = cti_plus_1 - 0.02;
} 

void PullPlanEditorViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
	// bug in here?
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
}

void PullPlanEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
	void* ptr;
	enum GlassMime::Type type;
	GlassMime::decode(event->mimeData()->text().toAscii().constData(), &ptr, &type);
	switch (type)
	{
		case GlassMime::colorbar:
			isDraggingColor = true;
			draggedColor = reinterpret_cast<GlassColor*>(ptr);
			break;
		case GlassMime::pullplan:
			isDraggingPlan = true;
			draggedPlan = reinterpret_cast<PullPlan*>(ptr);
			break;
	}
}

void PullPlanEditorViewWidget :: dragLeaveEvent(QDragLeaveEvent* /*event*/)
{
	subplansHighlighted.clear();
	casingsHighlighted.clear();
	isDraggingPlan = isDraggingColor = false;
}


void PullPlanEditorViewWidget :: updateHighlightedSubplansAndCasings(QDropEvent* event)
{
	QPoint mouse = mapFromGlobal(QCursor::pos());
	int x = adjustedX(mouse.x());
	int y = adjustedY(mouse.y());

	subplansHighlighted.clear();
	casingsHighlighted.clear();

	populateHighlightedSubplans(x, y, event);
	if (subplansHighlighted.size() > 0)  // if you hit something, set highlight color and get out
	{
		if (isDraggingColor)
			highlightColor = *(draggedColor->getColor());
		else
			highlightColor.r = highlightColor.g = highlightColor.b = highlightColor.a = 1.0;
		return;
	}
	populateHighlightedCasings(x, y);
	if (casingsHighlighted.size() > 0) 
	{
		highlightColor = *(draggedColor->getColor());
		return;
	}
}

void PullPlanEditorViewWidget :: dragMoveEvent(QDragMoveEvent* event)
{
	updateHighlightedSubplansAndCasings(event);
	repaint();
}

void PullPlanEditorViewWidget :: dropEvent(QDropEvent* event)
{
	// subplans highlighted can mean you're dragging color or plan
	if (subplansHighlighted.size() > 0)
	{
		event->accept();
		for (unsigned int i = 0; i < subplansHighlighted.size(); ++i)
		{
			if (isDraggingColor)
			{
				// this is a memory leak as of Jan 31, 2013.
				// need to either:
				// 1. make user responsible for it by adding to the library
				// 2. do primitive reference counting 
				// 3. give it a special "this is just a dropped color plan" mark
				// 4. accept that the memory is lost forever 
				PullPlan* newColorBarPlan = new PullPlan(PullTemplate::BASE_CIRCLE);
				newColorBarPlan->setCasingColor(draggedColor, 0);
				newColorBarPlan->setOutermostCasingShape(plan->subs[subplansHighlighted[i]].shape);
				plan->subs[subplansHighlighted[i]].plan = newColorBarPlan;
			}
			else // isDraggingPlan == true
				plan->subs[subplansHighlighted[i]].plan = draggedPlan;
		}
	}
	// casings highlighted is definitely a dragged plan
	else if (casingsHighlighted.size() > 0)
	{
		event->accept();
		plan->setCasingColor(draggedColor, casingsHighlighted[0]);
	}

	isDraggingPlan = isDraggingColor = false;
	subplansHighlighted.clear();
	casingsHighlighted.clear();
	emit someDataChanged();
}

void PullPlanEditorViewWidget :: populateHighlightedSubplans(int x, int y, QDropEvent* event)
{
	// the goal is to populate the list of subplans hit by the drag
	// if the drag is color, then we hit no matter what due to the amorphous shape of color
	// if the drag is a plan, then we have to do type-checking for correct shape
	subplansHighlighted.clear();

	// if you ain't draggin, get out
	if (!isDraggingColor && !isDraggingPlan)
		return;

	// can't drag into yourself
	if (isDraggingPlan && draggedPlan->hasDependencyOn(plan)) 
		return;

	int drawSize = squareSize - 20;
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->subs[i]);

		// Determine if drop hit the subplan
		float dx = x - (drawSize/2 * subpull->location.x + drawSize/2 + 10);
		float dy = y - (drawSize/2 * subpull->location.y + drawSize/2 + 10);
		bool hit = (getShapeRadius(subpull->shape, dx, dy) < (subpull->diameter/2.0) * drawSize/2);
		
		if (!hit)
			continue;

		// if you're dragging color, you win
		if (isDraggingColor)
		{
			subplansHighlighted.push_back(i);
		}
		// else have to do shape type-checking
		else if (/* isDraggingPlan && */ subpull->shape == draggedPlan->getOutermostCasingShape())
		{
			subplansHighlighted.push_back(i);
		}

	}
	
	// the remainder of the code is to handle the "fill-all" behavior when the shift key is being held down
	
	// if user is hovering over a subplan and the shift key is currently held down, fill in all subplans
	// Note that this needs to wait until another event (say, a drag move) occurs to catch the shift button being down.
	// The crazy thing is, on Windows a drag *blocks* the event loop, preventing the whole application from
	// getting a keyPressEvent() until the drag is completed. So reading keyboardModifiers() actually 
	// lets you notice that the shift key is down earlier, i.e. during the drag, which is the only time you care anyway.
	if (subplansHighlighted.size() > 0 && event && (event->keyboardModifiers() & Qt::ShiftModifier))
	{
		// fill er up
		subplansHighlighted.clear();	
		for (unsigned int i = 0; i < plan->subs.size(); ++i)
		{
			SubpullTemplate* subpull = &(plan->subs[i]);
			if (isDraggingColor)
				subplansHighlighted.push_back(i);
			else if (/* isDraggingPlan && */ subpull->shape == draggedPlan->getOutermostCasingShape())
				subplansHighlighted.push_back(i);
		}
	}
}


void PullPlanEditorViewWidget :: populateHighlightedCasings(int x, int y)
{
	if (!isDraggingColor)
		return; 

	// Deal w/casing
	float drawSize = (squareSize - 20);
	float distanceFromCenter;
	for (unsigned int i = 0; i < plan->getCasingCount(); ++i) {
		switch (plan->getCasingShape(i)) {
			case CIRCLE_SHAPE:
				distanceFromCenter = sqrt(pow(x - (drawSize/2.0 + 10.0), 2.0)
					+ pow(y - (drawSize/2.0 + 10.0), 2.0));
				if (distanceFromCenter <= drawSize/2 * plan->getCasingThickness(i))
				{
					casingsHighlighted.push_back(i);
					return;
				}
				break;
			case SQUARE_SHAPE:
				if (MAX(fabs(x - squareSize/2.0), fabs(y - squareSize/2.0)) < drawSize/2 * plan->getCasingThickness(i))
				{
					casingsHighlighted.push_back(i);
					return;
				}
				break;
		}
	}
}

void PullPlanEditorViewWidget :: setPullPlan(PullPlan* plan) 
{
	this->plan = plan;
	updateHighlightedSubplansAndCasings(NULL);
	this->repaint();	
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
		painter->setPen(Qt::NoPen);
	}
}

void PullPlanEditorViewWidget :: paintShape(float x, float y, float size, enum GeometricShape shape, QPainter* painter)
{
	int roundedX, roundedY;

	roundedX = floor(rawX(x) + 0.5);
	roundedY = floor(rawY(y) + 0.5);

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


void PullPlanEditorViewWidget :: drawSubplan(float x, float y, float drawWidth, float drawHeight, 
	PullPlan* plan, bool highlightThis, bool outermostLevel, QPainter* painter) {

	// Fill the subplan area with some `cleared out' color
	painter->setBrush(QColor(200, 200, 200));
	painter->setPen(Qt::NoPen);
	paintShape(x, y, drawWidth, plan->getOutermostCasingShape(), painter);

	if (highlightThis)
	{
		painter->setBrush(QColor(255*highlightColor.r, 255*highlightColor.g, 255*highlightColor.b,
			255*highlightColor.a));
		painter->setPen(Qt::NoPen);
		paintShape(x, y, drawWidth, plan->getOutermostCasingShape(), painter);
		return;
	}

	// Do casing colors outermost to innermost to get concentric rings of each casing's color
	for (unsigned int i = plan->getCasingCount() - 1; i < plan->getCasingCount(); --i) 
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
		if (outermostLevel && casingsHighlighted.size() > 0 && casingsHighlighted[0] == i)
		{
			painter->setBrush(QColor(255*highlightColor.r, 255*highlightColor.g, 255*highlightColor.b, 
				255*highlightColor.a));
		}
		else
		{
			Color* c = plan->getCasingColor(i)->getColor();
			QColor qc(255*c->r, 255*c->g, 255*c->b, 255*c->a);
			painter->setBrush(qc);
		}

		setBoundaryPainter(painter, outermostLevel);
		paintShape(casingX, casingY, casingWidth, plan->getCasingShape(i), painter);
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

			drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
				highlighted, false, painter);
		}
		else {
			drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
				false, false, painter);
		}
		
		painter->setBrush(Qt::NoBrush);
		setBoundaryPainter(painter, outermostLevel); 
		paintShape(rX, rY, rWidth, plan->subs[i].shape, painter);
	}
}

void PullPlanEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;

	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(event->rect(), QColor(200, 200, 200));
	drawSubplan(10, 10, squareSize - 20, squareSize - 20, plan, false, true, &painter);

	painter.setBrush(Qt::NoBrush);
	setBoundaryPainter(&painter, true);
	paintShape(10, 10, squareSize - 20, plan->getOutermostCasingShape(), &painter);

	painter.end();
}
