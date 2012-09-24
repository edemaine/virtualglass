#include "asyncpullplanlibrarywidget.h"

AsyncPullPlanLibraryWidget :: AsyncPullPlanLibraryWidget(PullPlan *_pullPlan, QWidget *parent) : AsyncRenderWidget(parent), pullPlan(_pullPlan)
{
	setFixedSize(100, 100);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);

	setGraphicsEffect(new QGraphicsHighlightEffect());
	connect(graphicsEffect(), SIGNAL(enabledChanged(bool)), graphicsEffect(), SLOT(setStyleSheet(bool)));
	connect(graphicsEffect(), SIGNAL(styleSheetString(QString)), this, SLOT(setStyleSheet(QString)));

	updatePixmaps();
}

PullPlan* AsyncPullPlanLibraryWidget :: getPullPlan()
{
	return pullPlan;
}

void AsyncPullPlanLibraryWidget :: updatePixmaps()
{
	// This is fast enough to do in real time
	updateDragPixmap();

	//queue up an async update:
	Camera camera;
	camera.eye = make_vector(0.0f, 11.0f, 5.0f);
	camera.lookAt = make_vector(0.0f, 0.0f, 5.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new PullPlanRenderData(pullPlan));
}


const QPixmap* AsyncPullPlanLibraryWidget :: getDragPixmap()
{
	return &(this->dragPixmap);
}

void AsyncPullPlanLibraryWidget :: updateDragPixmap()
{
        QPixmap _pixmap(200, 200);
        _pixmap.fill(Qt::transparent); 

        QPainter painter(&_pixmap);

        drawSubplan(10, 10, 180, 180, pullPlan, true, &painter);

        painter.setBrush(Qt::NoBrush);
        setBoundaryPainter(&painter, true);
        paintShape(10, 10, 180, pullPlan->getOutermostCasingShape(), &painter);

        painter.end();

	dragPixmap = _pixmap.scaled(100, 100);
}

void AsyncPullPlanLibraryWidget :: drawSubplan(float x, float y, float drawWidth, float drawHeight,
        PullPlan* plan, bool outermostLevel, QPainter* painter) {

        // Fill the subplan area with some `cleared out' color
        painter->setBrush(QColor(200, 200, 200));
        painter->setPen(Qt::NoPen);
        paintShape(x, y, drawWidth, plan->getOutermostCasingShape(), painter);

        // Do casing colors outermost to innermost to get concentric rings of each casing's color
        // Skip outermost casing (that is done by your parent) and innermost (that is the `invisible'
        // casing for you to resize your subcanes)
        for (unsigned int i = plan->getCasingCount() - 1; i < plan->getCasingCount(); --i)
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
		Color* c = plan->getCasingColor(i)->getColor();
		QColor qc(255*c->r, 255*c->g, 255*c->b, 255*c->a);
		painter->setBrush(qc);
                setBoundaryPainter(painter, outermostLevel);
                paintShape(casingX, casingY, casingWidth, plan->getCasingShape(i), painter);
        }


        // Recursively call drawing on subplans
        for (unsigned int i = plan->subs.size()-1; i < plan->subs.size(); --i)
        {
                SubpullTemplate* sub = &(plan->subs[i]);

                float rX = x + (sub->location.x - sub->diameter/2.0) * drawWidth/2 + drawWidth/2;
                float rY = y + (sub->location.y - sub->diameter/2.0) * drawWidth/2 + drawHeight/2;
                float rWidth = sub->diameter * drawWidth/2;
                float rHeight = sub->diameter * drawHeight/2;

		drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, outermostLevel, painter);
                setBoundaryPainter(painter, outermostLevel);
                painter->setBrush(Qt::NoBrush);
                paintShape(rX, rY, rWidth, plan->subs[i].shape, painter);
        }
}

void AsyncPullPlanLibraryWidget :: setBoundaryPainter(QPainter* painter, bool outermostLevel) {

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

void AsyncPullPlanLibraryWidget :: paintShape(float x, float y, float size, enum GeometricShape shape, QPainter* painter)
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


