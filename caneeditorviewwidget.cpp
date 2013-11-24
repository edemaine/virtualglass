
#include <QResizeEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QPainter>
#include <QMimeData>
#include <QDrag>

#include "constants.h"
#include "glasscolor.h"
#include "cane.h"
#include "glassmime.h"
#include "canelibrarywidget.h"
#include "glasscolorlibrarywidget.h"
#include "caneeditorviewwidget.h"
#include "globalbackgroundcolor.h"
#include "canecrosssectionrender.h"
#include "undoredo.h"

CaneEditorViewWidget :: CaneEditorViewWidget(Cane* cane, UndoRedo* undoRedo, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setMinimumSize(200, 200);
	this->cane = cane;
	this->undoRedo = undoRedo;
	isDraggingCasing = false;
}


QRect CaneEditorViewWidget :: usedRect()
{
	return QRect(drawUpperLeft.x, drawUpperLeft.y, squareSize, squareSize);
}

void CaneEditorViewWidget :: resizeEvent(QResizeEvent* event)
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

float CaneEditorViewWidget :: shapeRadius(enum GeometricShape shape, Point2D loc)
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

bool CaneEditorViewWidget :: isOnCasing(int casingIndex, Point2D loc)
{
	return fabs(cane->casingThickness(casingIndex) - shapeRadius(cane->casingShape(casingIndex), loc)) < 0.025; 
}

void CaneEditorViewWidget :: mousePressEvent(QMouseEvent* event)
{
	Point2D mouseLoc = mouseToCaneCoords(event->pos().x(), event->pos().y());

	// Check for casing resize
	for (unsigned int i = 0; i < cane->casingCount() - 1; ++i) 
	{
		if (isOnCasing(i, mouseLoc))
		{
			isDraggingCasing = true; 
			draggedCasingIndex = i;
			return;
		}	
	}

	// Check for convenience subcane-to-subcane drag
	Cane* selectedSubcane = subcaneAt(mouseLoc);
	if (selectedSubcane != NULL)
	{
	        char buf[500];
		GlassMime::encode(buf, selectedSubcane, GlassMime::PULLPLAN_MIME);
		QByteArray pointerData(buf);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText(pointerData);

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);

		QPixmap pixmap(100, 100);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		CaneCrossSectionRender::render(&painter, 100, selectedSubcane);
		painter.end();
		drag->setPixmap(pixmap);

		drag->setHotSpot(QPoint(50, 50));
		drag->exec(Qt::CopyAction);

		return;
	}	

	// Check for convenience casing-to-casing drag
	int selectedCasingIndex = casingIndexAt(mouseLoc);
	if (selectedCasingIndex != -1)
	{
		const GlassColor* selectedColor = cane->casingColor(static_cast<unsigned int>(selectedCasingIndex)); 

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
		drag->setHotSpot(QPoint(50, 50));

		drag->exec(Qt::CopyAction);

		return;
	}
}

int CaneEditorViewWidget :: casingIndexAt(Point2D loc)
{
	for (unsigned int i = 0; i < cane->casingCount(); ++i) 
	{
		if (shapeRadius(cane->casingShape(i), loc) < cane->casingThickness(i))
			return i;
	}

	return -1;
}

int CaneEditorViewWidget :: subcaneIndexAt(Point2D loc)
{
	// Recursively call drawing on subcanes
	for (unsigned int i = 0; i < cane->subpullCount(); ++i)
	{
		SubcaneTemplate sub = cane->subcaneTemplate(i);
		Point2D delta;
		delta.x = loc.x - sub.location.x;
		delta.y = loc.y - sub.location.y;
		if (shapeRadius(sub.shape, delta) < sub.diameter/2.0)
			return i;
	}

	return -1;			
}

Cane* CaneEditorViewWidget :: subcaneAt(Point2D loc)
{
	int subcaneIndex = subcaneIndexAt(loc);
	if (subcaneIndex == -1)
		return NULL;
	return cane->subcaneTemplate(subcaneIndex).cane;
}

void CaneEditorViewWidget :: setMinMaxCasingRadii(float* min, float* max)
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
		csi = cane->casingShape(0);
		csi_minus_1 = csi;
		csi_plus_1 = cane->casingShape(1);
	
		cti_minus_1 = 0.0;
		cti_plus_1 = cane->casingThickness(1);	
	}
	else 
	{
		csi = cane->casingShape(draggedCasingIndex);
		csi_minus_1 = cane->casingShape(draggedCasingIndex-1);
		csi_plus_1 = cane->casingShape(draggedCasingIndex+1);
		cti_minus_1 = cane->casingThickness(draggedCasingIndex-1);
		cti_plus_1 = cane->casingThickness(draggedCasingIndex+1);
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

void CaneEditorViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
	if (!isDraggingCasing)
		return;

	Point2D mouseLoc = mouseToCaneCoords(event->pos().x(), event->pos().y());
	float radius = shapeRadius(cane->casingShape(draggedCasingIndex), mouseLoc);

	float min;
	float max;

	setMinMaxCasingRadii(&min, &max);	
	cane->setCasingThickness(MIN(MAX(radius, min), max), draggedCasingIndex);
	updateEverything();
	emit someDataChanged();
}

void CaneEditorViewWidget :: mouseReleaseEvent(QMouseEvent*)
{
	isDraggingCasing = false;
	undoRedo->modifiedCane(cane);
	updateEverything();
	emit someDataChanged();
}

void CaneEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
	updateHighlightedSubcanesAndCasings(event);
}

void CaneEditorViewWidget :: dragMoveEvent(QDragMoveEvent* event)
{
	updateHighlightedSubcanesAndCasings(event);
	repaint();
}

void CaneEditorViewWidget :: dragLeaveEvent(QDragLeaveEvent*)
{
	subcanesHighlighted.clear();
	casingsHighlighted.clear();
}

Point2D CaneEditorViewWidget :: mouseToCaneCoords(float x, float y)
{
	Point2D mouseLoc;
	mouseLoc.x = (x - drawUpperLeft.x - squareSize/2) / static_cast<float>(squareSize/2-10);
	mouseLoc.y = (y - drawUpperLeft.y - squareSize/2) / static_cast<float>(squareSize/2-10);

	return mouseLoc;
}

void CaneEditorViewWidget :: updateHighlightedSubcanesAndCasings(QDragMoveEvent* event)
{
	Point2D mouseLoc = mouseToCaneCoords(event->pos().x(), event->pos().y());

	subcanesHighlighted.clear();
	casingsHighlighted.clear();

	// determine highlighted subcane or casing
	void* ptr;
	enum GlassMime::Type type;
	GlassMime::decode(event->mimeData()->text().toStdString().c_str(), &ptr, &type);
	switch (type)
	{
		case GlassMime::COLOR_LIBRARY_MIME:
		{
			GlassColorLibraryWidget* draggedLibraryColor 
				= reinterpret_cast<GlassColorLibraryWidget*>(ptr);
			highlightColor = draggedLibraryColor->glassColor->color();
			int subcaneIndexUnderMouse = subcaneIndexAt(mouseLoc);
			if (subcaneIndexUnderMouse != -1)
			{
				// if user is hovering over a subcane and the shift key is currently held down, fill in all subcanes
				// Note that this needs to wait until another event (say, a drag move) occurs to catch the shift button being down.
				// The crazy thing is, on Windows a drag *blocks* the event loop, preventing the whole application from
				// getting a keyPressEvent() until the drag is completed. So reading keyboardModifiers() actually 
				// lets you notice that the shift key is down earlier, i.e. during the drag, which is the only time you care anyway.
				if (event && (event->keyboardModifiers() & Qt::ShiftModifier))
				{
					for (unsigned int i = 0; i < cane->subpullCount(); ++i)
						subcanesHighlighted.insert(i);
				}
				else
					subcanesHighlighted.insert(subcaneIndexUnderMouse);
				break;
			}
			// If we didn't find a subcane under the mouse, see if there's a casing under it
			int casingIndexUnderMouse = casingIndexAt(mouseLoc);
			if (casingIndexUnderMouse == -1)
				break;
			if (event && (event->keyboardModifiers() & Qt::ShiftModifier))
			{
				for (unsigned int i = 0; i < cane->casingCount(); ++i)
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
			int casingIndexUnderMouse = casingIndexAt(mouseLoc);
			if (casingIndexUnderMouse == -1)
				break;
			if (event && (event->keyboardModifiers() & Qt::ShiftModifier))
			{
				for (unsigned int i = 0; i < cane->casingCount(); ++i)
					casingsHighlighted.insert(i);
			}
			else
				casingsHighlighted.insert(casingIndexUnderMouse);
			break;
		}
		case GlassMime::PULLPLAN_MIME:
		case GlassMime::PULLPLAN_LIBRARY_MIME:
		{
			Cane* draggedCane;
			if (type == GlassMime::PULLPLAN_MIME)
				draggedCane = reinterpret_cast<Cane*>(ptr);
			else 
				draggedCane = reinterpret_cast<CaneLibraryWidget*>(ptr)->cane;
			highlightColor.r = highlightColor.g = highlightColor.b = highlightColor.a = 1.0;
			if (draggedCane->hasDependencyOn(cane))
				break;
			int subcaneIndexUnderMouse = subcaneIndexAt(mouseLoc);
			if (subcaneIndexUnderMouse == -1)
				break;
			if (draggedCane->outermostCasingShape() != cane->subcaneTemplate(subcaneIndexUnderMouse).shape)
				break;
			if (event && (event->keyboardModifiers() & Qt::ShiftModifier))
			{
				for (unsigned int i = 0; i < cane->subpullCount(); ++i)
				{
					if (draggedCane->outermostCasingShape() == cane->subcaneTemplate(i).shape)
						subcanesHighlighted.insert(i);
				}
			}
			else
				subcanesHighlighted.insert(subcaneIndexUnderMouse);
			break;
		}
		default:
			break;
	}	
}

void CaneEditorViewWidget :: dropEvent(QDropEvent* event)
{
	// Just so the drop animation looks right
	if (subcanesHighlighted.size() == 0 && casingsHighlighted.size() == 0)
		return;		
	else
		event->accept();

	void* ptr;
	enum GlassMime::Type type;
	GlassMime::decode(event->mimeData()->text().toStdString().c_str(), &ptr, &type);

	switch (type)
	{
		case GlassMime::COLOR_LIBRARY_MIME:
		{
			GlassColorLibraryWidget* draggedLibraryColor = reinterpret_cast<GlassColorLibraryWidget*>(ptr);
			for (set<unsigned int>::iterator it = subcanesHighlighted.begin(); it != subcanesHighlighted.end(); ++it)
			{
				SubcaneTemplate sub = cane->subcaneTemplate(*it);
				switch (sub.shape)
				{
					case CIRCLE_SHAPE:
						sub.cane = draggedLibraryColor->circleCane;
						break;
					case SQUARE_SHAPE:
						sub.cane = draggedLibraryColor->squareCane;
						break;
				}
				cane->setSubcaneTemplate(sub, *it);
			}
			for (set<unsigned int>::iterator it = casingsHighlighted.begin(); it != casingsHighlighted.end(); ++it)
				cane->setCasingColor(draggedLibraryColor->glassColor, *it);
			break;
		}
		case GlassMime::COLOR_MIME:
		{
			GlassColor* draggedColor = reinterpret_cast<GlassColor*>(ptr);
			for (set<unsigned int>::iterator it = casingsHighlighted.begin(); it != casingsHighlighted.end(); ++it)
				cane->setCasingColor(draggedColor, *it);
			break;
		}
		case GlassMime::PULLPLAN_MIME:
		case GlassMime::PULLPLAN_LIBRARY_MIME: 
		{
			Cane* draggedCane;
			if (type == GlassMime::PULLPLAN_MIME)
				draggedCane = reinterpret_cast<Cane*>(ptr);
			else
				draggedCane = reinterpret_cast<CaneLibraryWidget*>(ptr)->cane;
			for (set<unsigned int>::iterator it = subcanesHighlighted.begin(); it != subcanesHighlighted.end(); ++it)
			{
				SubcaneTemplate sub = cane->subcaneTemplate(*it);
				if (sub.shape == draggedCane->outermostCasingShape())
				{
					sub.cane = draggedCane;	
					cane->setSubcaneTemplate(sub, *it);
				}
			}
			break;
		}
		default:
			break;
	}

	subcanesHighlighted.clear();
	casingsHighlighted.clear();
	undoRedo->modifiedCane(cane);
	updateEverything();
	emit someDataChanged();
}

void CaneEditorViewWidget :: updateEverything()
{
	this->repaint();	
}

void CaneEditorViewWidget :: setCane(Cane* cane) 
{
	this->cane = cane;
	updateEverything();
}


void CaneEditorViewWidget :: setBoundaryPainter(QPainter* painter, bool outermostLevel) 
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

void CaneEditorViewWidget :: paintShape(Point2D upperLeft, float size, enum GeometricShape shape, QPainter* painter)
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

void CaneEditorViewWidget :: drawSubcane(Point2D upperLeft, float drawWidth, float drawHeight, 
	Cane* cane, bool highlightThis, bool outermostLevel, QPainter* painter) 
{

	// Fill the subcane area with some `cleared out' color
	painter->setBrush(GlobalBackgroundColor::qcolor);
	painter->setPen(Qt::NoPen);
	paintShape(upperLeft, drawWidth, cane->outermostCasingShape(), painter);

	if (highlightThis)
	{
		painter->setBrush(QColor(255*highlightColor.r, 255*highlightColor.g, 255*highlightColor.b,
			255*highlightColor.a));
		painter->setPen(Qt::NoPen);
		paintShape(upperLeft, drawWidth, cane->outermostCasingShape(), painter);
		return;
	}

	// Do casing colors outermost to innermost to get concentric rings of each casing's color
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
		
		// Fill with actual casing color (highlighting white or some other color)
		if (outermostLevel && casingsHighlighted.find(i) != casingsHighlighted.end())
		{
			painter->setBrush(QColor(255*highlightColor.r, 255*highlightColor.g, 255*highlightColor.b, 
				255*highlightColor.a));
		}
		else
		{
			Color c = cane->casingColor(i)->color();
			QColor qc(255*c.r, 255*c.g, 255*c.b, 255*c.a);
			painter->setBrush(qc);
		}

		setBoundaryPainter(painter, outermostLevel);
		paintShape(casingUpperLeft, casingWidth, cane->casingShape(i), painter);
	}

	// Recursively call drawing on subcanes
	for (unsigned int i = cane->subpullCount()-1; i < cane->subpullCount(); --i)
	{
		SubcaneTemplate sub = cane->subcaneTemplate(i);

		Point2D subUpperLeft;
		subUpperLeft.x = upperLeft.x + (sub.location.x - sub.diameter/2.0) * drawWidth/2 + drawWidth/2;
		subUpperLeft.y = upperLeft.y + (sub.location.y - sub.diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = sub.diameter * drawWidth/2;
		float rHeight = sub.diameter * drawHeight/2;

		drawSubcane(subUpperLeft, rWidth, rHeight, sub.cane, 
			outermostLevel && subcanesHighlighted.find(i) != subcanesHighlighted.end(), false, painter);
		
		painter->setBrush(Qt::NoBrush);
		setBoundaryPainter(painter, outermostLevel); 
		paintShape(subUpperLeft, rWidth, sub.shape, painter);
	}
}

void CaneEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;

	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(event->rect(), GlobalBackgroundColor::qcolor);
	Point2D upperLeft;
	upperLeft.x = upperLeft.y = 10.0;
	drawSubcane(upperLeft, squareSize - 20, squareSize - 20, cane, false, true, &painter);

	painter.setBrush(Qt::NoBrush);
	setBoundaryPainter(&painter, true);
	paintShape(upperLeft, squareSize - 20, cane->outermostCasingShape(), &painter);

	painter.end();
}
