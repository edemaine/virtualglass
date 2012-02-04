
#ifndef COLOREDITORWIDGET_H
#define COLOREDITORWIDGET_H

#include <QtGui>
#include "pullplan.h"
#include "qgraphicshighlighteffect.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"
#include "colorbarlibrarywidget.h"
#include "purecolorlibrarywidget.h"

class ColorEditorWidget : public QWidget
{
	Q_OBJECT

	public:
		ColorEditorWidget(ColorBarLibraryWidget* widget, QWidget* parent=0);
		void setLibraryWidget(ColorBarLibraryWidget* widget);
		ColorBarLibraryWidget* getLibraryWidget();
		void seedColors();
		void updateLibraryWidgetPixmaps();

	signals:
		void someDataChanged();

	protected:
		void mousePressEvent(QMouseEvent* event);
                void highlightLibraryWidget(PureColorLibraryWidget* w);
                void unhighlightLibraryWidget(PureColorLibraryWidget* w);

	public slots:
		void updateEverything();
                void alphaSliderPositionChanged(int);
		void sourceComboBoxChanged(int);		

	private:
		Geometry geometry;
		Mesher mesher;
		ColorBarLibraryWidget* libraryWidget;	
		QComboBox* sourceComboBox;
		NiceViewWidget* niceViewWidget;
                QSlider* alphaSlider;
		QScrollArea* colorLibrary1ScrollArea;
		QScrollArea* colorLibrary2ScrollArea;
		QScrollArea* colorLibrary3ScrollArea;
		QScrollArea* colorLibrary4ScrollArea;
		QScrollArea* colorLibrary5ScrollArea;
		QScrollArea* colorLibrary6ScrollArea;
                QVBoxLayout* colorLibrary1Layout;
                QVBoxLayout* colorLibrary2Layout;
                QVBoxLayout* colorLibrary3Layout;
                QVBoxLayout* colorLibrary4Layout;
                QVBoxLayout* colorLibrary5Layout;
                QVBoxLayout* colorLibrary6Layout;

		void setupLayout();
		void setupConnections();
};


#endif

