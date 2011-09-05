#include "recipewidget.h"

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
	caneOutdated = true;
	this->openglWidget = openglWidget;
	this->setSelectionMode(QAbstractItemView::SingleSelection);
	connect(this,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(changeData(QTreeWidgetItem*,int)));
	connect(this,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(singleClickEvent(QTreeWidgetItem*,int)));

	connect(this,SIGNAL(recipeCaneChanged()),openglWidget->getModel(),SLOT(exactChange()));
	connect(openglWidget->getModel(), SIGNAL(caneChanged()), this, SLOT(updateRecipe()));
	connect(this, SIGNAL(addOperation(Cane*,int)), openglWidget->getModel(), SLOT(insertMode(Cane*,int)));
	newClear();
}

bool RecipeWidget :: isLibraryCane(QTreeWidgetItem* item)
{
	if (item==NULL)
		return false;
	Cane* cane = getCane(item);
	if (cane==NULL)
		return false;
	if (item->text(1) == ".")
		return false;
	return true;
}

void RecipeWidget :: singleClickEvent(QTreeWidgetItem* item,int column)
{
	caneOutdated = true;
	colorPicker(item,column);
}

void RecipeWidget :: colorPicker(QTreeWidgetItem* item,int column)
{
	if (column!=3)
		return;
	Cane* cane = getCane(item);
	if (cane==NULL)
		return;
	QColor selectedColor = QColorDialog::getColor(cane->qcolor(),NULL,"Select New Color",QColorDialog::ShowAlphaChannel);
	if (selectedColor.isValid())
	{
		cane->setColor(selectedColor);
		emit recipeCaneChanged();
	}
	else
	{
		updateBaseRecipe(cane,item,column);
	}
}

Cane* RecipeWidget::getCane(QTreeWidgetItem* node)
{
	if (node == this->invisibleRootItem() || node == NULL)
	{
		return NULL;
	}
	else if (node == this->visibleRootItem())
	{
		return this->openglWidget->getModel()->getCane();
	}
	else
	{
		if (node == NULL)
			return NULL;
		QVariant data = node->data(0,Qt::UserRole);
		if (data.isNull() || data.isNull() || !data.isValid())
			return NULL;
		return data.value<Cane*>();
	}
}

int RecipeWidget::childIndex(QTreeWidgetItem* item, QTreeWidgetItem* itemParent)
{
	int itemIndex=0;
	if (item!=visibleRootItem() && item!=NULL && itemParent!=NULL)
	{
		itemIndex = itemParent->indexOfChild(item);
	}
	return itemIndex;
}

/*void RecipeWidget::updateBaseRecipe(Cane* rootCane, QTreeWidgetItem* rootNode, bool recurse)
{
	if (rootCane==NULL)
		return;
	rootNode->setData(0, Qt::UserRole, QVariant::fromValue(rootCane));

	updateLibraryColumn(rootCane,rootNode);

	rootNode->setText(2,QString("%1").arg(rootCane->typeName()));
	rootNode->setBackgroundColor(3,rootCane->qcolor());
	rootNode->setText(3,"");

	QTreeWidgetItem* parentNode = rootNode->parent();
	Cane* parentCane = getCane(parentNode);
	int rootIndex = childIndex(rootNode,parentNode);
	if (parentCane != NULL)
	{
		Point p = parentCane->subcaneLocations[rootIndex];

		rootNode->setText(4,QString("%1").arg(p.x));
		rootNode->setText(5,QString("%1").arg(p.y));
		rootNode->setText(6,QString("%1").arg(p.z));
	}
	else
	{
		rootNode->setText(4,"0");
		rootNode->setText(5,"0");
		rootNode->setText(6,"0");
	}

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
*/
void RecipeWidget::updateBaseRecipe(Cane* rootCane, QTreeWidgetItem* rootNode, int column = -1)
{
	if (rootCane==NULL)
		return;
	if (column==-1)
	{
		updateBaseRecipe(rootCane,rootNode,false);
		return;
	}
	QTreeWidgetItem* parentNode;
	int rootIndex;
	Cane* parentCane;
	Point p;
	switch (column)
	{
	case 0:
		rootNode->setData(0, Qt::UserRole, QVariant::fromValue(rootCane));
		break;
	case 1:
		updateLibraryColumn(rootCane,rootNode);
		break;
	case 2:
		rootNode->setText(2,QString("%1").arg(rootCane->typeName()));
		break;
	case 3:
		rootNode->setBackgroundColor(3,rootCane->qcolor());
		rootNode->setText(3,"");
		break;
	case 4:
	case 5:
	case 6:
		parentNode = rootNode->parent();
		if (parentNode == this->invisibleRootItem())
			return;
		rootIndex = childIndex(rootNode,parentNode);
		parentCane = getCane(parentNode);

		if (parentCane != NULL)
		{
			p = parentCane->subcaneLocations[rootIndex];
			rootNode->setText(4,QString("%1").arg(p.x));
			rootNode->setText(5,QString("%1").arg(p.y));
			rootNode->setText(6,QString("%1").arg(p.z));
		}
		break;
	default:
		for (int j=0;j<MAX_AMT_TYPES;j++)
		{
			rootNode->setText(7+j,QString("%1").arg(rootCane->amts[j]));
			rootNode->setToolTip(7+j, QString("%1").arg(rootCane->typeAmt(rootCane->type,j)));
			rootNode->setStatusTip(7+j, QString("%1").arg(rootCane->typeAmt(rootCane->type,j)));
		}
		break;
	}
}

void RecipeWidget :: changeData(QTreeWidgetItem* item,int column)
{
	if (!this->isVisible() || !caneOutdated)
		return;
	Cane* cane = getCane(item);
	if ((column>=4 && column<=6))
	{

			Point p;
			bool fC1,fC2,fC3;
			p.x = item->text(4).toFloat(&fC1);
			p.y = item->text(5).toFloat(&fC2);
			p.z = item->text(6).toFloat(&fC3);
			if (!(fC1 && fC2 && fC3))
			{
				updateBaseRecipe(cane,item,column);
				return;
			}
			QTreeWidgetItem* itemParent=item->parent();
			int itemIndex=0;
			if (item!=visibleRootItem())
			{
				itemIndex = childIndex(item,itemParent);
			}
			Cane* caneParent=getCane(itemParent);
			if (caneParent!=NULL && caneParent->type == BUNDLE_CANETYPE)
			{
				caneParent->subcaneLocations[itemIndex] = p;
			}
			emit recipeCaneChanged();

		updateBaseRecipe(cane,item,column);
		return;
	}
	if (column<=2 || column==3)
	{
		updateBaseRecipe(cane,item,column);
		return;
	} else if (column>=7)
	{
		int amtIndex = column-7;
		QString amtType = cane->typeAmt(cane->type,amtIndex);
		QString undefinedString = QString("Undefined");
		if (amtType.compare(undefinedString)==0)
		{
			item->setText(column,"0");
			return;
		}
		cane->amts[amtIndex] = item->text(column).toFloat();
	}
	updateBaseRecipe(cane,item,false);
	emit recipeCaneChanged();
}

void RecipeWidget :: updateRecipe()
{
	newClear();
	if (openglWidget->getModel()->getCane() == NULL)
	{
		return;
	}
	updateRecipe(openglWidget->getModel()->getCane(), visibleRootItem(), invisibleRootItem());
}

void RecipeWidget :: updateRecipe(bool)
{
	updateRecipe();
}

//column 1
bool RecipeWidget :: updateLibraryColumn(Cane* cane,QTreeWidgetItem* node)
{
	if (cane->libraryIndex!=-1)
	{
		node->setText(1,QString("%1").arg(cane->libraryIndex));
		return true;
	}
	else
	{
		node->setText(1,".");
		return false;
	}
}



void RecipeWidget :: updateRecipe(Cane* rootCane, QTreeWidgetItem* rootNode, QTreeWidgetItem* parentNode)
{
	if (!this->isVisible())
		return;
	if (rootCane == NULL || parentNode == NULL || rootNode == NULL)
		return;
	rootNode->setFlags(rootNode->flags() | Qt::ItemIsEditable);
	rootNode->setData(0, Qt::UserRole, QVariant::fromValue(rootCane));
	updateLibraryColumn(rootCane,rootNode);
	rootNode->setText(2,QString("%1").arg(rootCane->typeName()));
	rootNode->setBackgroundColor(3,rootCane->qcolor());
	rootNode->setText(3,"");

	if (parentNode == invisibleRootItem())
	{
		rootNode->setText(4,QString("%1").arg(0));
		rootNode->setText(5,QString("%1").arg(0));
		rootNode->setText(6,QString("%1").arg(0));
	}
	else
	{
		Cane* parentCane = getCane(parentNode);
		int rootIndex = parentNode->childCount()-1;

		if (parentCane != NULL)
		{
			Point p = parentCane->subcaneLocations[rootIndex];
			rootNode->setText(4,QString("%1").arg(p.x));
			rootNode->setText(5,QString("%1").arg(p.y));
			rootNode->setText(6,QString("%1").arg(p.z));
		}
	}

	for (int j=0;j<MAX_AMT_TYPES;j++)
	{
		rootNode->setText(7+j,QString("%1").arg(rootCane->amts[j]));
		rootNode->setToolTip(7+j, QString("%1").arg(rootCane->typeAmt(rootCane->type,j)));
		rootNode->setStatusTip(7+j, QString("%1").arg(rootCane->typeAmt(rootCane->type,j)));
	}

	int numCane = rootCane->subcaneCount;
	for (int i=0;i<numCane;i++)
	{
		Cane* subCane = rootCane->subcanes[i];
		QTreeWidgetItem* nextLevelCaneWidget = new QTreeWidgetItem(rootNode);
		updateRecipe(subCane,nextLevelCaneWidget,rootNode);
	}

	parentNode->addChild(rootNode);
}

void RecipeWidget :: newClear()
{
	clear();
	invisibleRootItem()->addChild(new QTreeWidgetItem());
	visibleRootItem()->setExpanded(true);
	caneOutdated = false;
}

QTreeWidgetItem* RecipeWidget ::visibleRootItem()
{
	return invisibleRootItem()->child(0);
}
