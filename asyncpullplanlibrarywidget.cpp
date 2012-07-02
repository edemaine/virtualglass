#include "asyncpullplanlibrarywidget.h"
#include "mesh.h"
#include "qgraphicshighlighteffect.h"
#include "pullplanrenderdata.h"
#include "pullplanrenderpixmap.h"

AsyncPullPlanLibraryWidget :: AsyncPullPlanLibraryWidget(PullPlan *_pullPlan, QWidget *parent) : AsyncRenderWidget(parent), pullPlan(_pullPlan)
{
	setFixedSize(100, 100);
	setMouseTracking(true);
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
	editorPixmap = PullPlanRenderPixmap::getEditorPixmap(pullPlan);

	//queue up an async update:
	Camera camera;
	camera.eye = make_vector(0.0f, 11.0f, 5.0f);
	camera.lookAt = make_vector(0.0f, 0.0f, 5.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new PullPlanRenderData(pullPlan));
}

const QPixmap* AsyncPullPlanLibraryWidget :: getEditorPixmap()
{
	return &(this->editorPixmap);
}

