

#include "pulltemplategraphicsview.h"

PullTemplateGraphicsView :: PullTemplateGraphicsView(QGraphicsScene* scene, QWidget* parent) : QGraphicsView(scene, parent)
{
	setAcceptDrops(true);
	setFixedWidth(410);
	setFixedHeight(410);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ensureVisible(0, 0, 400, 400, 5, 5);
}




