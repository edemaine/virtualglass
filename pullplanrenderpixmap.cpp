
#include "pullplanrenderpixmap.h"

QPixmap PullPlanRenderPixmap :: getEditorPixmap(PullPlan* plan)
{
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::white);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.fillRect(QRect(0, 0, size, size), QColor(200, 200, 200));
        drawSubplan(10, 10, size - 20, size - 20, plan, plan->getOutermostCasingShape(), true, &painter);

        setBoundaryPainter(&painter, true);
        paintShape(10, 10, size - 20, plan->getOutermostCasingShape(), &painter);

        painter.end();

        return pixmap.scaled(100, 100);
}

void PullPlanRenderPixmap :: drawSubplan(float x, float y, float drawWidth, float drawHeight,
        PullPlan* plan, int mandatedShape, bool outermostLevel, QPainter* painter) {

        // Fill the subplan area with some `cleared out' color
        painter->setBrush(QColor(200, 200, 200));
        painter->setPen(Qt::NoPen);
        paintShape(x, y, drawWidth, mandatedShape, painter);

        // Do casing colors outermost to innermost to get concentric rings of each casing's color
        // Skip outermost casing (that is done by your parent) and innermost (that is the `invisible'
        // casing for you to resize your subcanes)
        for (unsigned int i = plan->getCasingCount() - 1; plan->getCasingCount() > i && i > 0; --i)
        {
                int casingWidth = drawWidth * plan->getCasingThickness(i);
                int casingHeight = drawHeight * plan->getCasingThickness(i);
                int casingX = x + drawWidth / 2 - casingWidth / 2;
                int casingY = y + drawHeight / 2 - casingHeight / 2;

                // Fill with solid neutral grey (in case fill is transparent)
                painter->setBrush(QColor(200, 200, 200));
                painter->setPen(Qt::NoPen); // Will draw boundary after all filling is done
                paintShape(casingX, casingY, casingWidth, plan->getCasingShape(i), painter);

                // Fill with actual casing color (highlighting white or some other color)
		painter->setBrush(QColor(255*plan->getCasingColor(i)->r, 255*plan->getCasingColor(i)->g,
			255*plan->getCasingColor(i)->b, 255*plan->getCasingColor(i)->a));
                setBoundaryPainter(painter, outermostLevel);
                paintShape(casingX, casingY, casingWidth, plan->getCasingShape(i), painter);
        }

        // If you're a color bar, just fill region with color.
        if (plan->isBase())
        {
                Color* c = plan->getOutermostCasingColor();
                painter->setBrush(QColor(255*c->r, 255*c->g, 255*c->b, 255*c->a));
                painter->setPen(Qt::NoPen);
                paintShape(x, y, drawWidth, mandatedShape, painter);
                return;
        }

        // Recursively call drawing on subplans
        for (unsigned int i = plan->subs.size()-1; i < plan->subs.size(); --i)
        {
                SubpullTemplate* sub = &(plan->subs[i]);

                float rX = x + (sub->location.x - sub->diameter/2.0) * drawWidth/2 + drawWidth/2;
                float rY = y + (sub->location.y - sub->diameter/2.0) * drawWidth/2 + drawHeight/2;
                float rWidth = sub->diameter * drawWidth/2;
                float rHeight = sub->diameter * drawHeight/2;

                if (outermostLevel) {
			drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan,
				plan->subs[i].shape, true, painter);
                }
                else {
                        drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan,
                                plan->subs[i].shape, false, painter);
                }

                setBoundaryPainter(painter, outermostLevel);
                painter->setBrush(Qt::NoBrush);
                paintShape(rX, rY, rWidth, plan->subs[i].shape, painter);
        }
}

void PullPlanRenderPixmap :: setBoundaryPainter(QPainter* painter, bool outermostLevel) {

        if (outermostLevel)
        {
                QPen pen;
                pen.setWidth(3);
                pen.setColor(Qt::black);
                painter->setPen(pen);
        }
        else
        {
                QPen pen;
                pen.setWidth(1);
                pen.setColor(Qt::black);
                painter->setPen(pen);
        }

}

void PullPlanRenderPixmap :: paintShape(float x, float y, float size, int shape, QPainter* painter)
{
        switch (shape)
        {
                case CIRCLE_SHAPE:
                        painter->drawEllipse(x, y, size, size);
                        break;
                case SQUARE_SHAPE:
                        painter->drawRect(x, y, size, size);
                        break;
        }

}


