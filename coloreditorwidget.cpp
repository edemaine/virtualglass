
#include <algorithm>

#include <QComboBox>
#include <QStackedWidget>
#include <QFileDialog>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QApplication>
#include <QMouseEvent>
#include <QScrollBar>

#include "cane.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"
#include "purecolorlibrarywidget.h"
#include "glasscolor.h"
#include "coloreditorwidget.h"
#include "glassfileio.h"
#include "globalgraphicssetting.h"
#include "constants.h"

ColorEditorWidget :: ColorEditorWidget(QWidget* parent) : QWidget(parent)
{
	resetGlassColor();

	niceViewWidget = new NiceViewWidget(NiceViewWidget::GLASSCOLOR_CAMERA_MODE, this);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();

	// fake selecting the first list for initialization
	collectionComboBox->setCurrentIndex(0);
}

void ColorEditorWidget :: resetGlassColor()
{
	color = new GlassColor();
}

GlassColor* ColorEditorWidget :: glassColor() 
{
	return color;
}

void ColorEditorWidget :: setGlassColor(GlassColor* _color) 
{
	color = _color;
	updateEverything();
}

void ColorEditorWidget :: collectionComboBoxChanged(int _index)
{
	if (_index < collectionComboBox->count() - 1) // if it's not the ``Add collection...'' one
	{
		prevCollection = _index;
		collectionStack->setCurrentIndex(_index);
	}
	else
	{
		// immediately reset back to previously selected collection, 
		// faking the fact that ``Add collection...'' should not be 
		// a selectable option like the real collections.
		collectionComboBox->setCurrentIndex(prevCollection);
		QString fileName = QFileDialog::getOpenFileName(this, "Open file...", 
			QDir::currentPath(), "VirtualGlass color file (*.vgc)");

		if (!fileName.isNull())
		{
			loadCollection(fileName);
			collectionComboBox->setCurrentIndex(collectionComboBox->count()-2);
		}
	}
}

void ColorEditorWidget :: setupLayout()
{
	QGridLayout* editorLayout = new QGridLayout(this);
	this->setLayout(editorLayout);
	editorLayout->setContentsMargins(0, 0, 0, 0);
	editorLayout->setSpacing(10);

	// Setup collection menu
	collectionComboBox = new QComboBox(this);
	collectionComboBox->setDuplicatesEnabled(true);
	collectionComboBox->addItem("Add collection...");
	editorLayout->addWidget(collectionComboBox, 0, 0);
	
	// Setup stack of collection libraries
	collectionStack = new QStackedWidget(this);
	editorLayout->addWidget(collectionStack, 2, 0);
	loadCollection(":/vgc/reichenbach-opaque-colors.vgc");
	loadCollection(":/vgc/reichenbach-transparent-colors.vgc");
	loadCollection(":/vgc/kugler-opaque-colors.vgc");
	loadCollection(":/vgc/kugler-transparent-colors.vgc");
	loadCollection(":/vgc/gaffer-opaque-colors.vgc");
	loadCollection(":/vgc/gaffer-transparent-colors.vgc");

	// Add alpha (transparency) adjuster
	QWidget* alphaWidget = new QWidget(this);
	alphaWidget->setMinimumHeight(50); // to match legendWidget in MainWindow	
	editorLayout->addWidget(alphaWidget, 3, 0);

	QHBoxLayout* alphaLayout = new QHBoxLayout(alphaWidget);
	alphaLayout->addWidget(new QLabel("Transparency:", alphaWidget), 0);
	alphaLayout->addWidget(new QLabel("0%", alphaWidget));
	alphaSlider = new QSlider(Qt::Horizontal, alphaWidget);
	alphaSlider->setRange(0, 255);
	alphaLayout->addWidget(alphaSlider, 1);
	alphaLayout->addWidget(new QLabel("100%", alphaWidget));

	// Add 3D view	
	editorLayout->addWidget(niceViewWidget, 0, 1, 4, 1);

	// Set relative areas used
	editorLayout->setColumnStretch(1, 1);
}

void ColorEditorWidget :: setupConnections()
{
	connect(collectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(collectionComboBoxChanged(int)));
	connect(alphaSlider, SIGNAL(valueChanged(int)), this, SLOT(alphaSliderPositionChanged(int)));
	connect(alphaSlider, SIGNAL(sliderReleased()), this, SLOT(alphaSliderChangeEnded()));
}

bool compareGlassColors(GlassColor* c1, GlassColor* c2)
{
	string a = c1->shortName();
	string b = c2->shortName();
	if (a.size() != b.size())
		return a.size() < b.size();
	return a <= b;
}

void ColorEditorWidget :: loadCollection(QString fileName)
{
	vector<GlassColor*> colors;
	QString collectionName;

	if(!readColorFile(fileName, collectionName, colors))
		return;
	std::sort(colors.begin(), colors.end(), compareGlassColors);

	// This part sets up the necessary GUI parts 
	// related to the color list.
	collectionComboBox->insertItem(collectionComboBox->count()-1, collectionName);
	QScrollArea* listScrollArea = new QScrollArea(collectionStack);
	collectionStack->addWidget(listScrollArea); // add this to the layout
	listScrollArea->setBackgroundRole(QPalette::Dark);
	listScrollArea->setWidgetResizable(true);
	listScrollArea->setFixedWidth(338);

	listScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	listScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QWidget* colorLibraryWidget = new QWidget(listScrollArea);
	listScrollArea->setWidget(colorLibraryWidget);
	QVBoxLayout* listLayout = new QVBoxLayout(colorLibraryWidget);
	colorLibraryLayouts.push_back(listLayout);
	listLayout->setSpacing(10);
	listLayout->setContentsMargins(10, 10, 10, 10);
	colorLibraryWidget->setLayout(listLayout);

	for (unsigned int i = 0; i < colors.size(); ++i)
	{
		PureColorLibraryWidget* pclw = new PureColorLibraryWidget(colors[i], colorLibraryWidget);
		listLayout->addWidget(pclw);
		connect(pclw, SIGNAL(colorSelected(GlassColor*)), 
			this, SLOT(setColorProperties(GlassColor*)));
		// memory leak here because GlassColor objects are now forgotten about
	}

	listLayout->addStretch(1);
}

void ColorEditorWidget :: alphaSliderChangeEnded()
{
	this->color->saveState();
}

void ColorEditorWidget :: alphaSliderPositionChanged(int)
{
	if (alphaSlider->sliderPosition() == static_cast<int>(color->color().a * 255))
		return;

	Color c = color->color();
	c.a = (255 - alphaSlider->sliderPosition()) / 255.0;
	color->setColor(c);
	updateEverything();
	emit someDataChanged();
}

bool ColorEditorWidget :: canUndo()
{
	return this->color->canUndo();
}

bool ColorEditorWidget :: canRedo()
{
	return this->color->canRedo();
}

void ColorEditorWidget :: undo()
{
	this->color->undo();
	
	updateEverything();
	emit someDataChanged();	
}

void ColorEditorWidget :: redo()
{
	this->color->redo();
	
	updateEverything();
	emit someDataChanged();	
}

void ColorEditorWidget :: setColorProperties(GlassColor* color)
{
	this->color->setColor(color->color());
	this->color->setShortName(color->shortName());
	this->color->setLongName(color->longName());
	this->color->saveState();

	updateEverything();
	emit someDataChanged();	
}

void ColorEditorWidget :: updateEverything()
{
	generateMesh(color, &geometry, GlobalGraphicsSetting::VERY_HIGH);
	niceViewWidget->repaint();

	this->alphaSlider->setSliderPosition(255 - (int) (color->color().a * 255));

	QLayoutItem* w;
	PureColorLibraryWidget* pclw;
	for (unsigned int i = colorLibraryLayouts.size() - 1; i <= colorLibraryLayouts.size(); --i)
	{
		for (int j = 0; j < colorLibraryLayouts[i]->count()-1; ++j)
		{
			// this generically checks that the RGBa values are the same, 
			// but we could do something fancier, checking that the names match
			w = colorLibraryLayouts[i]->itemAt(j);
			pclw = dynamic_cast<PureColorLibraryWidget*>(w->widget());
			if (color->shortName().compare(pclw->shortName()) == 0)
				pclw->setSelected(true);
			else
				pclw->setSelected(false);
		}
	}
}



