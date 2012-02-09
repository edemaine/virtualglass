#ifndef ASYNCCOLORBARLIBRARYWIDGET_H
#define ASYNCCOLORBARLIBRARYWIDGET_H

#include "asyncpullplanlibrarywidget.h"

class AsyncColorBarLibraryWidget : public AsyncPullPlanLibraryWidget
{
	Q_OBJECT

	public:
		AsyncColorBarLibraryWidget(PullPlan* plan, QString colorName, QWidget* parent=0);
		QString getColorName();
		void setColorName(QString name);

		void updatePixmaps(QPixmap const &editorPixmap);

		//to overlay color name:
		virtual void paintEvent(QPaintEvent *);

	private:
		QString colorName;
};

#endif
