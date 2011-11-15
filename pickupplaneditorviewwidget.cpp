
#include "pickupplaneditorviewwidget.h"

PickupPlanEditorViewWidget :: PickupPlanEditorViewWidget(PickupPlan* plan, Model* model, QWidget* parent) : QWidget(parent)
{
	width = 500;
	height = 500;	

	setAcceptDrops(true);
	setFixedWidth(width + 20);
	setFixedHeight(height + 20);
	this->plan = plan;
	this->model = model;
	this->niceViewWidget = new NiceViewWidget(this);
	this->niceViewWidget->setCameraMode(PICKUPPLAN_MODE);
	this->niceViewWidget->setGeometry(model->getGeometry(this->plan));

	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setLayout(layout);
	layout->addWidget(niceViewWidget);
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

	for (unsigned int i = 0; i < plan->getTemplate()->subpulls.size(); ++i)
	{
		SubpickupTemplate* sp = plan->getTemplate()->subpulls[i];
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
		}	

		// Scale to pixels
		ll.x = ll.x * width/2 + width/2 + 10;  
		ll.y = ll.y * height/2 + height/2 + 10;  
		ur.x = ur.x * width/2 + width/2 + 10;  
		ur.y = ur.y * height/2 + height/2 + 10;  

		// Need to invert event location, since upper left/lower left origins exist	
		if (ll.x < event->pos().x() && event->pos().x() < ur.x 
			&& ll.y < (height + 10 - event->pos().y()) && (height + 10 - event->pos().y()) < ur.y)
		{
			event->accept();
		}
		else
			continue;

		if (droppedPlan->getTemplate()->shape == AMORPHOUS_SHAPE) // if it's a color bar
		{
			// This is a memory leak, as every drag of a color bar makes a new pull plan
			droppedPlan = new PullPlan(CIRCLE_BASE_TEMPLATE, true, droppedPlan->color);
		}

		// If the shift button is down, fill in the entire group
		if (event->keyboardModifiers() & 0x02000000)
		{
			int group = plan->getTemplate()->subpulls[i]->group;
			for (unsigned int j = 0; j < plan->getTemplate()->subpulls.size(); ++j)
			{
				if (plan->getTemplate()->subpulls[j]->group == group)
					plan->subplans[j] = droppedPlan;
			}
		}
		else // Otherwise just fill in this one
			plan->subplans[i] = droppedPlan;

	
		emit someDataChanged();
		return;	
	} 
}

void PickupPlanEditorViewWidget :: setPickupPlan(PickupPlan* plan)
{
	this->plan = plan;
	this->niceViewWidget->setGeometry(model->getGeometry(this->plan));
	this->repaint();
}

void PickupPlanEditorViewWidget :: paintEvent(QPaintEvent * /*event*/)
{
	return;

	QPainter painter;
	painter.begin(this->niceViewWidget);
        painter.setRenderHint(QPainter::Antialiasing);

	QPen pen;
	pen.setColor(Qt::white);
	pen.setWidth(3);
	painter.setPen(pen);
	
	for (unsigned int i = 0; i < plan->getTemplate()->subpulls.size(); ++i)
	{
		if (plan->subplans[i]->isBase)
		{
			Color c = plan->subplans[i]->color;
			painter.setBrush(QColor(255*c.r, 255*c.g, 255*c.b, 255*c.a));
			pen.setStyle(Qt::NoPen);
		}
		else
		{
			painter.setBrush(Qt::NoBrush);
			pen.setColor(Qt::white);
			pen.setStyle(Qt::DotLine);
		}
		painter.setPen(pen);

		SubpickupTemplate* sp = plan->getTemplate()->subpulls[i];
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
                ll.x = ll.x * width/2 + width/2 + 10;
                ll.y = ll.y * height/2 + height/2 + 10;
		rWidth *= width/2;
		rHeight *= height/2;
		
		painter.drawRect(ll.x, ll.y, rWidth, rHeight);
	}

	painter.end();
}



