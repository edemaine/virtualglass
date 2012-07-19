
#include "pickupplaneditorviewwidget.h"

PickupPlanEditorViewWidget :: PickupPlanEditorViewWidget(PickupPlan* pickup, QWidget* parent) : QWidget(parent)
{
	setAcceptDrops(true);
	setMinimumSize(200, 200);
	this->pickup = pickup;
	this->niceViewWidget = new NiceViewWidget(PICKUPPLAN_MODE, this);
	mesher.generateMesh(pickup, &geometry);
	this->niceViewWidget->setGeometry(&geometry);
	this->niceViewWidget->repaint();

	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setLayout(layout);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(niceViewWidget, 1);
}

void PickupPlanEditorViewWidget :: getSubplanAt(float x, float y, PullPlan** subplan, int* subplanIndex)
{
	int hitIndex = -1;
	float hitDepth = -100.0;
	for (unsigned int i = 0; i < pickup->subs.size(); ++i)
	{
		SubpickupTemplate* sp = &(pickup->subs[i]);
		Point ll, ur;

		switch (sp->orientation)
		{
			case HORIZONTAL_ORIENTATION:
				ll.x = sp->location.x;
				ll.y = sp->location.y - sp->width/2;
				ur.x = sp->location.x + sp->length;	
				ur.y = sp->location.y + sp->width/2;	
				break;		
			case VERTICAL_ORIENTATION:
				ll.x = sp->location.x - sp->width/2;	
				ll.y = sp->location.y;	
				ur.x = sp->location.x + sp->width/2;	
				ur.y = sp->location.y + sp->length;	
				break;		
			case MURRINE_ORIENTATION:
				ll.x = sp->location.x - sp->width/2;	
				ll.y = sp->location.y - sp->width/2;	
				ur.x = sp->location.x + sp->width/2;	
				ur.y = sp->location.y + sp->width/2;	
				break;
			default:
				//some sort of "safe"-ish default.
				ll.x = 0.0;
				ll.y = 0.0;
				ur.x = 0.0;
				ur.y = 0.0;
				break;
		}	

		// Need to invert event location, since upper left/lower left origins exist	
                if (ll.x < x && x < ur.x && ll.y < -y && -y < ur.y)
		{
			if (hitIndex == -1)
			{
				hitIndex = i;
				hitDepth = pickup->subs[i].location.z;
			}
			else if (hitDepth > pickup->subs[i].location.z)
			{
				hitIndex = i;
				hitDepth = pickup->subs[i].location.z;
			}
		}
	}

	if (hitIndex != -1)
	{
		*subplanIndex = hitIndex;
		*subplan = pickup->subs[hitIndex].plan;
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
		AsyncPullPlanLibraryWidget plplw(subplan);

                char buf[500];
                sprintf(buf, "%p %d", subplan, PULL_PLAN_MIME);
                QByteArray pointerData(buf);
                QMimeData* mimeData = new QMimeData;
                mimeData->setText(pointerData);

                QDrag *drag = new QDrag(this);
                drag->setMimeData(mimeData);
                drag->setPixmap(*(plplw.getDragPixmap()));

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

QPixmap PickupPlanEditorViewWidget :: getPixmap()
{
	return QPixmap::fromImage(niceViewWidget->renderImage());
}

void PickupPlanEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PickupPlanEditorViewWidget :: dropEvent(QDropEvent* event)
{
        PullPlan* droppedPlan;
        int type;

        decodeMimeData(event->mimeData()->text().toAscii().constData(), &droppedPlan, &type);
        if (type != PULL_PLAN_MIME) // if the thing passed isn't a pull plan 
                return;  
	
        float x = (adjustedX(event->pos().x()) - squareSize/2) / float(squareSize/2-10);
        float y = (adjustedY(event->pos().y()) - squareSize/2) / float(squareSize/2-10);

	int subplanIndex;
	PullPlan* subplan;	
	getSubplanAt(x, y, &subplan, &subplanIndex); 	
	
	if (subplan != NULL)
	{
		event->accept();
		pickup->subs[subplanIndex].plan = droppedPlan;
		emit someDataChanged();
	}
	else
	{
		event->ignore();
	}
}

void PickupPlanEditorViewWidget :: setPickup(PickupPlan* pickup)
{
	this->pickup = pickup;
	mesher.generateMesh(pickup, &geometry);
	this->niceViewWidget->repaint();
}

