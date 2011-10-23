
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

	event->accept();

	PickupPlan* ptr;
	sscanf(event->mimeData()->text().toAscii().constData(), "%p", &ptr);
	piece->setPickup(ptr);	
	emit someDataChanged();
}

void PieceEditorViewWidget :: setPiece(Piece* piece)
{
	this->piece = piece;
}

void PieceEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.setRenderHint(QPainter::HighQualityAntialiasing);
	painter.begin(this);
	painter.fillRect(event->rect(), Qt::black);
	QPen pen;
	pen.setColor(Qt::white);
	pen.setWidth(3);
	painter.setPen(pen);

	painter.drawEllipse(width/8 + 10, height/8 + 10, 3*width/4, 3*height/4);

	for (unsigned int i = 0; i < piece->getPickup()->getTemplate()->subpulls.size(); ++i)
	{
		if (piece->getPickup()->getSubplans()[i]->isBase)
		{
			Color c = piece->getPickup()->getSubplans()[i]->color;
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

		int rX, rY, rWidth, rHeight;
		SubpickupTemplate* sp = &(piece->getPickup()->getTemplate()->subpulls[i]);
		switch (sp->orientation)
		{
			case HORIZONTAL_ORIENTATION:
				rX = (sp->location.x - sp->length/2.0) * width/4 + width/2 + 10;
 				rY = (sp->location.y - sp->width/2.0) * width/4 + height/2 + 10;
 				rWidth = sp->length * width/4;
				rHeight = sp->width * height/4;
				break;
			case VERTICAL_ORIENTATION:
				rX = (sp->location.x - sp->width/2.0) * width/4 + width/2 + 10;
 				rY = (sp->location.y - sp->length/2.0) * width/4 + height/2 + 10;
 				rWidth = sp->width * width/4;
				rHeight = sp->length * height/4;
				break;
			default:
				exit(1);
		}

		painter.drawRect(rX, rY, rWidth, rHeight);
	}
	painter.end();
}



