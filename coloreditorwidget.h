
#ifndef COLOREDITORWIDGET_H
#define COLOREDITORWIDGET_H

#include <vector>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QString>

class GlassColor;
class PullPlan;
class Mesher;
class Geometry;
class PureColorLibraryWidget;
class NiceViewWidget;
class QVBoxLayout;
class QComboBox;
class QSlider;
class QStackedWidget;

using std::vector;
             
class ColorEditorWidget : public QWidget
{
	Q_OBJECT

	public:
		ColorEditorWidget(GlassColor* _color, QWidget* parent=0);
		void setGlassColor(GlassColor* _color);
		GlassColor* getGlassColor();

	signals:
		void someDataChanged();

	protected:
		void mousePressEvent(QMouseEvent* event);

	public slots:
		void updateEverything();
                void alphaSliderPositionChanged(int);
		void collectionComboBoxChanged(int);		

	private:
		Geometry* geometry;
		Mesher* mesher;
		GlassColor* glassColor;	
		QComboBox* collectionComboBox;
		NiceViewWidget* niceViewWidget;
		QVBoxLayout* editorLayout;
                QSlider* alphaSlider;
		vector<QScrollArea*> colorLibraryScrollAreas;
		vector<QVBoxLayout*> colorLibraryLayouts;
		QStackedWidget* collectionStack;
		int prevCollection;

		void setupLayout();
		void setupConnections();
		void loadCollection(QString fileName);
};


#endif

