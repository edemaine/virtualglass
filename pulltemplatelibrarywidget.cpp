
#include "pulltemplatelibrarywidget.h"

PullTemplateLibraryWidget :: PullTemplateLibraryWidget(QPixmap _view, enum PullTemplate::Type _type, QWidget* _parent): QLabel(_parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);

	setPixmap(_view);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->type = _type;

        setGraphicsEffect(new LibraryItemEffect());
        connect(graphicsEffect(), SIGNAL(enabledChanged(bool)), graphicsEffect(), SLOT(setStyleSheet(bool)));
        connect(graphicsEffect(), SIGNAL(styleSheetString(QString)), this, SLOT(setStyleSheet(QString)));
}


