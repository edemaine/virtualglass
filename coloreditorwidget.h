
#ifndef COLOREDITORWIDGET_H
#define COLOREDITORWIDGET_H

#include <vector>
#include <string>

#include <QWidget>
#include <QString>

#include "geometry.h"
#include "constants.h"

class UndoRedo;
class GlassColor;
class Cane;
class PureColorLibraryWidget;
class NiceViewWidget;
class QVBoxLayout;
class QGridLayout;
class QComboBox;
class QSlider;
class QStackedWidget;

using std::vector;
using std::string;

class ColorEditorWidget : public QWidget
{
	Q_OBJECT

	public:
		ColorEditorWidget(UndoRedo* undoRedo, QWidget* parent=0);
		void resetGlassColor();
		void setGlassColor(GlassColor* _color);
		GlassColor* glassColor() const;
		NiceViewWidget* niceViewWidget;

	public slots:
		void updateEverything();
		void setColorProperties(GlassColor* color);

	private slots:
		void alphaSliderPositionChanged(int);
		void alphaSliderChangeEnded();
		void collectionComboBoxChanged(int);		

	private:
		UndoRedo* undoRedo;
		Geometry geometry;
		GlassColor* glassColor_;	
		QComboBox* collectionComboBox;
		QGridLayout* editorLayout;
		QSlider* alphaSlider;
		vector<QVBoxLayout*> colorLibraryLayouts;
		QStackedWidget* collectionStack;
		int prevCollection;

		bool isDragging;
		QPoint dragStartPosition;
		QPoint lastDragPosition;
		int maxDragDistance;

		void setupLayout();
		void setupConnections();
		void loadCollection(QString fileName);
};


#endif

