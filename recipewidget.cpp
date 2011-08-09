#include "recipeWidget.h"

RecipeWidget::RecipeWidget(QWidget *parent, OpenGLWidget* openglWidget) :
	QTreeWidget(parent)
{
	setColumnCount(6+MAX_AMT_TYPES);
	QStringList headers;
	headers << "Hierarchy" << "Library" << "Type" << "Color" << "Relative Location";
	for (int i=0;i<MAX_AMT_TYPES;i++)
	{
		headers << QString("Amt %1").arg(i);
	}
	headers << "Data";
	setHeaderLabels(headers);
	this->underMouse();
	this->openglWidget = openglWidget;
}

void RecipeWidget :: updateRecipe()
{
	clear();
	updateRecipe(openglWidget->getModel()->getCane(), this->invisibleRootItem());
}

void RecipeWidget :: updateRecipe(bool recurse)
{
	updateRecipe();
}

//column 0

void RecipeWidget :: updateRecipe(Cane* rootCane, QTreeWidgetItem* rootNode)
{
	int numCane = rootCane->subcaneCount;
	for (int i=0;i<numCane;i++)
	{
		Cane* subCane = rootCane->subcanes[i];
		QTreeWidgetItem* nextLevelCaneWidget = new QTreeWidgetItem(rootNode);
		if (subCane->libraryIndex!=-1)
			nextLevelCaneWidget->setText(1,QString("%1").arg(subCane->libraryIndex));
		else
			nextLevelCaneWidget->setText(1,"");
		//nextLevelCaneWidget->setText(2,QString("%1").arg(subCane->type));
		nextLevelCaneWidget->setText(2,QString("%1").arg(subCane->typeName()));
		nextLevelCaneWidget->setBackgroundColor(3,subCane->qcolor());
		nextLevelCaneWidget->setText(4,QString("(%1, %2, %3)").arg(rootCane->subcaneLocations[i].x).arg(rootCane->subcaneLocations[i].y).arg(rootCane->subcaneLocations[i].z));
		for (int j=0;j<MAX_AMT_TYPES;j++)
		{
			nextLevelCaneWidget->setText(5+j,QString("%1").arg(subCane->amts[j]));
			nextLevelCaneWidget->setToolTip(5+j, QString("%1").arg(subCane->typeAmt(subCane->type,j)));
		}
		nextLevelCaneWidget->setData(5+MAX_AMT_TYPES, Qt::UserRole, QVariant::fromValue(subCane));
		updateRecipe(subCane,nextLevelCaneWidget);
		rootNode->addChild(nextLevelCaneWidget);
	}
	rootNode->setExpanded(false);
}
