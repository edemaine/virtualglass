
#include "pickupplaneditorviewwidget.h"

PickupPlanEditorViewWidget :: PickupPlanEditorViewWidget(PickupPlan* pickup, QWidget* parent) : QWidget(parent)
{
	setAcceptDrops(true);
	setMinimumSize(200, 200);
	this->pickup = pickup;
	this->fillRule = SINGLE_FILL_RULE;
	this->niceViewWidget = new NiceViewWidget(PICKUPPLAN_MODE, this);
	mesher.generateMesh(pickup, &geometry);
	this->niceViewWidget->setGeometry(&geometry);
	this->niceViewWidget->repaint();

	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setLayout(layout);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(niceViewWidget, 1);
}

PullPlan* PickupPlanEditorViewWidget :: getSubplanAt(float x, float y)
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
		return pickup->subs[hitIndex].plan;
	else
		return NULL;
}

void PickupPlanEditorViewWidget :: mousePressEvent(QMouseEvent* event)
{
        // Check for convenience subplan-to-subplan drag
        float x = (adjustedX(event->pos().x()) - squareSize/2) / float(squareSize/2-10);
        float y = (adjustedY(event->pos().y()) - squareSize/2) / float(squareSize/2-10);

        PullPlan* selectedSubplan = getSubplanAt(x, y);
        if (selectedSubplan != NULL)
        {
		AsyncPullPlanLibraryWidget plplw(selectedSubplan);

                char buf[500];
                sprintf(buf, "%p %d", selectedSubplan, PULL_PLAN_MIME);
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

int PickupPlanEditorViewWidget :: getFillRule()
{
	return this->fillRule;
}

void PickupPlanEditorViewWidget :: setFillRule(int r)
{
	this->fillRule = r;
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
	event->setDropAction(Qt::CopyAction);

        PullPlan* droppedPlan;
        int type;
        sscanf(event->mimeData()->text().toAscii().constData(), "%p %d", &droppedPlan, &type);
        if (type != PULL_PLAN_MIME) // if the thing passed isn't a pull plan 
                return;  

	/*
        float x = (adjustedX(event->pos().x()) - squareSize/2) / float(squareSize/2-10);
        float y = (adjustedY(event->pos().y()) - squareSize/2) / float(squareSize/2-10);
	*/

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

		// Scale to pixels
		ll.x = ll.x * (squareSize - 20)/2 + (squareSize - 20)/2 + 10;  
		ll.y = ll.y * (squareSize - 20)/2 + (squareSize - 20)/2 + 10;  
		ur.x = ur.x * (squareSize - 20)/2 + (squareSize - 20)/2 + 10;  
		ur.y = ur.y * (squareSize - 20)/2 + (squareSize - 20)/2 + 10;  

		// Need to invert event location, since upper left/lower left origins exist	
                if (ll.x < adjustedX(event->pos().x()) && adjustedX(event->pos().x()) < ur.x
                        && ll.y < (squareSize - adjustedY(event->pos().y())) && (squareSize - adjustedY(event->pos().y())) < ur.y)
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
		event->accept();
	else
		return;

	switch (fillRule)
	{
		case SINGLE_FILL_RULE:
		{
			pickup->subs[hitIndex].plan = droppedPlan;
			break;
		}
		case ALL_FILL_RULE:
		{
			for (unsigned int j = 0; j < pickup->subs.size(); ++j)
				pickup->subs[j].plan = droppedPlan;
			break;
		}
		case GROUP_FILL_RULE:
		{
			int group = pickup->subs[hitIndex].group;
			for (unsigned int j = hitIndex; j < pickup->subs.size(); ++j)
			{
				if (pickup->subs[j].group == group)
					pickup->subs[j].plan = droppedPlan;
			}
			break;
		}
		case EVERY_OTHER_FILL_RULE:
		{
			bool parity = true;
			int group = pickup->subs[hitIndex].group;
			for (unsigned int j = hitIndex; j < pickup->subs.size(); ++j)
			{
				if (pickup->subs[j].group == group)
				{
					if (parity)
						pickup->subs[j].plan = droppedPlan;
					parity = !parity;
				}
			}
			break;
		}
		case EVERY_THIRD_FILL_RULE:
		{
			int triarity = 0;
			int group = pickup->subs[hitIndex].group;
			for (unsigned int j = hitIndex; j < pickup->subs.size(); ++j)
			{
				if (pickup->subs[j].group == group)
				{
					if (triarity == 0)
						pickup->subs[j].plan = droppedPlan;
					triarity = (triarity + 1) % 3;
				}
			}
			break;
		}
	}	

	emit someDataChanged();
	return;	
}

void PickupPlanEditorViewWidget :: setPickup(PickupPlan* pickup)
{
	this->pickup = pickup;
	mesher.generateMesh(pickup, &geometry);
	this->niceViewWidget->repaint();
}

