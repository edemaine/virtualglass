
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QMimeData>
#include <QDrag>

#include "piece.h"
#include "pickupeditorviewwidget.h"
#include "glasscolorlibrarywidget.h"
#include "canelibrarywidget.h"
#include "niceviewwidget.h"
#include "glassmime.h"
#include "glasscolor.h"
#include "canecrosssectionrender.h"

PickupEditorViewWidget :: PickupEditorViewWidget(Piece* piece, QWidget* parent) : QWidget(parent)
{
	setAcceptDrops(true);
	setMinimumSize(200, 200);
	this->piece = piece;
	this->niceViewWidget = new NiceViewWidget(NiceViewWidget::PICKUPPLAN_CAMERA_MODE, this);
	this->niceViewWidget->setGeometry(&geometry);

	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setLayout(layout);
	layout->addWidget(niceViewWidget, 1);
	layout->setContentsMargins(0, 0, 0, 0);

	setupConnections();
}

void PickupEditorViewWidget :: setupConnections()
{
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
}

void PickupEditorViewWidget :: getSubcaneAt(float x, float y, Cane** subcane, int* subcaneIndex)
{
	int hitIndex = -1;
	float hitDepth = -100.0;
	for (unsigned int i = 0; i < this->piece->pickup()->subpickupCount(); ++i)
	{
		SubpickupTemplate sp = this->piece->pickup()->getSubpickupTemplate(i);
		Point2D ll, ur;
		ll = ur = make_vector<float>(0.0f, 0.0f);

		switch (sp.orientation)
		{
			case HORIZONTAL_PICKUP_CANE_ORIENTATION:
				ll.x = sp.location.x;
				ll.y = sp.location.y - sp.width/2;
				ur.x = sp.location.x + sp.length;	
				ur.y = sp.location.y + sp.width/2;	
				break;		
			case VERTICAL_PICKUP_CANE_ORIENTATION:
				ll.x = sp.location.x - sp.width/2;	
				ll.y = sp.location.y;	
				ur.x = sp.location.x + sp.width/2;	
				ur.y = sp.location.y + sp.length;	
				break;		
			case MURRINE_PICKUP_CANE_ORIENTATION:
				ll.x = sp.location.x - sp.width/2;	
				ll.y = sp.location.y - sp.width/2;	
				ur.x = sp.location.x + sp.width/2;	
				ur.y = sp.location.y + sp.width/2;	
				break;
		}

		// Need to invert event location, since upper left/lower left origins exist	
		if (ll.x < x && x < ur.x && ll.y < -y && -y < ur.y)
		{
			if (hitIndex == -1)
			{
				hitIndex = i;
				hitDepth = sp.location.z;
			}
			else if (hitDepth > sp.location.z)
			{
				hitIndex = i;
				hitDepth = sp.location.z;
			}
		}
	}

	if (hitIndex != -1)
	{
		*subcaneIndex = hitIndex;
		*subcane = this->piece->pickup()->getSubpickupTemplate(hitIndex).cane;
	}
	else
	{
		*subcaneIndex = -1;
		*subcane = NULL;
	}
}

void PickupEditorViewWidget :: mousePressEvent(QMouseEvent* event)
{
	// Check for convenience subcane-to-subcane drag
	float x = (adjustedX(event->pos().x()) - squareSize/2) / float(squareSize/2-10);
	float y = (adjustedY(event->pos().y()) - squareSize/2) / float(squareSize/2-10);

	int subcaneIndex;
	Cane* subcane;
	getSubcaneAt(x, y, &subcane, &subcaneIndex);
	if (subcane != NULL)
	{
		char buf[500];
		sprintf(buf, "%p %d", subcane, GlassMime::PULLPLAN_MIME);
		QByteArray pointerData(buf);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText(pointerData);

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);

		QPixmap pixmap(100, 100);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		CaneCrossSectionRender::render(&painter, 100, subcane);
		painter.end();
		drag->setPixmap(pixmap);

		drag->setHotSpot(QPoint(50, 50));
		drag->exec(Qt::CopyAction);
	}
}

void PickupEditorViewWidget :: resizeEvent(QResizeEvent* event)
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

float PickupEditorViewWidget :: adjustedX(float rawX)
{
	return rawX - ulX;
}

float PickupEditorViewWidget :: adjustedY(float rawY)
{
	return rawY - ulY;
}

float PickupEditorViewWidget :: rawX(float adjustedX)
{
	return adjustedX + ulX;
}

float PickupEditorViewWidget :: rawY(float adjustedY)
{
	return adjustedY + ulY;
}

void PickupEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PickupEditorViewWidget :: dropEvent(QDropEvent* event)
{
	void* droppedObject;
	Cane* droppedPlan = 0;
	enum GlassMime::Type type;

	GlassMime::decode(event->mimeData()->text().toStdString().c_str(), &droppedObject, &type);
	switch (type)
	{
		case GlassMime::COLOR_LIBRARY_MIME:
			droppedPlan = reinterpret_cast<GlassColorLibraryWidget*>(droppedObject)->circleCane;
			break;
		case GlassMime::PULLPLAN_LIBRARY_MIME:
			droppedPlan = reinterpret_cast<CaneLibraryWidget*>(droppedObject)->cane;
			break;
		case GlassMime::PULLPLAN_MIME:
			droppedPlan = reinterpret_cast<Cane*>(droppedObject);
			break;
		default:
			return;
	}

	// otherwise it's a pull cane, and we do some complicated things now
	float x = (adjustedX(event->pos().x()) - squareSize/2) / float(squareSize/2-10);
	float y = (adjustedY(event->pos().y()) - squareSize/2) / float(squareSize/2-10);

	int subcaneIndex;
	Cane* subcane;	
	getSubcaneAt(x, y, &subcane, &subcaneIndex); 	
	
	if (subcane != NULL)
	{
		event->accept();
		if ((event->keyboardModifiers() & Qt::ShiftModifier))
		{
			for (unsigned int i = 0; i < this->piece->pickup()->subpickupCount(); ++i)
			{
				SubpickupTemplate t = this->piece->pickup()->getSubpickupTemplate(i);
				t.cane = droppedPlan;
				this->piece->pickup()->setSubpickupTemplate(t, i);
			}
		}
		else
		{
			SubpickupTemplate t = this->piece->pickup()->getSubpickupTemplate(subcaneIndex);
			t.cane = droppedPlan;
			this->piece->pickup()->setSubpickupTemplate(t, subcaneIndex);
		}
		piece->saveState();
		emit someDataChanged();
	}
	else
	{
		event->ignore();
	}
}

void PickupEditorViewWidget :: setPiece(Piece* _piece)
{
	this->piece = _piece;
	updateEverything();
}

void PickupEditorViewWidget :: updateEverything()
{
	this->niceViewWidget->repaint();
}

