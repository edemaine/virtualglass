
#include "pickupplaneditorviewwidget.h"

PickupPlanEditorViewWidget :: PickupPlanEditorViewWidget(Piece* piece, Model* model, QWidget* parent) : QWidget(parent)
{
	setAcceptDrops(true);
	setFixedSize(400, 400);
	this->piece = piece;
	this->model = model;
	this->niceViewWidget = new NiceViewWidget(PICKUPPLAN_MODE, this);
	this->niceViewWidget->setGeometry(model->getGeometry(piece->pickup));

	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setLayout(layout);
	layout->setContentsMargins(0, 0, 0, 0);
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

	for (unsigned int i = 0; i < piece->pickup->getTemplate()->subtemps.size(); ++i)
	{
		SubpickupTemplate* sp = piece->pickup->getTemplate()->subtemps[i];
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
		ll.x = ll.x * (width() - 20)/2 + (width() - 20)/2 + 10;  
		ll.y = ll.y * (height() - 20)/2 + (height() - 20)/2 + 10;  
		ur.x = ur.x * (width() - 20)/2 + (width() - 20)/2 + 10;  
		ur.y = ur.y * (height() - 20)/2 + (height() - 20)/2 + 10;  

		// Need to invert event location, since upper left/lower left origins exist	
                if (ll.x < event->pos().x() && event->pos().x() < ur.x
                        && ll.y < (height() - event->pos().y()) && (height() - event->pos().y()) < ur.y)
		{
			event->accept();
		}
		else
			continue;

		// If the shift button is down, fill in the entire group
		if (event->keyboardModifiers() & 0x02000000)
		{
			int group = piece->pickup->getTemplate()->subtemps[i]->group;
			for (unsigned int j = 0; j < piece->pickup->getTemplate()->subtemps.size(); ++j)
			{
				if (piece->pickup->getTemplate()->subtemps[j]->group == group)
					piece->pickup->subplans[j] = droppedPlan;
			}
		}
                // If the alt button is down, fill alternating elements in the group
                else if (event->keyboardModifiers() & 0x08000000)
                {
                        int group = piece->pickup->getTemplate()->subtemps[i]->group;
                        bool parity = true;
                        unsigned int subtempCount = piece->pickup->getTemplate()->subtemps.size();
                        unsigned int j = i;
                        do
                        {
                                if (piece->pickup->getTemplate()->subtemps[j]->group == group)
				{
					if (parity)
						piece->pickup->subplans[j] = droppedPlan;
					parity = !parity;
				}
                                ++j;
                                j = j % subtempCount;
                        }
                        while ((j != ((i-1 + subtempCount) % subtempCount)) && (j != i));
                }
		else // Otherwise just fill in this one
			piece->pickup->subplans[i] = droppedPlan;

		emit someDataChanged();
		return;	
	} 
}

void PickupPlanEditorViewWidget :: setPiece(Piece* piece)
{
	this->piece = piece;
	this->niceViewWidget->setGeometry(model->getGeometry(piece->pickup));
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
	
	for (unsigned int i = 0; i < piece->pickup->getTemplate()->subtemps.size(); ++i)
	{
		if (piece->pickup->subplans[i]->getTemplate()->isBase())
		{
			Color* c = piece->pickup->subplans[i]->color;
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

		SubpickupTemplate* sp = piece->pickup->getTemplate()->subtemps[i];
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



