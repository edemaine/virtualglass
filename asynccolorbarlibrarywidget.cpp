
#include "asynccolorbarlibrarywidget.h"
#include "pullplanrenderdata.h"

AsyncColorBarLibraryWidget :: AsyncColorBarLibraryWidget(PullPlan* _plan, QString _colorName,  QWidget* _parent): AsyncPullPlanLibraryWidget(_plan, _parent), colorName(_colorName)
{
}

QString AsyncColorBarLibraryWidget :: getColorName()
{
	return colorName;
}

void AsyncColorBarLibraryWidget :: setColorName(QString name)
{
	colorName = name;
}

void AsyncColorBarLibraryWidget :: paintEvent(QPaintEvent *event)
{
	AsyncPullPlanLibraryWidget::paintEvent(event);
	QPainter painter(this);
	painter.setPen(QPen(Qt::black));
	painter.drawText(rect().adjusted(5, 0, 0, -2), Qt::AlignBottom | Qt::AlignLeft, colorName);
}

void AsyncColorBarLibraryWidget :: updatePixmaps(QPixmap const &_editorPixmap)
{
	editorPixmap = _editorPixmap;

	//queue up an async update:
	Camera camera;
	camera.eye = make_vector(0.0f, 11.0f, 5.0f);
	camera.lookAt = make_vector(0.0f, 0.0f, 5.0f);
	camera.up = make_vector(0.0f, 0.0f, 1.0f);
	camera.isPerspective = false;
	camera.size = make_vector(300U, 300U);
	setScene(camera, new ColorBarRenderData(pullPlan));
}

