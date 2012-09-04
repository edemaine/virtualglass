#ifndef PULLTEMPLATELIBRARYWIDGET_H
#define PULLTEMPLATELIBRARYWIDGET_H

#include <QtGui>

class PullTemplateLibraryWidget : public QLabel
{
	public:
		PullTemplateLibraryWidget(QPixmap view, int pullTemplateType, QWidget* parent=0);
		int getPullTemplateType();

	private:
		int pullTemplateType;
};

#endif
