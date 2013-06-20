
#include <QResizeEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QPainter>

#include "constants.h"
#include "glasscolor.h"
#include "pullplan.h"
#include "glassmime.h"
#include "pullplanlibrarywidget.h"
#include "glasscolorlibrarywidget.h"
#include "pullplaneditorviewwidget.h"
#include "globalbackgroundcolor.h"
#include "pullplancrosssectionrender.h"

PullPlanEditorViewWidget :: PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setMinimumSize(200, 200);
	this->plan = plan;
	isDraggingCasing = false;
}


QRect PullPlanEditorViewWidget :: usedRect()
{
	return QRect(drawUpperLeft.x, drawUpperLeft.y, squareSize, squareSize);
}

void PullPlanEditorViewWidget :: resizeEvent(QResizeEvent* event)
{
	int width, height;

	width = event->size().width();
	height = event->size().height();

	if (width > height) // wider than tall 
	{
		drawUpperLeft.x = (width - height)/2.0;
		drawUpperLeft.y = 0.0;
		squareSize = height; 
	}
	else  
	{
		drawUpperLeft.x = 0.0;
		drawUpperLeft.y = (height - width)/2.0;
		squareSize = width; 
	}
}

float PullPlanEditorViewWidget :: getShapeRadius(enum GeometricShape shape, Point2D loc)
{
	switch (shape)
	{
		case CIRCLE_SHAPE:
			return sqrt(loc.x * loc.x + loc.y * loc.y); 
		case SQUARE_SHAPE:
			return MAX(fabs(loc.x), fabs(loc.y));
	}

	return -1.0;
}

bool PullPlanEditorViewWidget :: isOnCasing(int casingIndex, Point2D loc)
{
	return fabs(plan->getCasingThickness(casingIndex) - getShapeRadius(plan->getCasingShape(casingIndex), loc)) < 0.025; 
}

void PullPlanEditorViewWidget :: mousePressEvent(QMouseEvent* event)
{
	Point2D mouseLoc = mouseToCaneCoords(event->pos().x(), event->pos().y());

	// Check for casing resize
	for (unsigned int i = 0; i < plan->casingCount() - 1; ++i) 
	{
		if (isOnCasing(i, mouseLoc))
		{
			isDraggingCasing = true; 
			draggedCasingIndex = i;
			return;
		}	
	}

	// Check for convenience subplan-to-subplan drag
	PullPlan* selectedSubplan = getSubplanAt(mouseLoc);
	if (selectedSubplan != NULL)
	{
	        char buf[500];
		GlassMime::encode(buf, selectedSubplan, GlassMime::PULLPLAN_MIME);
		QByteArray pointerData(buf);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText(pointerData);

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);

		QPixmap pixmap(100, 100);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		PullPlanCrossSectionRender::render(&painter, 100, selectedSubplan);
		painter.end();
		drag->setPixmap(pixmap);

		drag->setHotSpot(QPoint(50, 100));
		drag->exec(Qt::CopyAction);

		return;
	}	

	// Check for convenience casing-to-casing drag
	int selectedCasingIndex = getCasingIndexAt(mouseLoc);
	if (selectedCasingIndex != -1)
	{
		const GlassColor* selectedColor = plan->getCasingColor(static_cast<unsigned int>(selectedCasingIndex)); 

		QPixmap _pixmap(200, 200);
		_pixmap.fill(Qt::transparent);
		QPainter painter(&_pixmap);
		Color c = selectedColor->color();
		QColor qc(255*c.r, 255*c.g, 255*c.b, MAX(255*c.a, 30));
		painter.setBrush(qc);
		painter.setPen(Qt::NoPen);
		painter.drawEllipse(10, 10, 180, 180);
		painter.end();
		QPixmap pixmap = _pixmap.scaled(100, 100);

	        char buf[500];
		GlassMime::encode(buf, selectedColor, GlassMime::COLOR_MIME);
		QByteArray pointerData(buf);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText(pointerData);

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->setPixmap(pixmap);
		drag->setHotSpot(QPoint(50, 100));

		drag->exec(Qt::CopyAction);

		return;
	}
}

int PullPlanEditorViewWidget :: getCasingIndexAt(Point2D loc)
{
	for (unsigned int i = 0; i < plan->casingCount(); ++i) 
	{
		if (getShapeRadius(plan->getCasingShape(i), loc) < plan->getCasingThickness(i))
			return i;
	}

	return -1;
}

int PullPlanEditorViewWidget :: getSubplanIndexAt(Point2D loc)
{
	// Recursively call drawing on subplans
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		SubpullTemplate& sub = plan->subs[i];
		Point2D delta;
		delta.x = loc.x - sub.location.x;
		delta.y = loc.y - sub.location.y;
		if (getShapeRadius(sub.shape, delta) < sub.diameter/2.0)
			return i;
	}

	return -1;			
}

PullPlan* PullPlanEditorViewWidget :: getSubplanAt(Point2D loc)
{
	int subplanIndex = getSubplanIndexAt(loc);
	if (subplanIndex == -1)
		return NULL;
	return plan->subs[subplanIndex].plan;
}

void PullPlanEditorViewWidget :: setMinMaxCasingRadii(float* min, float* max)
{
	// Goal here is to deal with casings of different shapes, and set upper and 
	// lower bounds for the radius of a particular casing based upon how much it
	// can change before bumping into the inscribed (next smallest) or 
	// circumscribed (next largest) casing. 

	// The major situation/issue to deal with is adjusting by sqrt(2) in the case
	// that the casing is square and is surrounded by two circle casings or vice versa.
	// We also allow adjacent casings of different shapes to get really close (0.01)
	// while casing of the same shape need to be spaced a little (0.05) for UX reasons:
	// if they are too close, it becomes hard to tell there are two casings, or to 
	// click on them reliably to resize them.
	
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
	if (!isDraggingCasing)
		return;

	Point2D mouseLoc = mouseToCaneCoords(event->pos().x(), event->pos().y());
	float radius = getShapeRadius(plan->getCasingShape(draggedCasingIndex), mouseLoc);

	float min;
	float max;

	setMinMaxCasingRadii(&min, &max);	
	plan->setCasingThickness(MIN(MAX(radius, min), max), draggedCasingIndex);
	updateEverything();
	emit someDataChanged();
}

void PullPlanEditorViewWidget :: mouseReleaseEvent(QMouseEvent*)
{
	isDraggingCasing = false;
}

void PullPlanEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
	updateHighlightedSubplansAndCasings(event);
}

void PullPlanEditorViewWidget :: dragMoveEvent(QDragMoveEvent* event)
{
	updateHighlightedSubplansAndCasings(event);
	repaint();
}

void PullPlanEditorViewWidget :: dragLeaveEvent(QDragLeaveEvent*)
{
	subplansHighlighted.clear();
	casingsHighlighted.clear();
}

Point2D PullPlanEditorViewWidget :: mouseToCaneCoords(float x, float y)
{
	Point2D mouseLoc;
	mouseLoc.x = (x - drawUpperLeft.x - squareSize/2) / static_cast<float>(squareSize/2-10);
	mouseLoc.y = (y - drawUpperLeft.y - squareSize/2) / static_cast<float>(squareSize/2-10);

	return mouseLoc;
}

void PullPlanEditorViewWidget :: updateHighlightedSubplansAndCasings(QDragMoveEvent* event)
{
	Point2D mouseLoc = mouseToCaneCoords(event->pos().x(), event->pos().y());

	subplansHighlighted.clear();
	casingsHighlighted.clear();

	// determine highlighted subplan or casing
	void* ptr;
	enum GlassMime::Type type;
	GlassMime::decode(event->mimeData()->text().toAscii().constData(), &ptr, &type);
	switch (type)
	{
		case GlassMime::COLORLIBRARY_MIME:
		{
			GlassColorLibraryWidget* draggedLibraryColor = reinterpret_cast<GlassColorLibraryWidget*>(ptr);
			highlightColor = draggedLibraryColor->glassColor->color();
			int subplanIndexUnderMouse = getSubplanIndexAt(mouseLoc);
			if (subplanIndexUnderMouse != -1)
			{
				// if user is hovering over a subplan and the shift key is currently held down, fill in all subplans
				// Note that this needs to wait until another event (say, a drag move) occurs to catch the shift button being down.
				// The crazy thing is, on Windows a drag *blocks* the event loop, preventing the whole application from
				// getting a keyPressEvent() until the drag is completed. So reading keyboardModifiers() actually 
				// lets you notice that the shift key is down earlier, i.e. during the drag, which is the only time you care anyway.
				if (event && (event->keyboardModifiers() & Qt::ShiftModifier))
				{
					for (unsigned int i = 0; i < plan->subs.size(); ++i)
						subplansHighlighted.insert(i);
				}
				else
					subplansHighlighted.insert(subplanIndexUnderMouse);
				break;
			}
			// If we didn't find a subplan under the mouse, see if there's a casing under it
			int casingIndexUnderMouse = getCasingIndexAt(mouseLoc);
			if (casingIndexUnderMouse == -1)
				break;
			if (event && (event->keyboardModifiers() & Qt::ShiftModifier))
			{
				for (unsigned int i = 0; i < plan->casingCount(); ++i)
					casingsHighlighted.insert(i);
			}
			else
				casingsHighlighted.insert(casingIndexUnderMouse);
			break;
		}
		case GlassMime::COLOR_MIME:
		{
			GlassColor* draggedColor = reinterpret_cast<GlassColor*>(ptr);
			highlightColor = draggedColor->color();
			int casingIndexUnderMouse = getCasingIndexAt(mouseLoc);
			if (casingIndexUnderMouse == -1)
				break;
			if (event && (event->keyboardModifiers() & Qt::ShiftModifier))
			{
				for (unsigned int i = 0; i < plan->casingCount(); ++i)
					casingsHighlighted.insert(i);
			}
			else
				casingsHighlighted.insert(casingIndexUnderMouse);
			break;
		}
		case GlassMime::PULLPLAN_MIME:
		{
			PullPlan* draggedPlan = reinterpret_cast<PullPlan*>(ptr);
			highlightColor.r = highlightColor.g = highlightColor.b = highlightColor.a = 1.0;
			if (draggedPlan->hasDependencyOn(plan))
				break;
			int subplanIndexUnderMouse = getSubplanIndexAt(mouseLoc);
			if (subplanIndexUnderMouse == -1)
				break;
			if (draggedPlan->outermostCasingShape() != plan->subs[subplanIndexUnderMouse].shape)
				break;
			if (event && (event->keyboardModifiers() & Qt::ShiftModifier))
			{
				for (unsigned int i = 0; i < plan->subs.size(); ++i)
				{
					if (draggedPlan->outermostCasingShape() == plan->subs[i].shape)
						subplansHighlighted.insert(i);
				}
			}
			else
				subplansHighlighted.insert(subplanIndexUnderMouse);
			break;
		}
	}	
}

void PullPlanEditorViewWidget :: dropEvent(QDropEvent* event)
{
	// Just so the drop animation looks right
	if (subplansHighlighted.size() == 0 && casingsHighlighted.size() == 0)
		return;		
	else
		event->accept();

	void* ptr;
	enum GlassMime::Type type;
	GlassMime::decode(event->mimeData()->text().toAscii().constData(), &ptr, &type);

	switch (type)
	{
		case GlassMime::COLORLIBRARY_MIME:
		{
			GlassColorLibraryWidget* draggedLibraryColor = reinterpret_cast<GlassColorLibraryWidget*>(ptr);
			for (set<unsigned int>::iterator it = subplansHighlighted.begin(); it != subplansHighlighted.end(); ++it)
			{
				SubpullTemplate& sub = plan->subs[*it];
				switch (sub.shape)
				{
					case CIRCLE_SHAPE:
						sub.plan = draggedLibraryColor->circlePlan;
						break;
					case SQUARE_SHAPE:
						sub.plan = draggedLibraryColor->squarePlan;
						break;
				}
			}
			for (set<unsigned int>::iterator it = casingsHighlighted.begin(); it != casingsHighlighted.end(); ++it)
				plan->setCasingColor(draggedLibraryColor->glassColor, *it);
			break;
		}
		case GlassMime::COLOR_MIME:
		{
			GlassColor* draggedColor = reinterpret_cast<GlassColor*>(ptr);
			for (set<unsigned int>::iterator it = casingsHighlighted.begin(); it != casingsHighlighted.end(); ++it)
				plan->setCasingColor(draggedColor, *it);
			break;
		}
		case GlassMime::PULLPLAN_MIME:
		{
			PullPlan* draggedPlan = reinterpret_cast<PullPlan*>(ptr);
			for (set<unsigned int>::iterator it = subplansHighlighted.begin(); it != subplansHighlighted.end(); ++it)
			{
				SubpullTemplate& sub = plan->subs[*it];
				if (sub.shape == draggedPlan->outermostCasingShape())
					sub.plan = draggedPlan;	
			}
			break;
		}
	}

	subplansHighlighted.clear();
	casingsHighlighted.clear();
	updateEverything();
	emit someDataChanged();
}

void PullPlanEditorViewWidget :: updateEverything()
{
	this->repaint();	
}

void PullPlanEditorViewWidget :: setPullPlan(PullPlan* plan) 
{
	this->plan = plan;
	updateEverything();
}


void PullPlanEditorViewWidget :: setBoundaryPainter(QPainter* painter, bool outermostLevel) 
{
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

void PullPlanEditorViewWidget :: paintShape(Point2D upperLeft, float size, enum GeometricShape shape, QPainter* painter)
{
	int roundedX, roundedY;

	roundedX = upperLeft.x + drawUpperLeft.x + 0.5;
	roundedY = upperLeft.y + drawUpperLeft.y + 0.5;

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

void PullPlanEditorViewWidget :: drawSubplan(Point2D upperLeft, float drawWidth, float drawHeight, 
	PullPlan* plan, bool highlightThis, bool outermostLevel, QPainter* painter) 
{

	// Fill the subplan area with some `cleared out' color
	painter->setBrush(GlobalBackgroundColor::qcolor);
	painter->setPen(Qt::NoPen);
	paintShape(upperLeft, drawWidth, plan->outermostCasingShape(), painter);

	if (highlightThis)
	{
		painter->setBrush(QColor(255*highlightColor.r, 255*highlightColor.g, 255*highlightColor.b,
			255*highlightColor.a));
		painter->setPen(Qt::NoPen);
		paintShape(upperLeft, drawWidth, plan->outermostCasingShape(), painter);
		return;
	}

	// Do casing colors outermost to innermost to get concentric rings of each casing's color
	for (unsigned int i = plan->casingCount() - 1; i < plan->casingCount(); --i) 
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
		
		// Fill with actual casing color (highlighting white or some other color)
		if (outermostLevel && casingsHighlighted.find(i) != casingsHighlighted.end())
		{
			painter->setBrush(QColor(255*highlightColor.r, 255*highlightColor.g, 255*highlightColor.b, 
				255*highlightColor.a));
		}
		else
		{
			Color c = plan->getCasingColor(i)->color();
			QColor qc(255*c.r, 255*c.g, 255*c.b, 255*c.a);
			painter->setBrush(qc);
		}

		setBoundaryPainter(painter, outermostLevel);
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

		drawSubplan(subUpperLeft, rWidth, rHeight, plan->subs[i].plan, 
			outermostLevel && subplansHighlighted.find(i) != subplansHighlighted.end(), false, painter);
		
		painter->setBrush(Qt::NoBrush);
		setBoundaryPainter(painter, outermostLevel); 
		paintShape(subUpperLeft, rWidth, plan->subs[i].shape, painter);
	}
}

void PullPlanEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;

	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(event->rect(), GlobalBackgroundColor::qcolor);
	Point2D upperLeft;
	upperLeft.x = upperLeft.y = 10.0;
	drawSubplan(upperLeft, squareSize - 20, squareSize - 20, plan, false, true, &painter);

	painter.setBrush(Qt::NoBrush);
	setBoundaryPainter(&painter, true);
	paintShape(upperLeft, squareSize - 20, plan->outermostCasingShape(), &painter);

	painter.end();
}
