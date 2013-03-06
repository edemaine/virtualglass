
#include <QComboBox>
#include <QStackedWidget>
#include <QFileDialog>
#include <QMouseEvent>
#include <algorithm>
#include "pullplan.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"
#include "purecolorlibrarywidget.h"
#include "glasscolor.h"
#include "coloreditorwidget.h"
#include "glassfileio.h"

ColorEditorWidget :: ColorEditorWidget(QWidget* parent) : QWidget(parent)
{
	resetGlassColor();

	geometry = new Geometry();

	niceViewWidget = new NiceViewWidget(NiceViewWidget::PULLPLAN_CAMERA_MODE, this);
	generateMesh(glassColor, geometry, 4);
	niceViewWidget->setGeometry(geometry);

	setupLayout();
	setupConnections();

	// fake selecting the first list for initialization
	collectionComboBox->setCurrentIndex(0);
}

void ColorEditorWidget :: resetGlassColor()
{
	glassColor = new GlassColor();
}

GlassColor* ColorEditorWidget :: getGlassColor() {

	return glassColor;
}

void ColorEditorWidget :: setGlassColor(GlassColor* _gc) 
{
	glassColor = _gc;
	emit someDataChanged();
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
		QString fileName = QFileDialog::getOpenFileName(this, "Load Color Library", "", 
			"VirtualGlass Color Libraries (*.vgc)");

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

	// Setup collection menu
	QWidget* colMenuWidget = new QWidget(this); 
	QHBoxLayout* colMenuLayout = new QHBoxLayout(colMenuWidget);
	colMenuWidget->setLayout(colMenuLayout);
	colMenuLayout->addWidget(new QLabel("Collection:", colMenuWidget));
	collectionComboBox = new QComboBox(colMenuWidget);
	collectionComboBox->setDuplicatesEnabled(true);
	collectionComboBox->addItem("Add collection...");
	colMenuLayout->addWidget(collectionComboBox, 1);
	editorLayout->addWidget(colMenuWidget, 0, 0);
	
	// Setup stack of collection libraries
	collectionStack = new QStackedWidget(this);
	editorLayout->addWidget(collectionStack, 2, 0);
	loadCollection(":/reichenbach-opaque-colors.vgc");
	loadCollection(":/reichenbach-transparent-colors.vgc");
	loadCollection(":/kugler-opaque-colors.vgc");
	loadCollection(":/kugler-transparent-colors.vgc");
	loadCollection(":/gaffer-opaque-colors.vgc");
	loadCollection(":/gaffer-transparent-colors.vgc");

	// Add alpha (transparency) adjuster
	QWidget* alphaWidget = new QWidget(this);
	QHBoxLayout* alphaLayout = new QHBoxLayout(alphaWidget);
	alphaLayout->addWidget(new QLabel("Transparency", alphaWidget), 0);
	alphaLayout->addWidget(new QLabel("0%", alphaWidget));
	alphaSlider = new QSlider(Qt::Horizontal, alphaWidget);
	alphaSlider->setRange(0, 255);
	alphaLayout->addWidget(alphaSlider, 1);
	alphaLayout->addWidget(new QLabel("100%", alphaWidget));
	editorLayout->addWidget(alphaWidget, 3, 0);

	// Little description for the editor
	QLabel* descriptionLabel = new QLabel("Color editor.", this);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(descriptionLabel, 4, 0);

	// Add 3D view	
	editorLayout->addWidget(niceViewWidget, 0, 1, 4, 1);

	// Set relative areas used
	//editorLayout->setRowStretch(1, 1);
	editorLayout->setColumnStretch(1, 1);

}

void ColorEditorWidget :: setupConnections()
{
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
	connect(collectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(collectionComboBoxChanged(int)));
	connect(alphaSlider, SIGNAL(valueChanged(int)), this, SLOT(alphaSliderPositionChanged(int)));
}

bool compareGlassColors(GlassColor* c1, GlassColor* c2)
{
	string a = c1->getShortName();
	string b = c2->getShortName();
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
	colorLibraryScrollAreas.push_back(listScrollArea); // push the new scroll area
	collectionStack->addWidget(listScrollArea); // add this to the layout
	listScrollArea->setBackgroundRole(QPalette::Dark);
	listScrollArea->setWidgetResizable(true);
	listScrollArea->setFixedWidth(340);
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
		PureColorLibraryWidget* pclw = new PureColorLibraryWidget(colors[i], this);
		listLayout->addWidget(pclw);
		// memory leak here because GlassColor objects are now forgotten about
	}

	listLayout->addStretch(1);
}

void ColorEditorWidget :: alphaSliderPositionChanged(int)
{
	if (alphaSlider->sliderPosition() != (int) (glassColor->getColor().a * 255))
	{
		Color c = glassColor->getColor();
		c.a = (255 - alphaSlider->sliderPosition()) / 255.0;
		glassColor->setColor(c);
		emit someDataChanged();
	} 
}

void ColorEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	PureColorLibraryWidget* pclw = dynamic_cast<PureColorLibraryWidget*>(childAt(event->pos()));
	if (pclw != NULL)
	{
		glassColor->setColor(pclw->getColor());
		glassColor->setShortName(pclw->getShortName());
		glassColor->setLongName(pclw->getLongName());
		this->alphaSlider->setSliderPosition(255 - int(glassColor->getColor().a * 255));
		emit someDataChanged();	
	}
}

void ColorEditorWidget :: updateEverything()
{
	geometry->clear();
	generateMesh(glassColor, geometry, 4);
	niceViewWidget->repaint();

	this->alphaSlider->setSliderPosition(255 - (int) (glassColor->getColor().a * 255));

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
			if (glassColor->getShortName().compare(pclw->getShortName()) == 0)
				pclw->setSelected(true);
			else
				pclw->setSelected(false);
		}
	}
}



