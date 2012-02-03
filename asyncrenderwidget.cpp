#include "asyncrenderwidget.h"
#include "asyncrenderinternal.h"
#include "geometry.h"

using namespace AsyncRenderInternal;

AsyncRenderWidget::AsyncRenderWidget(QWidget *_parent) : QLabel(_parent), id(0) {
	Controller::controller().registerWidget(this);
}

AsyncRenderWidget::~AsyncRenderWidget() {
	Controller::controller().unregisterWidget(this);
}

void AsyncRenderWidget::renderFinished(Camera const &/*unused: camera */, RenderData *data, Geometry *geometry, QImage *image) {

	delete data;
	delete geometry;
	delete image;
}
