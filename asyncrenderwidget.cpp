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

void AsyncRenderWidget::setScene(Camera const &camera, RenderData *data) {
	assert(data);
	Controller::controller().queue(this, camera, data);
}

void AsyncRenderWidget::setCamera(Camera const &) {
	bool functionIsWritten = false;
	assert(functionIsWritten);
}

void AsyncRenderWidget::clearGeometry() {
	bool functionIsWritten = false;
	assert(functionIsWritten);
}

void AsyncRenderWidget::renderFinished(Camera const &/*unused: camera */, RenderData *data, Geometry *geometry, QImage *image) {

	assert(image);

	setPixmap(QPixmap::fromImage(*image).scaled(size()));

	delete data;
	delete geometry;
	delete image;

        static_cast<QGraphicsHighlightEffect*>(graphicsEffect())->setBusy(false);
}


