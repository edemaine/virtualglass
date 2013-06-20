
#ifndef COLOREDITORWIDGET_H
#define COLOREDITORWIDGET_H

#include <vector>
#include <string>

#include <QWidget>
#include <QString>

#include "geometry.h"
#include "constants.h"

class GlassColor;
class PullPlan;
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
		ColorEditorWidget(QWidget* parent=0);
		void resetGlassColor();
		void setGlassColor(GlassColor* _color);
		void updateEverything();
		GlassColor* glassColor();
		NiceViewWidget* niceViewWidget;

	signals:
		void someDataChanged();

	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);

	private slots:
		void alphaSliderPositionChanged(int);
		void collectionComboBoxChanged(int);		

	private:
		Geometry geometry;
		GlassColor* color;	
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

