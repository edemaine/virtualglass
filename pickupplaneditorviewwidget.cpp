
#include "pickupplaneditorviewwidget.h"

PickupPlanEditorViewWidget :: PickupPlanEditorViewWidget(PickupPlan* pickup, QWidget* parent) : QWidget(parent)
{
	setAcceptDrops(true);
	setFixedSize(500, 500);
	this->pickup = pickup;
	this->fillRule = SINGLE_FILL_RULE;
	this->niceViewWidget = new NiceViewWidget(PICKUPPLAN_MODE, this);
	mesher.generateMesh(pickup, &geometry);
	this->niceViewWidget->setGeometry(&geometry);
	this->niceViewWidget->repaint();

	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setLayout(layout);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(niceViewWidget);
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
	if (type == COLOR_BAR_MIME) // if you drop a color, you're specifying the underlay color
	{
                event->accept();
                if (event->pos().x() < 10 || event->pos().x() > width()-10
			|| event->pos().y() < 10 || event->pos().y() > height()-10)
		{
			pickup->underlayColorPlan->setColor(droppedPlan->getColor());
		}
		else
			pickup->overlayColorPlan->setColor(droppedPlan->getColor());
		emit someDataChanged();
                return;  
	}
        else if (type != PULL_PLAN_MIME) // if the thing passed isn't a pull plan 
                return;  

	int hitIndex = -1;
	float hitDepth = -100.0;
	for (unsigned int i = 0; i < pickup->subtemps.size(); ++i)
	{
		SubpickupTemplate* sp = pickup->subtemps[i];
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
		ll.x = ll.x * (width() - 20)/2 + (width() - 20)/2 + 10;  
		ll.y = ll.y * (height() - 20)/2 + (height() - 20)/2 + 10;  
		ur.x = ur.x * (width() - 20)/2 + (width() - 20)/2 + 10;  
		ur.y = ur.y * (height() - 20)/2 + (height() - 20)/2 + 10;  

		// Need to invert event location, since upper left/lower left origins exist	
                if (ll.x < event->pos().x() && event->pos().x() < ur.x
                        && ll.y < (height() - event->pos().y()) && (height() - event->pos().y()) < ur.y)
		{
			if (hitIndex == -1)
			{
				hitIndex = i;
				hitDepth = pickup->subtemps[i]->location.z;
			}
			else if (hitDepth > pickup->subtemps[i]->location.z)
			{
				hitIndex = i;
				hitDepth = pickup->subtemps[i]->location.z;
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
			pickup->subplans[hitIndex] = droppedPlan;
			break;
		}
		case ALL_FILL_RULE:
		{
			for (unsigned int j = 0; j < pickup->subplans.size(); ++j)
				pickup->subplans[j] = droppedPlan;
			break;
		}
		case GROUP_FILL_RULE:
		{
			int group = pickup->subtemps[hitIndex]->group;
			for (unsigned int j = hitIndex; j < pickup->subtemps.size(); ++j)
			{
				if (pickup->subtemps[j]->group == group)
					pickup->subplans[j] = droppedPlan;
			}
			break;
		}
		case EVERY_OTHER_FILL_RULE:
		{
			bool parity = true;
			int group = pickup->subtemps[hitIndex]->group;
			for (unsigned int j = hitIndex; j < pickup->subtemps.size(); ++j)
			{
				if (pickup->subtemps[j]->group == group)
				{
					if (parity)
						pickup->subplans[j] = droppedPlan;
					parity = !parity;
				}
			}
			break;
		}
		case EVERY_THIRD_FILL_RULE:
		{
			int triarity = 0;
			int group = pickup->subtemps[hitIndex]->group;
			for (unsigned int j = hitIndex; j < pickup->subtemps.size(); ++j)
			{
				if (pickup->subtemps[j]->group == group)
				{
					if (triarity == 0)
						pickup->subplans[j] = droppedPlan;
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

void PickupPlanEditorViewWidget :: paintEvent(QPaintEvent * /*event*/)
{
	this->niceViewWidget->repaint();

	QPainter painter;
	painter.begin(this->niceViewWidget);
        painter.setRenderHint(QPainter::Antialiasing);

	QPen pen;
	pen.setColor(Qt::black);
	pen.setWidth(3);
	painter.setPen(pen);
	
	for (unsigned int i = 0; i < pickup->subtemps.size(); ++i)
	{
		if (pickup->subplans[i]->isBase())
		{
			Color* c = pickup->subplans[i]->getColor();
			painter.setBrush(QColor(255*c->r, 255*c->g, 255*c->b, 255*c->a));
			pen.setStyle(Qt::NoPen);
		}
		else
		{
			painter.setBrush(Qt::NoBrush);
			pen.setColor(Qt::white);
			pen.setStyle(Qt::DotLine);
		}
		painter.setPen(pen);

		SubpickupTemplate* sp = pickup->subtemps[i];
                Point ll;
		float rWidth, rHeight;
                switch (sp->orientation)
                {
                        case HORIZONTAL_ORIENTATION:
                                ll.x = sp->location.x;
                                ll.y = sp->location.y - sp->width/2;
				rWidth = sp->length;
				rHeight = sp->width;
                                break;
                        case VERTICAL_ORIENTATION:
                                ll.x = sp->location.x - sp->width/2;
                                ll.y = sp->location.y;
				rWidth = sp->width;
				rHeight = sp->length;
                                break;
                        case MURRINE_ORIENTATION:
                                ll.x = sp->location.x - sp->width/2;
                                ll.y = sp->location.y - sp->width/2;
				rWidth = sp->width;
				rHeight = sp->width;
                                break;
			default:
				exit(1);
                }

                // Scale to pixels
                ll.x = ll.x * width()/2 + width()/2 + 10;
                ll.y = ll.y * height()/2 + height()/2 + 10;
		rWidth *= width()/2;
		rHeight *= height()/2;
		
		painter.drawRect(ll.x, ll.y, rWidth, rHeight);
	}

	painter.end();
}



