
#include "coloreditorwidget.h"

ColorEditorWidget :: ColorEditorWidget(AsyncColorBarLibraryWidget* libraryWidget, QWidget* parent) : QWidget(parent)
{
	this->niceViewWidget = new NiceViewWidget(PULLPLAN_MODE, this);
	this->libraryWidget = libraryWidget;
	mesher.generateColorMesh(libraryWidget->getPullPlan(), &geometry);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();
}


AsyncColorBarLibraryWidget* ColorEditorWidget :: getLibraryWidget() {

	return this->libraryWidget;
}

void ColorEditorWidget :: setLibraryWidget(AsyncColorBarLibraryWidget* widget) {

	this->libraryWidget = widget;
	this->libraryWidget->updatePixmaps();
}


void ColorEditorWidget :: sourceComboBoxChanged(int)
{
	colorLibrary1ScrollArea->hide();
	colorLibrary2ScrollArea->hide();
	colorLibrary3ScrollArea->hide();
	colorLibrary4ScrollArea->hide();
	colorLibrary5ScrollArea->hide();
	colorLibrary6ScrollArea->hide();

	switch (sourceComboBox->currentIndex())
	{
		case 0: 
			colorLibrary1ScrollArea->show();
			break;			
		case 1: 
			colorLibrary2ScrollArea->show();
			break;			
		case 2: 
			colorLibrary3ScrollArea->show();
			break;			
		case 3: 
			colorLibrary4ScrollArea->show();
			break;			
		case 4: 
			colorLibrary5ScrollArea->show();
			break;			
		case 5: 
			colorLibrary6ScrollArea->show();
			break;			
	} 
}

void ColorEditorWidget :: setupLayout()
{
	QHBoxLayout* pageLayout = new QHBoxLayout(this);
	this->setLayout(pageLayout);
	QVBoxLayout* editorLayout = new QVBoxLayout(this);
	pageLayout->addLayout(editorLayout);

	QHBoxLayout* sourceLayout = new QHBoxLayout(this);
	editorLayout->addLayout(sourceLayout);
	sourceLayout->addWidget(new QLabel("Collection:", this), 0);
	sourceComboBox = new QComboBox(this);
	sourceLayout->addWidget(sourceComboBox, 1);
	sourceComboBox->addItem("Reichenbach - Transparents");
	sourceComboBox->addItem("Reichenbach - Opaques");
	sourceComboBox->addItem("Gaffer - Transparents");
	sourceComboBox->addItem("Gaffer - Opals");
	sourceComboBox->addItem("Kugler - Transparents");
	sourceComboBox->addItem("Kugler - Opaques");

	colorLibrary1ScrollArea = new QScrollArea(this);
	editorLayout->addWidget(colorLibrary1ScrollArea);
	colorLibrary1ScrollArea->setBackgroundRole(QPalette::Dark);
	colorLibrary1ScrollArea->setWidgetResizable(true);
	colorLibrary1ScrollArea->setMinimumWidth(320);
	colorLibrary1ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	colorLibrary1ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	QWidget* colorLibrary1Widget = new QWidget(colorLibrary1ScrollArea);
	colorLibrary1ScrollArea->setWidget(colorLibrary1Widget);
	colorLibrary1Layout = new QVBoxLayout(colorLibrary1Widget);
	colorLibrary1Layout->setSpacing(10);
	colorLibrary1Widget->setLayout(colorLibrary1Layout);
	colorLibrary1ScrollArea->hide();

	colorLibrary2ScrollArea = new QScrollArea(this);
	editorLayout->addWidget(colorLibrary2ScrollArea);
	colorLibrary2ScrollArea->setBackgroundRole(QPalette::Dark);
	colorLibrary2ScrollArea->setWidgetResizable(true);
	colorLibrary2ScrollArea->setMinimumWidth(320);
	colorLibrary2ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	colorLibrary2ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	QWidget* colorLibrary2Widget = new QWidget(colorLibrary2ScrollArea);
	colorLibrary2ScrollArea->setWidget(colorLibrary2Widget);
	colorLibrary2Layout = new QVBoxLayout(colorLibrary2Widget);
	colorLibrary2Layout->setSpacing(10);
	colorLibrary2Widget->setLayout(colorLibrary2Layout);
	colorLibrary2ScrollArea->hide();

	colorLibrary3ScrollArea = new QScrollArea(this);
	editorLayout->addWidget(colorLibrary3ScrollArea);
	colorLibrary3ScrollArea->setBackgroundRole(QPalette::Dark);
	colorLibrary3ScrollArea->setWidgetResizable(true);
	colorLibrary3ScrollArea->setMinimumWidth(320);
	colorLibrary3ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	colorLibrary3ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	QWidget* colorLibrary3Widget = new QWidget(colorLibrary3ScrollArea);
	colorLibrary3ScrollArea->setWidget(colorLibrary3Widget);
	colorLibrary3Layout = new QVBoxLayout(colorLibrary3Widget);
	colorLibrary3Layout->setSpacing(10);
	colorLibrary3Widget->setLayout(colorLibrary3Layout);
	colorLibrary3ScrollArea->hide();

	colorLibrary4ScrollArea = new QScrollArea(this);
	editorLayout->addWidget(colorLibrary4ScrollArea);
	colorLibrary4ScrollArea->setBackgroundRole(QPalette::Dark);
	colorLibrary4ScrollArea->setWidgetResizable(true);
	colorLibrary4ScrollArea->setMinimumWidth(320);
	colorLibrary4ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	colorLibrary4ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	QWidget* colorLibrary4Widget = new QWidget(colorLibrary4ScrollArea);
	colorLibrary4ScrollArea->setWidget(colorLibrary4Widget);
	colorLibrary4Layout = new QVBoxLayout(colorLibrary4Widget);
	colorLibrary4Layout->setSpacing(10);
	colorLibrary4Widget->setLayout(colorLibrary4Layout);
	colorLibrary4ScrollArea->hide();

	colorLibrary5ScrollArea = new QScrollArea(this);
	editorLayout->addWidget(colorLibrary5ScrollArea);
	colorLibrary5ScrollArea->setBackgroundRole(QPalette::Dark);
	colorLibrary5ScrollArea->setWidgetResizable(true);
	colorLibrary5ScrollArea->setMinimumWidth(320);
	colorLibrary5ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	colorLibrary5ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	QWidget* colorLibrary5Widget = new QWidget(colorLibrary5ScrollArea);
	colorLibrary5ScrollArea->setWidget(colorLibrary5Widget);
	colorLibrary5Layout = new QVBoxLayout(colorLibrary5Widget);
	colorLibrary5Layout->setSpacing(10);
	colorLibrary5Widget->setLayout(colorLibrary5Layout);
	colorLibrary5ScrollArea->hide();

	colorLibrary6ScrollArea = new QScrollArea(this);
	editorLayout->addWidget(colorLibrary6ScrollArea);
	colorLibrary6ScrollArea->setBackgroundRole(QPalette::Dark);
	colorLibrary6ScrollArea->setWidgetResizable(true);
	colorLibrary6ScrollArea->setMinimumWidth(320);
	colorLibrary6ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	colorLibrary6ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	QWidget* colorLibrary6Widget = new QWidget(colorLibrary6ScrollArea);
	colorLibrary6ScrollArea->setWidget(colorLibrary6Widget);
	colorLibrary6Layout = new QVBoxLayout(colorLibrary6Widget);
	colorLibrary6Layout->setSpacing(10);
	colorLibrary6Widget->setLayout(colorLibrary6Layout);
	colorLibrary6ScrollArea->hide();

	QHBoxLayout* alphaLayout = new QHBoxLayout(this);
	editorLayout->addLayout(alphaLayout);
	QLabel* alphaLabel1 = new QLabel("Transparency:", this);
	alphaLayout->addWidget(alphaLabel1, 0);
	QLabel* alphaLabel2 = new QLabel("0%", this);
	alphaLayout->addWidget(alphaLabel2, 0);
	alphaSlider = new QSlider(Qt::Horizontal, this);
	alphaSlider->setRange(0, 255);
	alphaLayout->addWidget(alphaSlider);
	QLabel* alphaLabel3 = new QLabel("100%", this);
	alphaLayout->addWidget(alphaLabel3, 0);
	connect(alphaSlider, SIGNAL(valueChanged(int)), this, SLOT(alphaSliderPositionChanged(int)));
	pageLayout->addWidget(niceViewWidget, 10);

	// Little description for the editor
	QLabel* descriptionLabel = new QLabel("Color editor.", this);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(descriptionLabel, 0);
}

void ColorEditorWidget :: setupConnections()
{
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(sourceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sourceComboBoxChanged(int)));
}

void ColorEditorWidget :: seedColors()
{ 
	Color caneColor;
	QString caneName;

	QFile file(":/Colors1.txt");
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	while (!file.atEnd())
	{
		QByteArray line = file.readLine();
		line = line.trimmed();
		if (line.isEmpty())
			continue;

		if (line.at(0) == '[')
		{
			line.remove(0,1);
			line.remove(line.lastIndexOf(']'),1);
			line = line.trimmed();
			caneName = line;
		}
		else if (line.at(0) == '-')
		{
			line.remove(0,1);
			line = line.trimmed();
			QList<QByteArray> colorData = line.split(',');
			caneColor.r = colorData[0].toInt() / 255.0;
			caneColor.g = colorData[1].toInt() / 255.0;
			caneColor.b = colorData[2].toInt() / 255.0;
			caneColor.a = colorData[3].toInt() / 255.0;

			PureColorLibraryWidget* pclw = new PureColorLibraryWidget(caneColor, caneName, this);
			if (colorData[3].toInt() != 255) // if a transparent
			{
				if (caneName[0] ==  'R')
					colorLibrary1Layout->addWidget(pclw);
				else if (caneName[0] == 'G')
					colorLibrary3Layout->addWidget(pclw);
				else if (caneName[0] == 'K')
					colorLibrary5Layout->addWidget(pclw);
			}
			else
			{
				if (caneName[0] ==  'R')
					colorLibrary2Layout->addWidget(pclw);
				else if (caneName[0] == 'G')
					colorLibrary4Layout->addWidget(pclw);
				else if (caneName[0] == 'K')
					colorLibrary6Layout->addWidget(pclw);
			}
		}
	}
	file.close();
}

void ColorEditorWidget :: alphaSliderPositionChanged(int)
{
	if (alphaSlider->sliderPosition() != (int) (libraryWidget->getPullPlan()->getOutermostCasingColor()->a * 255))
	{
		libraryWidget->getPullPlan()->getOutermostCasingColor()->a = (255 - alphaSlider->sliderPosition()) / 255.0;
		emit someDataChanged();
	} 
}

void ColorEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	PureColorLibraryWidget* pclw = dynamic_cast<PureColorLibraryWidget*>(childAt(event->pos()));
	if (pclw != NULL)
	{
		*(libraryWidget->getPullPlan()->getOutermostCasingColor()) = pclw->getColor();
		libraryWidget->setColorName(pclw->getColorName().split(' ')[0]);
		this->alphaSlider->setSliderPosition(255 - int(libraryWidget->getPullPlan()->getOutermostCasingColor()->a * 255));
		emit someDataChanged();	
	}
}

void ColorEditorWidget :: unhighlightLibraryWidget(PureColorLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void ColorEditorWidget :: highlightLibraryWidget(PureColorLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
	w->graphicsEffect()->setEnabled(true);
}

void ColorEditorWidget :: updateEverything()
{
	geometry.clear();
	mesher.generateColorMesh(libraryWidget->getPullPlan(), &geometry);
	niceViewWidget->repaint();
	libraryWidget->updatePixmaps();

	this->alphaSlider->setSliderPosition(255 - (int) (libraryWidget->getPullPlan()->getOutermostCasingColor()->a * 255));

	QLayoutItem* w;
	PureColorLibraryWidget* pclw;
	Color* pColor;
	for (int j = 0; j < colorLibrary6Layout->count(); ++j)
	{
		w = colorLibrary6Layout->itemAt(j);
		pclw = dynamic_cast<PureColorLibraryWidget*>(w->widget());
		pColor = libraryWidget->getPullPlan()->getOutermostCasingColor();
		if (pclw->getColor().r == pColor->r &&
			pclw->getColor().g == pColor->g &&
			pclw->getColor().b == pColor->b)
		{
			highlightLibraryWidget(pclw);
			sourceComboBox->setCurrentIndex(5);
		}
		else
			unhighlightLibraryWidget(pclw);
	}
	for (int j = 0; j < colorLibrary5Layout->count(); ++j)
	{
		w = colorLibrary5Layout->itemAt(j);
		pclw = dynamic_cast<PureColorLibraryWidget*>(w->widget());
		pColor = libraryWidget->getPullPlan()->getOutermostCasingColor();
		if (pclw->getColor().r == pColor->r &&
			pclw->getColor().g == pColor->g &&
			pclw->getColor().b == pColor->b) 
		{
			highlightLibraryWidget(pclw);
			sourceComboBox->setCurrentIndex(4);
		}
		else
			unhighlightLibraryWidget(pclw);
	}
	for (int j = 0; j < colorLibrary4Layout->count(); ++j)
	{
		w = colorLibrary4Layout->itemAt(j);
		pclw = dynamic_cast<PureColorLibraryWidget*>(w->widget());
		pColor = libraryWidget->getPullPlan()->getOutermostCasingColor();
		if (pclw->getColor().r == pColor->r &&
			pclw->getColor().g == pColor->g &&
			pclw->getColor().b == pColor->b)
		{
			highlightLibraryWidget(pclw);
			sourceComboBox->setCurrentIndex(3);
		}
		else
			unhighlightLibraryWidget(pclw);
	}
	for (int j = 0; j < colorLibrary3Layout->count(); ++j)
	{
		w = colorLibrary3Layout->itemAt(j);
		pclw = dynamic_cast<PureColorLibraryWidget*>(w->widget());
		pColor = libraryWidget->getPullPlan()->getOutermostCasingColor();
		if (pclw->getColor().r == pColor->r &&
			pclw->getColor().g == pColor->g &&
			pclw->getColor().b == pColor->b)
		{
			highlightLibraryWidget(pclw);
			sourceComboBox->setCurrentIndex(2);
		}
		else
			unhighlightLibraryWidget(pclw);
	}
	for (int j = 0; j < colorLibrary2Layout->count(); ++j)
	{
		w = colorLibrary2Layout->itemAt(j);
		pclw = dynamic_cast<PureColorLibraryWidget*>(w->widget());
		pColor = libraryWidget->getPullPlan()->getOutermostCasingColor();
		if (pclw->getColor().r == pColor->r &&
			pclw->getColor().g == pColor->g &&
			pclw->getColor().b == pColor->b)
		{
			highlightLibraryWidget(pclw);
			sourceComboBox->setCurrentIndex(1);
		}
		else
			unhighlightLibraryWidget(pclw);
	}
	for (int j = 0; j < colorLibrary1Layout->count(); ++j)
	{
		w = colorLibrary1Layout->itemAt(j);
		pclw = dynamic_cast<PureColorLibraryWidget*>(w->widget());
		pColor = libraryWidget->getPullPlan()->getOutermostCasingColor();
		if (pclw->getColor().r == pColor->r &&
			pclw->getColor().g == pColor->g &&
			pclw->getColor().b == pColor->b) 
		{
			highlightLibraryWidget(pclw);
			sourceComboBox->setCurrentIndex(0);
		}
		else
			unhighlightLibraryWidget(pclw);
	}
}



