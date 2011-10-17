
#ifndef PULLTEMPLATEGRAPHICSVIEW_H
#define PULLTEMPLATEHRAPHICSVIEW_H

#include <QtGui>
#include "pullplan.h"

class PullTemplateGraphicsView : public QWidget
{
	Q_OBJECT

	public:
		PullTemplateGraphicsView(PullPlan* plan, QWidget* parent=0);

	signals:
		void someDataChanged();

	protected:
		void dragEnterEvent(QDragEnterEvent* dee);
		void dropEvent(QDropEvent* de);
		void paintEvent(QPaintEvent *event);

	private:
		PullPlan* plan;
	
};


#endif

