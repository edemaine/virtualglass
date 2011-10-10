
#include "pullplanlibrarywidget.h"

PullPlanLibraryWidget :: PullPlanLibraryWidget(NiceViewWidget* niceViewWidget, QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);

	setPixmap(QPixmap::fromImage(niceViewWidget->renderImage()));
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
}

PullPlan* PullPlanLibraryWidget :: getPullPlan()
{
	return pullPlan;
}

#ifdef UNDEF
void PullPlanLibraryWidget :: mousePressEvent(QMouseEvent* event)
{
        if (event->button() == Qt::LeftButton
                && this->geometry().contains(event->pos()))
        {
                QDrag *drag = new QDrag(this);
                QMimeData *mimeData = new QMimeData;
                mimeData->setText("0");
                drag->setMimeData(mimeData);
                drag->setPixmap(*(this->pixmap()));

                //Qt::DropAction dropAction = drag->start();
        }
}

void PullPlanLibraryWidget :: mouseMoveEvent(QMouseEvent* event)
{
        if (!(event->buttons() & Qt::LeftButton))
                return;

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setData(mimeType, data);
        drag->setMimeData(mimeData);

        //Qt::DropAction dropAction = drag->start(Qt::CopyAction | Qt::MoveAction);
}
#endif
