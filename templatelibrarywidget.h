
#ifndef TEMPLATELIBRARYWIDGET_H
#define TEMPLATELIBRARYWIDGET_H

#include <QLabel>
#include <QPixmap>

class QWidget;

class TemplateLibraryWidget : public QLabel
{
	public:
		TemplateLibraryWidget(QPixmap view, QWidget* parent = NULL);
		void setHighlighted(bool h);
};

#endif 
