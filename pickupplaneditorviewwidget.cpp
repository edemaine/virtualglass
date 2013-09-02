
#include <QMouseEvent>
#include <QVBoxLayout>

#include "piece.h"
#include "pickupplaneditorviewwidget.h"
#include "glasscolorlibrarywidget.h"
#include "pullplanlibrarywidget.h"
#include "niceviewwidget.h"
#include "glassmime.h"
#include "glasscolor.h"
#include "pullplancrosssectionrender.h"

PickupPlanEditorViewWidget :: PickupPlanEditorViewWidget(Piece* piece, QWidget* parent) : QWidget(parent)
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

void PickupPlanEditorViewWidget :: setupConnections()
{
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
}

void PickupPlanEditorViewWidget :: getSubplanAt(float x, float y, PullPlan** subplan, int* subplanIndex)
{
	int hitIndex = -1;
	float hitDepth = -100.0;
	for (unsigned int i = 0; i < this->piece->pickupPlan()->subpickupCount(); ++i)
	{
		SubpickupTemplate sp = this->piece->pickupPlan()->getSubpickupTemplate(i);
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
		*subplanIndex = hitIndex;
		*subplan = this->piece->pickupPlan()->getSubpickupTemplate(hitIndex).plan;
	}
	else
	{
		*subplanIndex = -1;
		*subplan = NULL;
	}
}

void PickupPlanEditorViewWidget :: mousePressEvent(QMouseEvent* event)
{
	// Check for convenience subplan-to-subplan drag
	float x = (adjustedX(event->pos().x()) - squareSize/2) / float(squareSize/2-10);
	float y = (adjustedY(event->pos().y()) - squareSize/2) / float(squareSize/2-10);

	int subplanIndex;
	PullPlan* subplan;
	getSubplanAt(x, y, &subplan, &subplanIndex);
	if (subplan != NULL)
	{
		char buf[500];
		sprintf(buf, "%p %d", subplan, GlassMime::PULLPLAN_MIME);
		QByteArray pointerData(buf);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText(pointerData);

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);

		QPixmap pixmap(100, 100);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		PullPlanCrossSectionRender::render(&painter, 100, subplan);
		painter.end();
		drag->setPixmap(pixmap);

		drag->setHotSpot(QPoint(50, 50));
		drag->exec(Qt::CopyAction);
	}
}

void PickupPlanEditorViewWidget :: resizeEvent(QResizeEvent* event)
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

float PickupPlanEditorViewWidget :: adjustedX(float rawX)
{
	return rawX - ulX;
}

float PickupPlanEditorViewWidget :: adjustedY(float rawY)
{
	return rawY - ulY;
}

float PickupPlanEditorViewWidget :: rawX(float adjustedX)
{
	return adjustedX + ulX;
}

float PickupPlanEditorViewWidget :: rawY(float adjustedY)
{
	return adjustedY + ulY;
}

void PickupPlanEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PickupPlanEditorViewWidget :: dropEvent(QDropEvent* event)
{
	void* droppedObject;
	PullPlan* droppedPlan = 0;
	enum GlassMime::Type type;

	GlassMime::decode(event->mimeData()->text().toAscii().constData(), &droppedObject, &type);
	switch (type)
	{
		case GlassMime::COLOR_LIBRARY_MIME:
			droppedPlan = reinterpret_cast<GlassColorLibraryWidget*>(droppedObject)->circlePlan;
			break;
		case GlassMime::PULLPLAN_LIBRARY_MIME:
			droppedPlan = reinterpret_cast<PullPlanLibraryWidget*>(droppedObject)->pullPlan;
			break;
		case GlassMime::PULLPLAN_MIME:
			droppedPlan = reinterpret_cast<PullPlan*>(droppedObject);
			break;
		default:
			return;
	}

	// otherwise it's a pull plan, and we do some complicated things now
	float x = (adjustedX(event->pos().x()) - squareSize/2) / float(squareSize/2-10);
	float y = (adjustedY(event->pos().y()) - squareSize/2) / float(squareSize/2-10);

	int subplanIndex;
	PullPlan* subplan;	
	getSubplanAt(x, y, &subplan, &subplanIndex); 	
	
	if (subplan != NULL)
	{
		event->accept();
		if ((event->keyboardModifiers() & Qt::ShiftModifier))
		{
			for (unsigned int i = 0; i < this->piece->pickupPlan()->subpickupCount(); ++i)
			{
				SubpickupTemplate t = this->piece->pickupPlan()->getSubpickupTemplate(i);
				t.plan = droppedPlan;
				this->piece->pickupPlan()->setSubpickupTemplate(t, i);
			}
		}
		else
		{
			SubpickupTemplate t = this->piece->pickupPlan()->getSubpickupTemplate(subplanIndex);
			t.plan = droppedPlan;
			this->piece->pickupPlan()->setSubpickupTemplate(t, subplanIndex);
		}
		piece->saveState();
		emit someDataChanged();
	}
	else
	{
		event->ignore();
	}
}

void PickupPlanEditorViewWidget :: setPiece(Piece* _piece)
{
	this->piece = _piece;
	updateEverything();
}

void PickupPlanEditorViewWidget :: updateEverything()
{
	this->niceViewWidget->repaint();
}

