#ifndef RECIPEWIDGET_H
#define RECIPEWIDGET_H

#include <QtGui>
#include "openglwidget.h"
#include "constants.h"
#include "primitives.h"
#include "librarycanewidget.h"
#include "cane.h"
#include "model.h"

class RecipeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	RecipeWidget(QWidget* parent, OpenGLWidget* openglWidget);

signals:
	void recipeCaneChanged();

public slots:
	void updateRecipe();
	void updateRecipe(bool recurse);
	void updateRecipe(Cane* rootCane, QTreeWidgetItem* rootNode, bool isInLibrary = false);
	void updateBaseRecipe(Cane* rootCane, QTreeWidgetItem* rootNode, bool recurse);
	void updateBaseRecipe(Cane* rootCane, QTreeWidgetItem* rootNode, int column);
	void changeData(QTreeWidgetItem* item,int column);
	void singleClickEvent(QTreeWidgetItem* item,int column);
	void doubleClickEvent(QTreeWidgetItem* item,int column);

private:
	OpenGLWidget* openglWidget;
	void newClear();
	QTreeWidgetItem* visibleRootItem();
	Cane* getCane(QTreeWidgetItem* node);
	bool updateLibraryColumn(Cane* cane,QTreeWidgetItem* node, bool isInLibrary = false);
	void colorPicker(QTreeWidgetItem* item,int column);
	bool isLibraryCane(QTreeWidgetItem* item);
	bool isTopLibraryCane(QTreeWidgetItem* item);
	Point getCoordinate(Point p);
	Point getPoint(QTreeWidgetItem* item);
	void setPoint(QTreeWidgetItem* item, Point p);
	int childIndex(QTreeWidgetItem* item, QTreeWidgetItem* itemParent);
	bool caneOutdated;

};

#endif // RECIPEVIEW_H
