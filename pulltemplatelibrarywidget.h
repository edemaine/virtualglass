#ifndef PULLTEMPLATELIBRARYWIDGET_H
#define PULLTEMPLATELIBRARYWIDGET_H

#include <QtGui>
#include <QObject>

class PullTemplateLibraryWidget : public QLabel
{
	Q_OBJECT

	public:
		PullTemplateLibraryWidget(QPixmap view, int pullTemplateType, QWidget* parent=0);
		int getPullTemplateType();

	private:
		int pullTemplateType;
};

#endif
