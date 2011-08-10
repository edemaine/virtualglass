#include "recipeWidget.h"

RecipeWidget::RecipeWidget(QWidget *parent, OpenGLWidget* openglWidget) :
	QTreeWidget(parent)
{
	setColumnCount(7+MAX_AMT_TYPES);
	QStringList headers;
	headers << "Hierarchy" << "Library" << "Type" << "Color" << "Relative X" << "Relative Y" << "Relative Z";
	for (int i=0;i<MAX_AMT_TYPES;i++)
	{
		headers << QString("Amt %1").arg(i);
	}
	setHeaderLabels(headers);
	this->underMouse();
	this->openglWidget = openglWidget;
	this->setSelectionMode(QAbstractItemView::ExtendedSelection);
	connect(this,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(changeData(QTreeWidgetItem*,int)));
	connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(colorPicker(QTreeWidgetItem*,int)));
	connect(this,SIGNAL(recipeCaneChanged()),openglWidget->getModel(),SLOT(exactChange()));
}

void RecipeWidget :: colorPicker(QTreeWidgetItem* item,int column)
{
	if (column!=3)
		return;
	Cane* cane = item->data(0,Qt::UserRole).value<Cane*>();
	QColor selectedColor = QColorDialog::getColor(cane->qcolor(),NULL,"Select New Color",QColorDialog::ShowAlphaChannel);
	if (selectedColor.isValid())
	{
		cane->setColor(selectedColor);
		emit recipeCaneChanged();
		updateBaseRecipe(cane,item,false);
	}
}

void RecipeWidget::updateBaseRecipe(Cane* rootCane, QTreeWidgetItem* rootNode, bool recurse = true)
{
	if (rootCane==NULL)
		return;
	rootNode->setData(0, Qt::UserRole, QVariant::fromValue(rootCane));
	if (rootCane->libraryIndex!=-1)
		rootNode->setText(1,QString("%1").arg(rootCane->libraryIndex));
	else
		rootNode->setText(1,"");

	rootNode->setText(2,QString("%1").arg(rootCane->typeName()));
	rootNode->setBackgroundColor(3,rootCane->qcolor());

	QTreeWidgetItem* parentNode = rootNode->parent();
	int rootIndex = parentNode->indexOfChild(rootNode);
	Cane* parentCane = parentNode->data(0,Qt::UserRole).value<Cane*>();
	Point p = parentCane->subcaneLocations[rootIndex];

	rootNode->setText(4,QString("%1").arg(p.x));
	rootNode->setText(5,QString("%2").arg(p.y));
	rootNode->setText(6,QString("%3").arg(p.z));

	for (int j=0;j<MAX_AMT_TYPES;j++)
	{
		rootNode->setText(7+j,QString("%1").arg(rootCane->amts[j]));
		rootNode->setToolTip(7+j, QString("%1").arg(rootCane->typeAmt(rootCane->type,j)));
		rootNode->setStatusTip(7+j, QString("%1").arg(rootCane->typeAmt(rootCane->type,j)));
	}

	if (!recurse)
		return;

	int numCane = rootCane->subcaneCount;
	for (int i=0;i<numCane;i++)
	{
		updateBaseRecipe(rootCane->subcanes[i],rootNode->child(i),recurse);
	}
}

void RecipeWidget :: changeData(QTreeWidgetItem* item,int column)
{
	if (!this->isVisible())
		return;
	if (column<=2)
		return;
	if (column==3)
		return;
	Cane* cane = item->data(0,Qt::UserRole).value<Cane*>();
	if (column>=4 && column<=6)
	{
		Point p;
		p.x = item->text(4).toFloat();
		p.y = item->text(5).toFloat();
		p.z = item->text(6).toFloat();
		QTreeWidgetItem* itemParent = item->parent();
		int itemIndex = itemParent->indexOfChild(item);
		Cane* caneParent = itemParent->data(0,Qt::UserRole).value<Cane*>();
		caneParent->subcaneLocations[itemIndex] = p;
	}
	else if (column>=7)
	{
		int amtIndex = column-7;
		cane->typeAmt(cane->type,amtIndex);
	}
	emit recipeCaneChanged();
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

		nextLevelCaneWidget->setFlags(nextLevelCaneWidget->flags() | Qt::ItemIsEditable);

		nextLevelCaneWidget->setData(0, Qt::UserRole, QVariant::fromValue(subCane));
		if (subCane->libraryIndex!=-1)
			nextLevelCaneWidget->setText(1,QString("%1").arg(subCane->libraryIndex));
		else
			nextLevelCaneWidget->setText(1,"");
		//nextLevelCaneWidget->setText(2,QString("%1").arg(subCane->type));
		nextLevelCaneWidget->setText(2,QString("%1").arg(subCane->typeName()));
		nextLevelCaneWidget->setBackgroundColor(3,subCane->qcolor());
		nextLevelCaneWidget->setText(4,QString("%1").arg(rootCane->subcaneLocations[i].x));
		nextLevelCaneWidget->setText(5,QString("%2").arg(rootCane->subcaneLocations[i].y));
		nextLevelCaneWidget->setText(6,QString("%3").arg(rootCane->subcaneLocations[i].z));
		for (int j=0;j<MAX_AMT_TYPES;j++)
		{
			nextLevelCaneWidget->setText(7+j,QString("%1").arg(subCane->amts[j]));
			nextLevelCaneWidget->setToolTip(7+j, QString("%1").arg(subCane->typeAmt(subCane->type,j)));
			nextLevelCaneWidget->setStatusTip(7+j, QString("%1").arg(subCane->typeAmt(subCane->type,j)));
		}
		updateRecipe(subCane,nextLevelCaneWidget);
		rootNode->addChild(nextLevelCaneWidget);
	}
	rootNode->setExpanded(false);
}
