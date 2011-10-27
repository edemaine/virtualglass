
#include "pieceeditorviewwidget.h"

PieceEditorViewWidget :: PieceEditorViewWidget(Piece* piece, QWidget* parent) : QWidget(parent)
{
	width = 500;
	height = 500;	

	setAcceptDrops(true);
	setFixedWidth(width + 20);
	setFixedHeight(height + 20);
	this->piece = piece;
}

void PieceEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PieceEditorViewWidget :: dropEvent(QDropEvent* event)
{
	event->setDropAction(Qt::CopyAction);

	QPoint center;
	center.setX(width/2 + 5);
	center.setY(height/2 + 5);

	if ((event->pos() - center).manhattanLength() > width/4 + height/4)
		return;

        PickupPlan* droppedPickup;
        int type;
        sscanf(event->mimeData()->text().toAscii().constData(), "%p %d", &droppedPickup, &type);
        if (type != PICKUP_PLAN_MIME)
                return;
 
	event->accept();
	piece->pickup = droppedPickup;
	emit someDataChanged();
}

void PieceEditorViewWidget :: setPiece(Piece* piece)
{
	this->piece = piece;
}

void PieceEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
        painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(event->rect(), QColor(200, 200, 200));
	QPen pen;
	pen.setColor(Qt::white);
	pen.setWidth(3);
	painter.setPen(pen);

	painter.drawEllipse(width/8 + 10, height/8 + 10, 3*width/4, 3*height/4);

	for (unsigned int i = 0; i < piece->pickup->getTemplate()->subpulls.size(); ++i)
	{
		if (piece->pickup->subplans[i]->isBase)
		{
			Color c = piece->pickup->subplans[i]->color;
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

                SubpickupTemplate* sp = &(piece->pickup->getTemplate()->subpulls[i]);
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
                        default:
                                exit(1);
                }

		ll.x *= 0.5;
		ll.y *= 0.5;
		rWidth *= 0.5;
		rHeight *= 0.5;

                // Scale to pixels
                ll.x = ll.x * width/2 + width/2 + 10;
                ll.y = ll.y * height/2 + height/2 + 10;
                rWidth *= width/2;
                rHeight *= height/2;

		painter.drawRect(ll.x, ll.y, rWidth, rHeight);
	}
	painter.end();
}



